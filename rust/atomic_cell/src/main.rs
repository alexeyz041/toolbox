
use std::sync::atomic::{AtomicPtr, Ordering};
use std::ptr::null_mut;

extern crate futures;
extern crate tokio;

use futures::future::*;
use std::sync::Arc;
use tokio::runtime::Runtime;
use futures::future::ok;


#[derive(Debug,Clone)]
struct Key {
	data: Vec<u8>,
}

impl Key {
	fn new(n: u8) -> Self {
		Key{ data:  vec![n,1,2] }
	}
}



struct Atomic<T> {
	ac: AtomicPtr<T>,
}

impl<T> Atomic<T>
	where T: ?Sized + Clone + Send + Sync
{
	fn new() -> Self {
		Atomic {
			ac: AtomicPtr::new(null_mut()),
		}
	}
	
	fn store(&self, k: &T) {
		let p : * mut T = Box::into_raw(Box::new(k.clone()));
		let old : * mut T = self.ac.swap(p, Ordering::SeqCst);
		if !old.is_null() {
			unsafe {
 				drop(Box::from_raw(old));
			}
		}
	}
	
	fn get(&self) -> Option<T> {
		let p : * mut T = self.ac.load(Ordering::SeqCst);
		if p.is_null() {
			None
		} else {
			unsafe {
				Some((*p).clone())
			}
		}
	}
}

impl<T> Drop for Atomic<T> {
    fn drop(&mut self) {
		let p : * mut T = self.ac.swap(null_mut(), Ordering::SeqCst);
		if !p.is_null() {
			unsafe {
				drop(Box::from_raw(p));
			}
		}       
    }
}

unsafe impl<T> Send for Atomic<T> {}
unsafe impl<T> Sync for Atomic<T> {}

fn main()
{
	let k1 = Key::new(1);
	let s = Arc::new(Atomic::<Key>::new());
	s.store(&k1);
	let k = s.get();
	println!("{:?}",k);

	let k2 = Key::new(2);
	s.store(&k2);
	let k = s.get();
	println!("{:?}",k);
	
	let mut rt = Runtime::new().unwrap();
	let lf = ok(())
				.and_then({ let s = s.clone(); move |_| {
					let k3 = Key::new(3);
					s.store(&k3);
					Ok(())
				}});
	rt.spawn(lf);

	loop {	
		let k = s.get();
		println!("{:?}",k);
		if let Some(k) = k {
			if k.data[0] == 3 {
				break;
			}
		}
	}
	
	rt.shutdown_on_idle().wait().unwrap();
}
