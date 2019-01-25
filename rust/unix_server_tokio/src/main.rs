
extern crate futures;
extern crate tokio;
extern crate tokio_timer;

use futures::Sink;
use tokio::runtime::Runtime;
use futures::Stream;
use tokio::net::unix::UnixListener;
use tokio::codec::Framed;
use tokio::codec::LinesCodec;
use futures::sync::mpsc::unbounded;
use futures::Future;
use std::fs;

use std::sync::atomic::{AtomicUsize, Ordering, ATOMIC_USIZE_INIT};

static GLOBAL_COUNT: AtomicUsize = ATOMIC_USIZE_INIT;


fn main()
{
	let _ = fs::remove_file("/tmp/sock");
	let mut rt = Runtime::new().unwrap();
	let ex = rt.executor();

	let addr = "/tmp/sock";
	let pf = UnixListener::bind(addr).unwrap()
		.incoming()
		.map_err(|e| println!("error 1 {}",e))
		.for_each({
			let ex = ex.clone();
			move |sock| {
		    let s = Framed::new(sock, LinesCodec::new());
		    let (wr,rd) = s.split();
			let (s,r) = unbounded::<String>();

			let sending = wr
	    			.sink_map_err(|e| println!("error 2 {}",e))
	    			.send_all(r)
	    			.and_then(|_| Ok(()));

		    let receiving = rd
		    		.for_each({
		    			let s = s.clone();
		    			move |line| {
							let count = GLOBAL_COUNT.fetch_add(1, Ordering::SeqCst);
			   	   			println!("{} Received line {} {}", count, line, line.len());
							s.unbounded_send(line).unwrap();
					    	Ok(())
					   	}
					})
					.map_err(|e| println!("error 3 {}",e))
	    			.and_then(|_| Ok(()));
				
			ex.spawn(receiving);
			ex.spawn(sending);
			Ok(())
		}});
	
	rt.spawn(pf);
	rt.shutdown_on_idle().wait().unwrap();
}

/*


extern crate lazycell;
use lazycell::LazyCell;

use std::sync::Arc;

struct MyClass {
 related: LazyCell<Option<Arc<MyClass>>>
}

fn main()
{
	let obj1 = Arc::new(MyClass { related: LazyCell::new() });
	let obj2 = Arc::new(MyClass { related: LazyCell::new() });
	obj1.related.fill(None);
	obj2.related.fill(None);

	obj1.related.fill(Some(Arc::clone(&obj2)));
	obj2.related.fill(Some(Arc::clone(&obj1)));
}
*/


