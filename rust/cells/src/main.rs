
use std::thread;
use std::cell::RefCell;
use std::cell::UnsafeCell;
use std::marker::Sync;
use std::sync::Arc;

struct Mem {
    a: u16,
}


impl Mem {
    fn new() -> Self {
    	Self{ a: 0 }
    }

    fn wr(&mut self) {
	self.a = 1;
    }
}

//--------------------------------------

struct Mem2 {
    m: RefCell<Mem>,
}


impl Mem2 {
    fn new() -> Self {
    	Self{ m: RefCell::new(Mem::new()) }
    }

    fn wr(&self) {
	let mut mm = self.m.borrow_mut();
	mm.wr();
    }
}

unsafe impl Sync for Mem2 {}

//--------------------------------------


struct Mem3 {
    m: UnsafeCell<Mem2>,
}


impl Mem3 {
    fn new() -> Self {
    	Self{ m: UnsafeCell::new(Mem2::new()) }
    }

    fn wr(&self) {
	let m2 = self.m.get();
	unsafe {
    	    (*m2).wr();
	}
    }
}

unsafe impl Sync for Mem3 {}

//--------------------------------------

fn run(m: Arc<Mem3>) {
    thread::spawn(move || {
	m.wr();
    });
}

fn run2(m: Arc<Mem2>) {
    thread::spawn(move || {
	m.wr();
    });
}



fn main()
{
    let mut m1 = Mem::new();
    m1.wr();

    let m2 = Mem2::new();
    m2.wr();

    let m3 = Mem3::new();
    m3.wr();

    run(Arc::new(m3));

    run2(Arc::new(m2));
}
