extern crate futures;
extern crate tokio;
extern crate tokio_timer;

use futures::Sink;
use std::net::SocketAddr;
use tokio::runtime::Runtime;
use futures::future::*;
use futures::Stream;
use tokio::net::TcpStream;
use tokio::codec::Framed;
use tokio::codec::LinesCodec;
use futures::sync::mpsc::unbounded;


fn main()
{
    let mut rt = Runtime::new().unwrap();

    let lf = loop_fn((),|_| {
	let addr = "127.0.0.1:8080".parse::<SocketAddr>().unwrap();
	let pf = TcpStream::connect(&addr)
	    .map_err(|e| println!("error 1 {}",e))
	    .inspect(|sock| println!("successfully connected to {}", sock.local_addr().unwrap()))
	    .and_then(move |sock| {
	        let s = Framed::new(sock, LinesCodec::new());
	        let (wr,rd) = s.split();
	        let (sender,rcvr) = unbounded::<String>();
	        
	        let sending = wr
			.sink_map_err(|e| println!("error 2 {}",e))
			.send_all(rcvr)
			.and_then(|_| Ok(()));
		
		sender.unbounded_send("Hello async".to_string()).unwrap();
	        let receiving = rd
		    .for_each(move |line| {
    		    println!("Received line {} {}", line, line.len());
			sender.unbounded_send("Hello async".to_string()).unwrap();
			Ok(())
		    })
		    .map_err(|e| println!("error 3 {}",e))
    		.and_then(|_| Ok(()));
		
		receiving
		    .join(sending)
		    .map(|(a,_)| a)
	    });
	    
	pf
	.and_then(|_| {
	    Ok(Loop::Continue(()))
        })
	.or_else(|_| {
	    Ok(Loop::Continue(()))
	})
    });

    rt.spawn(lf);
    rt.shutdown_on_idle().wait().unwrap();
}
