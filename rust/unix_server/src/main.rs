
use std::io::Write;
use std::os::unix::net::UnixListener;
use std::thread;
use std::io::Read;
use std::fs;

fn main()
{
    let _ = fs::remove_file("/tmp/sock");
    let listener = UnixListener::bind("/tmp/sock").unwrap();
    println!("listening started, ready to accept");
    for stream in listener.incoming() {
        thread::spawn(|| {
	    let mut stream = stream.unwrap();
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
