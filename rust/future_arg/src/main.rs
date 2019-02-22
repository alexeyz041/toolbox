
extern crate tokio;
extern crate futures;

use futures::future::*;

use tokio::runtime::TaskExecutor;
use tokio::runtime::Runtime;
use futures::future::ok;
use std::sync::Arc;
use std::sync::Mutex;


fn run(ex: TaskExecutor, f: Box<Fn(u16) -> Box<Future<Item=(), Error=()>+Sync+Send>+Sync+Send>, arg: u16) {
	ex.spawn(f(arg));
}

fn main() {
	let rt = Runtime::new().unwrap();
	let v = Arc::new(Mutex::new(0u16));
	run(rt.executor(),
		Box::new({ let v = v.clone(); move |x| {
			{
				let mut v = v.lock().unwrap();
				*v = x;
			}
			println!("arg {}",x);
			Box::new(ok(()))
		}}),
	14);
	{
		let v = v.lock().unwrap();
		println!("val {}",v);
	}
	rt.shutdown_on_idle().wait().unwrap();
}
