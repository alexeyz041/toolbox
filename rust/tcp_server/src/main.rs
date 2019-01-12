
use std::io::Write;
use std::net::TcpListener;
use std::thread;
use std::io::Read;


fn main()
{
    let listener = TcpListener::bind("127.0.0.1:8080").unwrap();
    println!("listening started, ready to accept");
    for stream in listener.incoming() {
        thread::spawn(|| {
	    let mut stream = stream.unwrap();
	    println!("connection from {}",stream.peer_addr().unwrap());
	    loop {
        	let mut buf = [0u8; 32];
		let rcvd = stream.read(&mut buf).unwrap();
        	println!("Received {} bytes", rcvd);
		if rcvd > 0 {
		    stream.write(&buf[..rcvd]).unwrap();
		}
	    }
        });
    }
}
