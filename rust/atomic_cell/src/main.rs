
extern crate crossbeam;

use crossbeam::atomic::AtomicCell;
use std::ptr::null_mut;

#[derive(Debug,Clone)]
struct Key {
	data: Vec<u8>,
}

impl Key {
	fn new(n: u8) -> Self {
		Key{ data:  vec![n,1,2] }
	}
}


struct KeyKeeper {
	key: AtomicCell<*mut Key>,
}

impl KeyKeeper {
	fn new() -> Self {
		println!("is atomic {}",AtomicCell::<*mut Key>::is_lock_free());
		KeyKeeper {
			key: AtomicCell::new(null_mut()),
		}
	}
	
	fn keep(&self, k: &Key) {
		let p : * mut Key = Box::into_raw(Box::new(k.clone()));
		let old : * mut Key = self.key.swap(p);
		if !old.is_null() {
			unsafe {
				println!("drop 1");
 				drop(Box::from_raw(old));
			}
		}
	}
	
	fn get(&self) -> Key {
		let p : * mut Key = self.key.load();
		if p.is_null() {
			Key::new(0)
		} else {
			unsafe {
				(*p).clone()
			}
		}
	}
}

impl Drop for KeyKeeper {
    fn drop(&mut self) {
		let p : * mut Key = self.key.swap(null_mut());
		if !p.is_null() {
			unsafe {
		        println!("Dropping!");
				drop(Box::from_raw(p));
			}
		}       
    }
}


fn main()
{
	let k1 = Key::new(1);
	let k2 = Key::new(2);
	let s = KeyKeeper::new();
	s.keep(&k1);
	s.keep(&k2);

	let k = s.get();
	println!("{:?}",k);

	let k = s.get();
	println!("{:?}",k);

	let k = s.get();
	println!("{:?}",k);
}
