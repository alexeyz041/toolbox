
extern crate memmap;

use std::env;
use std::fs::File;
use std::fs::OpenOptions;
use memmap::Mmap;
use memmap::MmapMut;

fn main() {
    let op = env::args()
        .nth(1)
	.expect("supply operation r or w");
    if op == "r" {
	read();
    } else if op == "w" {
	write();
    } else {
	println!("?{}",op);
    }
}

fn read() {
    let file = File::open("/dev/shm/malex.hello03.01").expect("failed to open the file");
    let mmap = unsafe { Mmap::map(&file).expect("failed to map the file") };
    for i in 0..10 {
    	println!("{}",mmap[i]);
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


