
extern crate tokio;
extern crate futures;

use futures::future::*;
use tokio::prelude::*;
use tokio::runtime::TaskExecutor;
use tokio::runtime::Runtime;
use futures::sync::mpsc::unbounded;
use futures::sync::oneshot;
use futures::sync::oneshot::Sender;
use futures::future::ok;
use std::sync::Arc;
use futures::sync::mpsc::UnboundedSender;


enum NoMutexOp {
	Set,
	Get
}

struct NoMutex<T> {
	sender: Arc<UnboundedSender<(NoMutexOp,Option<T>,Option<Sender<T>>)>>,
}

impl<T> NoMutex<T>
	where T: Send + Sync + std::fmt::Debug + std::default::Default + Clone + 'static
{
	fn new(ex: TaskExecutor) -> Self {
		let (sender, rcvr) = unbounded::<(NoMutexOp,Option<T>,Option<Sender<T>>)>();

		let handling = rcvr
			.fold(T::default(), |mut d, (op,val,tx)| {
				match op {
					NoMutexOp::Set => {
						if let Some(val) = val {
							d = val;
						}
						if let Some(tx) = tx {
							tx.send(d.clone()).unwrap();
						}							
					},
					
					NoMutexOp::Get => {
						if let Some(tx) = tx {
							tx.send(d.clone()).unwrap();
						}					
					},
				}
				ok(d)
			})
			.and_then(|_| Ok(()));
		ex.spawn(handling);
		
		Self { sender: Arc::new(sender) }
	}
	
	fn set(&self, val: T) {
		self.sender.unbounded_send((NoMutexOp::Set, Some(val), None)).unwrap();
	}

	fn set2(&self, val: T) -> Box<Future<Item=(), Error=()>+Send+Sync> {
		let (tx, rx) = oneshot::channel::<T>();
		self.sender.unbounded_send((NoMutexOp::Set, Some(val), Some(tx))).unwrap();
		Box::new(
			rx.and_then(|_| {
	        	Ok(())
	    	})
    		.map_err(|e| println!("error {}",e))
    	)
	}

	fn get(&self) -> Box<Future<Item=T, Error=()>+Send+Sync> {
		let (tx, rx) = oneshot::channel::<T>();
		self.sender.unbounded_send((NoMutexOp::Get, None, Some(tx))).unwrap();
		Box::new(
			rx.and_then(|val| {
	        	Ok(val)
	    	})
    		.map_err(|e| println!("error {}",e))
    	)
	}
}



fn main()
{
	let mut rt = Runtime::new().unwrap();
	
	let nm0 = Arc::new(NoMutex::<i16>::new(rt.executor()));
	
	let lf = loop_fn((nm0,0), |(nm, i)| {
				nm
				.set2(i)
				.and_then({
					let nm = nm.clone();
					move |_| {
						nm
						.get()
				}})
				.and_then(move |val| {
					println!("val {}",val);
					Ok(Loop::Continue( (nm,i+1) ))
				})
	});
	rt.spawn(lf);
	
	rt.shutdown_on_idle().wait().unwrap();
}
