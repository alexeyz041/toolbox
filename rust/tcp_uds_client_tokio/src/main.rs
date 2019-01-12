
extern crate futures;
extern crate tokio;
extern crate tokio_timer;

use futures::Sink;
use std::net::SocketAddr;
use tokio::runtime::Runtime;
use futures::future::*;
use futures::Stream;
use tokio::net::TcpStream;
use tokio::net::unix::UnixStream;
use tokio::codec::Framed;
use tokio::codec::LinesCodec;
use futures::sync::mpsc::unbounded;
use futures::sync::mpsc::UnboundedReceiver;
use futures::sync::mpsc::UnboundedSender;
use std::sync::Arc;
use futures::Future;


fn open_uds(rt: &mut Runtime, addr: &'static str) -> (Arc<UnboundedSender<String>>, UnboundedReceiver<String>)
{
    let (sender_rx,rcvr_rx) = unbounded::<String>();
    let (sender_tx,rcvr_tx) = unbounded::<String>();

	let pf = UnixStream::connect(addr)
		.map_err(|e| println!("error 1 {}",e))
		.inspect(move |_| println!("successfully connected to {}", addr))
		.and_then(move |sock| {
		    let s = Framed::new(sock, LinesCodec::new());
		    let (wr,rd) = s.split();

		    let sending = wr
	    			.sink_map_err(|e| println!("error 2 {}",e))
	    			.send_all(rcvr_tx)
	    			.and_then(|_| Ok(()));
				
		    let receiving = rd
		    		.for_each(move |line| {
						sender_rx.unbounded_send(line).unwrap();
		    	    	Ok(())
				    })
					.map_err(|e| println!("error 3 {}",e))
	    			.and_then(|_| Ok(()));
				
				receiving
					.join(sending)
					.map(|(a,_)| a)
			});
	rt.spawn(pf);		
	(Arc::new(sender_tx), rcvr_rx)
}


fn open_tcp(rt: &mut Runtime, addr: &str) -> (Arc<UnboundedSender<String>>, UnboundedReceiver<String>)
{
	let addr = addr.parse::<SocketAddr>().unwrap();

    let (sender_rx,rcvr_rx) = unbounded::<String>();
    let (sender_tx,rcvr_tx) = unbounded::<String>();

	let pf = TcpStream::connect(&addr)
		.map_err(|e| println!("error 1 {}",e))
		.inspect(move |_| println!("successfully connected to {}", addr))
		.and_then(move |sock| {
		    let s = Framed::new(sock, LinesCodec::new());
		    let (wr,rd) = s.split();

		    let sending = wr
	    			.sink_map_err(|e| println!("error 2 {}",e))
	    			.send_all(rcvr_tx)
	    			.and_then(|_| Ok(()));
				
		    let receiving = rd
		    		.for_each(move |line| {
						sender_rx.unbounded_send(line).unwrap();
		    	    	Ok(())
				    })
					.map_err(|e| println!("error 3 {}",e))
	    			.and_then(|_| Ok(()));
				
				receiving
					.join(sending)
					.map(|(a,_)| a)
			});
	rt.spawn(pf);		
	(Arc::new(sender_tx), rcvr_rx)
}


fn main()
{
	let mut rt = Runtime::new().unwrap();

//	let addr = "127.0.0.1:8080";
	let addr = "/tmp/sock";

	let (s, r) = if !addr.starts_with("/") {
		open_tcp(&mut rt, addr)
	} else {
		open_uds(&mut rt, addr)
	};
	s.unbounded_send("Hello async".to_string()).unwrap();
	let receiving = r
		.for_each(move |line| {
   	   		println!("Received line {} {}", line, line.len());
			s.unbounded_send("Hello async".to_string()).unwrap();
	    	Ok(())
		})
   		.and_then(|_| Ok(()));
	
	rt.spawn(receiving);
	rt.shutdown_on_idle().wait().unwrap();
}

