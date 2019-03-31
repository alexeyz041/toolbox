
extern crate futures;
extern crate tokio;
use futures::future::*;
use std::sync::Arc;
use tokio::runtime::Runtime;
use futures::future::ok;
extern crate lazycell;
use lazycell::AtomicLazyCell;


type MadType = Arc<Fn(u32)->Box<Future<Item=(),Error=()>+Send+Sync>+Send+Sync>;

fn main()
{
	let mut rt = Runtime::new().unwrap();
	let ex = rt.executor();
	let lc = Arc::new(AtomicLazyCell::<MadType>::new());

	let fu : MadType = Arc::new({
		let ex = ex.clone();
		let lc = lc.clone();
		move |x| {
			Box::new(
				ok(x)
					.and_then({
						let ex = ex.clone();
						let lc = lc.clone();
						move |x|{
							println!("{}",x);
							let f = lc.borrow().unwrap().clone();
							ex.spawn(f(x+1));
							Ok(())
						}
					})
			)
		}
	});
	lc.fill(fu.clone());
	
	rt.spawn(fu(1));
	rt.shutdown_on_idle().wait().unwrap();
}

