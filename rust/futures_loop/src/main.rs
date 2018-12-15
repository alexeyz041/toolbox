
extern crate futures;
extern crate tokio;
extern crate tokio_timer;

use std::time::Duration;
use tokio_timer::sleep;
use tokio::runtime::Runtime;
use futures::future::*;


fn main()
{

	let lf = lazy(|| {
		println!("lazy future");
		Ok(())
	});
	
	let pf = loop_fn(0u16,|i| {
		println!("loop future {}",i);
		
	    let sleep = sleep(Duration::from_millis(3000));
		sleep
			.map_err(|e| panic!("timer error {}",e))
			.map(move |_| i)
			.and_then(|i| {
			if i < 10 {
    	   	    Ok(Loop::Continue(i+1))
        	} else {
   	        	Ok(Loop::Break(14))
	       	}
	    })
	})
	.and_then(|i| {
		println!("then {}",i);
		Ok(())
	});

//	tokio::run(pf);
	

	let mut rt = Runtime::new().unwrap();
	rt.spawn(pf);
	rt.spawn(lf);
	rt.shutdown_on_idle().wait().unwrap();
}
