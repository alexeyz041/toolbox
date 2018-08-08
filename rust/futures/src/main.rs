use std::io::{Error, ErrorKind};

extern crate futures;
use futures::prelude::*;
use futures::future::ok;
use futures::future::err;

extern crate tokio_core;
use tokio_core::reactor::Core;


fn my_fn() -> Result<u32, Box<Error>> {
    Ok(100)
}

fn my_fut() -> impl Future<Item = u32, Error = Box<Error>> {
    ok(100)
    //err(Box::new(Error::new(ErrorKind::Other,"bad luck")))
}


fn my_fn_squared(i: u32) -> Result<u32, Box<Error>> {
    Ok(i * i)
}

fn my_fut_squared(i: u32) -> impl Future<Item = u32, Error = Box<Error>> {
    ok(i * i)
}


fn main()
{
    let mut reactor = Core::new().unwrap();
    let retval = reactor.run(my_fut()).unwrap();
    println!("{:?} {:?}", my_fn().unwrap(),retval);

    let chained_future = my_fut().and_then(|retval| my_fut_squared(retval));
    let retval2 = reactor.run(chained_future).unwrap();
    println!("{:?} {:?}", my_fn().and_then(|r|my_fn_squared(r)).unwrap(),retval2);
}
