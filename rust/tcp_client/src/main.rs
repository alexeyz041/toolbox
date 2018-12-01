
use std::io::prelude::*;
use std::net::TcpStream;

fn main()
{
    let mut stream = TcpStream::connect("127.0.0.1:8080").unwrap();
 //   stream.write_all(&command_bytes).unwrap();
    let mut response = String::new();
    stream.read_to_string(&mut response).unwrap();
    println!("{}", response);
}
