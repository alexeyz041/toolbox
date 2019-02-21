
extern crate crossbeam;

use crossbeam::atomic::AtomicCell;
use std::ptr::null_mut;
use std::default::Default;


#[derive(Debug,Clone)]
struct Key {
	data: Vec<u8>,
}

impl Key {
	fn new(n: u8) -> Self {
		Key{ data:  vec![n,1,2] }
	}
}

impl Default for Key {	
	fn default() -> Self {
		Self::new(0)
	}
}


struct Atomic<T> {
	ac: AtomicCell<*mut T>,
}

impl<T> Atomic<T>
	where T: ?Sized + Clone + Default
{
	fn new() -> Self {
		println!("is atomic {}",AtomicCell::<*mut T>::is_lock_free());
		Atomic {
			ac: AtomicCell::new(null_mut()),
		}
	}
	
	fn store(&self, k: &T) {
		let p : * mut T = Box::into_raw(Box::new(k.clone()));
		let old : * mut T = self.ac.swap(p);
		if !old.is_null() {
			unsafe {
				println!("drop 1");
 				drop(Box::from_raw(old));
			}
		}
	}
	
	fn get(&self) -> T {
		let p : * mut T = self.ac.load();
		if p.is_null() {
			T::default()
		} else {
			unsafe {
				(*p).clone()
			}
		}
	}
}

impl<T> Drop for Atomic<T> {
    fn drop(&mut self) {
		let p : * mut T = self.ac.swap(null_mut());
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
	let s = Atomic::<Key>::new();
	s.store(&k1);
	s.store(&k2);

	let k = s.get();
	println!("{:?}",k);

	let k = s.get();
	println!("{:?}",k);

	let k = s.get();
	println!("{:?}",k);
}
