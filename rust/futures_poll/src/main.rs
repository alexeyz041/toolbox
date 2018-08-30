//
// Example from https://dev.to/mindflavor/rust-futures-an-uneducated-short-and-hopefully-not-boring-tutorial---part-3---the-reactor-433
//

extern crate futures;
use futures::Future;
use futures::Async;
use futures::Poll;

extern crate chrono;
use chrono::prelude::*;
use chrono::Duration;

extern crate tokio_core;
use tokio_core::reactor::Core;


use std::error::Error;

#[derive(Debug)]
struct WaitForIt {
    message: String,
    until: DateTime<Utc>,
    polls: u64,
}


impl WaitForIt {
    pub fn new(message: String, delay: Duration) -> WaitForIt {
        WaitForIt {
            polls: 0,
            message: message,
            until: Utc::now() + delay,
        }
    }
}

impl Future for WaitForIt
{
    type Item = String;
    type Error = Box<Error>;

    fn poll(&mut self) -> Poll<Self::Item, Self::Error> {
        let now = Utc::now();
        if self.until < now {
            Ok(Async::Ready(
                format!("{} after {} polls!", self.message, self.polls),
            ))
        } else {
            self.polls += 1;
            println!("not ready yet --> {:?}", self);
	    futures::task::current().notify();
            Ok(Async::NotReady)
        }
    }
}


fn main()
{
    let mut reactor = Core::new().unwrap();

    let wfi_1 = WaitForIt::new("I'm done:".to_owned(), Duration::seconds(1));
    println!("wfi_1 == {:?}", wfi_1);

    let ret = reactor.run(wfi_1).unwrap();
    println!("ret == {:?}", ret);
}
