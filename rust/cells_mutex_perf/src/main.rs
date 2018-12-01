#[macro_use]
extern crate criterion;
use criterion::Criterion;

use std::thread;
use std::cell::RefCell;
use std::cell::UnsafeCell;
use std::marker::Sync;
use std::sync::Arc;
use std::sync::Mutex;

extern crate rand;
use rand::prelude::*;


struct Mem {
    a: [u8; 256],
}


impl Mem {
    fn new() -> Self {
    	Self{ a: [0u8; 256] }
    }

    fn wr(&mut self,d: &[u8]) {
	self.a.copy_from_slice(d);
	let mut rng = rand::thread_rng();
	for _ in 1..100 {
	    let m: f64 = rng.gen_range(2.0, 100.0);
	    m.ln();
	}
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

    fn wr(&self,d: &[u8]) {
	let mut mm = self.m.borrow_mut();
	mm.wr(d);
    }
}

unsafe impl Sync for Mem2 {}

//--------------------------------------

//fn criterion_benchmark(c: &mut Criterion) {
//    let m2 = Mem2::new();
//    let d = [1u8; 256];
//    c.bench_function("mem", |b| b.iter(||
//	m2.wr(&d)
//    ));
//}

fn criterion_benchmark(c: &mut Criterion) {
    let m2 = Arc::new(Mutex::new(Mem2::new()));
//    let m2 = Mem2::new();
    let d = [2u8; 256];
    c.bench_function("mem5", |b| b.iter(|| {
	let m2 = m2.lock().unwrap();
	m2.wr(&d);
    }));
}


criterion_group!(benches, criterion_benchmark);
criterion_main!(benches);
