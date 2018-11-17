extern crate pnet;

use pnet::datalink::{self, NetworkInterface};
use pnet::datalink::Channel::Ethernet;
//use pnet::packet::Packet; //, MutablePacket};
use pnet::packet::ethernet::EthernetPacket;

use std::env;
use std::thread;
use std::time::Instant;
use std::time::Duration;
use std::sync::mpsc::channel;
use std::sync::Arc;
use std::sync::Mutex;
use std::iter::Iterator;

extern crate rand;
use rand::Rng;
use rand::distributions::{Distribution, Uniform};
#[macro_use]
extern crate lazy_static;

enum Action {
	Forward,
	Drop,
	Delay,
	Corrupt,
}

struct DelayedPacket {
	packet: Vec<u8>,
	until: Instant,
}

lazy_static! {
	// probability of event in %
    static ref Drop_P: u16 = 5;
    static ref Delay_P: u16 = 10;
    static ref Corrupt_P: u16 = 5;

    static ref Max_Delay_Time: u16 = 10;	//sec

    static ref Prob: Uniform<u16> = Uniform::from(0..100);
}

        
fn get_action() -> Action
{
    let mut rng = rand::thread_rng();

    let p = Prob.sample(&mut rng);
    if p <= *Corrupt_P {
    	return Action::Corrupt;
    }

    let p = Prob.sample(&mut rng);
    if p <= *Drop_P {
    	return Action::Drop;
    }

    let p = Prob.sample(&mut rng);
	if p <= *Delay_P {
		return Action::Delay;
	}

	Action::Forward
}


fn corrupt(packet: &[u8]) -> Vec<u8>
{
	let mut p : Vec<u8> = Vec::from(packet);
	let ix = rand::thread_rng().gen_range(0, p.len());
	p[ix] = rand::thread_rng().gen_range(0,255);
	p
}


fn process_packet(packet: &[u8], delayed: &Arc<Mutex<Vec<DelayedPacket>>>) -> Option<Vec<u8>>
{
	match get_action() {
		Action::Forward => {
				println!("forward");
      			Some(packet.to_vec())
      	},
               
        Action::Corrupt => {
				println!("corrupt");
      			Some(corrupt(packet))
        },
        
        Action::Delay => {
		        let t = rand::thread_rng().gen_range(1, *Max_Delay_Time);
				println!("delay {}",t);
				let d = DelayedPacket { packet: packet.to_vec(), until: Instant::now() + Duration::from_secs(t.into()) };
				delayed.lock().unwrap().push(d);
				None
        },
        
        Action::Drop => {
				println!("drop");
				None
        },
        		
	}
}	



fn main()
{
    let interface1 = env::args().nth(1).expect("source interface name");
    let interface2 = env::args().nth(2).expect("destination interface name");

    let interface1 = datalink::interfaces().into_iter()
                              .filter(|iface: &NetworkInterface| iface.name == interface1)
                              .next()
                              .unwrap();
    let interface2 = datalink::interfaces().into_iter()
                              .filter(|iface: &NetworkInterface| iface.name == interface2)
                              .next()
                              .unwrap();

    let (mut tx1, mut rx1) = match datalink::channel(&interface1, Default::default()) {
        Ok(Ethernet(tx1, rx1)) => (tx1, rx1),
        Ok(_) => panic!("Unhandled channel type"),
        Err(e) => panic!("An error occurred when creating the datalink channel: {}", e)
    };

    let (mut tx2, mut rx2) = match datalink::channel(&interface2, Default::default()) {
        Ok(Ethernet(tx2, rx2)) => (tx2, rx2),
        Ok(_) => panic!("Unhandled channel type"),
        Err(e) => panic!("An error occurred when creating the datalink channel: {}", e)
    };

	let (ctx1_tx, ctx1_rx) = channel::<Vec<u8>>();
	let (ctx2_tx, ctx2_rx) = channel::<Vec<u8>>();
	let (crx1_tx, crx1_rx) = channel::<Vec<u8>>();
	let (crx2_tx, crx2_rx) = channel::<Vec<u8>>();

	let delayed1 = Arc::new(Mutex::new(Vec::new()));
	let delayed2 = Arc::new(Mutex::new(Vec::new()));

	// receiving
	thread::spawn(move || {
    	loop {
        	match rx1.next() {
            	Ok(packet) => {
                	let pkt = EthernetPacket::new(packet).unwrap();
					println!("1->2 {:?}",pkt);
					crx1_tx.send(packet.to_vec()).unwrap();
            	},
            	Err(e) => {
                	panic!("An error occurred while reading: {}", e);
            	}
        	}
    	}
	});

	thread::spawn(move || {
    	loop {
        match rx2.next() {
            Ok(packet) => {
               	let pkt = EthernetPacket::new(packet).unwrap();
				println!("2->1 {:?}",pkt);
                crx2_tx.send(packet.to_vec()).unwrap();
            },
            Err(e) => {
                panic!("An error occurred while reading: {}", e);
            }
        }
		}
	});
	
	// sending
	thread::spawn(move || {
    	loop {
    		match ctx1_rx.recv() {
	    		Ok(packet) => {
					tx1.send_to(&packet,None);
            	},
            	Err(e) => {
                	panic!("An error occurred while sending: {}", e);
            	}
    		}
		}
	});
	
	thread::spawn(move || {
    	loop {
    		match ctx2_rx.recv() {
	    		Ok(packet) => {
					tx2.send_to(&packet,None);
            	},
            	Err(e) => {
                	panic!("An error occurred while sending: {}", e);
            	}
    		}
		}
	});

	// processing
	let delayed1p = Arc::clone(&delayed1);
	let ctx2_tx_s = ctx2_tx.clone();
	thread::spawn(move || {
    	loop {
    		match crx1_rx.recv() {
	    		Ok(packet) => {
	    			if let Some(pkt) = process_packet(&packet,&delayed1p) {
						ctx2_tx_s.send(pkt).unwrap();
					}
            	},
            	Err(e) => {
                	panic!("An error occurred while processing: {}", e);
            	}
    		}
		}
	});

	let delayed2p = Arc::clone(&delayed2);
	let ctx1_tx_s = ctx1_tx.clone();
	thread::spawn(move || {
    	loop {
    		match crx2_rx.recv() {
	    		Ok(packet) => {
	    			if let Some(pkt) = process_packet(&packet,&delayed2p) {
						ctx1_tx_s.send(pkt).unwrap();
					}
            	},
            	Err(e) => {
                	panic!("An error occurred while processing: {}", e);
            	}
    		}
		}
	});

	// delay
    loop {
		thread::sleep(Duration::from_millis(1000));
		let now = Instant::now();
		
		{
			let mut delayed1 = delayed1.lock().unwrap();
			loop {
				let mut index = 0;
				let mut found = false;
				if let Some((ix,pkt)) = delayed1.iter().enumerate().find(|(_, d)| d.until < now) {
					ctx2_tx.send(pkt.packet.clone()).unwrap();
					index = ix;
					found = true;
					println!("send delayed");
				}
				if !found {
					break;
				}
				delayed1.remove(index);
			}
		}

		{
			let mut delayed2 = delayed2.lock().unwrap();
			loop {
				let mut index = 0;
				let mut found = false;
				if let Some((ix,pkt)) = delayed2.iter().enumerate().find(|(_, d)| d.until <= now) {
					ctx2_tx.send(pkt.packet.clone()).unwrap();
					index = ix;
					found = true;
					println!("send delayed");
				}
				if !found {
					break;
				}
				delayed2.remove(index);
			}
		}
    }
}
