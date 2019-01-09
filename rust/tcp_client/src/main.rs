
use std::io::prelude::*;
use std::net::TcpStream;
use std::{thread, time};


fn main()
{
    let mut stream = TcpStream::connect("127.0.0.1:8080").unwrap();
    loop {
        stream.write(b"Hello").unwrap();
	let mut response = [0u8; 10];
        stream.read(&mut response).unwrap();
	println!("{:?}", response);
	thread::sleep(time::Duration::from_millis(1000));
    }
}
