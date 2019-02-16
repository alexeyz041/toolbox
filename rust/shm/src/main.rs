
extern crate memmap;

use std::env;
use std::fs::File;
use std::fs::OpenOptions;
//use memmap::Mmap;
use memmap::MmapMut;
use std::io::Write;

fn main() {
    let op = env::args()
        .nth(1)
	.expect("supply operation r,w or c");
    if op == "r" {
	read();
    } else if op == "w" {
	write();
    } else if op == "c" {
	create();
    } else {
	println!("?{}",op);
    }
}

fn read() {
//    let file = File::open("/dev/shm/malex.hello03.01").expect("failed to open the file");
//    let mmap = unsafe { Mmap::map(&file).expect("failed to map the file") };

    let file = OpenOptions::new()
            .read(true)
            .write(true)
            .open("/dev/shm/malex.hello03.01")
            .expect("failed to open the file");
    let mmap = unsafe { MmapMut::map_mut(&file).expect("failed to map the file") };
    for i in 0..10 {
    	println!("{:3} {}",mmap[i], mmap[i] as char);
    }
}

fn write() {
    let file = OpenOptions::new()
            .read(true)
            .write(true)
            .open("/dev/shm/malex.hello03.01")
            .expect("failed to open the file");
    let mut mmap = unsafe { MmapMut::map_mut(&file).expect("failed to map the file") };
    mmap[0..10].copy_from_slice(b"0123456789");
}

fn create() {
    let mut file = File::create("/dev/shm/malex.hello03.01").unwrap();
    file.write_all(b"Hello, world!").unwrap();
}



