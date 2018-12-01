
use std::io::Write;
use std::net::TcpListener;
use std::thread;


fn main()
{
    let listener = TcpListener::bind("127.0.0.1:8080").unwrap();
    println!("listening started, ready to accept");
    for stream in listener.incoming() {
        thread::spawn(|| {
            let mut stream = stream.unwrap();
            stream.write(b"Hello World\r\n").unwrap();
            println!("connection from {}",stream.peer_addr().unwrap());
        });
    }
}
