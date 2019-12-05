
use std::io::Write;
use std::os::unix::net::UnixStream;
use std::thread;
use std::time::Duration;
use std::io::Read;
use std::path::Path;



fn main()
{
    let mut stream = match UnixStream::connect(&Path::new("/tmp/sock")) {
        Err(_) => panic!("can't connect to server"),
        Ok(stream) => stream,
    };

    for _ in 0..10 {
        // Send message
	match stream.write(b"Hello") {
    	    Err(_) => panic!("couldn't send message"),
            Ok(_) => {}
	}

	let mut buf = [0u8; 32];
	stream.read(&mut buf).unwrap();

	println!("reply {}", String::from_utf8(buf.to_vec()).unwrap());

	thread::sleep(Duration::from_secs(1));
    }
}
