//https://nitschinger.at/Binding-Threads-And-Processes-to-CPUs-in-Rust/
/*
extern crate hwloc;
use hwloc::Topology;

/// Example on how to check for specific topology support of a feature.
fn main() {
    let topo = Topology::new();

    // Check if Process Binding for CPUs is supported
    println!("CPU Binding (current process) supported: {}", topo.support().cpu().set_current_process());
    println!("CPU Binding (any process) supported: {}", topo.support().cpu().set_process());

    // Check if Thread Binding for CPUs is supported
    println!("CPU Binding (current thread) supported: {}", topo.support().cpu().set_current_thread());
    println!("CPU Binding (any thread) supported: {}", topo.support().cpu().set_thread());

    // Debug Print all the Support Flags
    println!("All Flags:\n{:?}", topo.support());
}
*/

extern crate hwloc;
extern crate libc;

use hwloc::{Topology, ObjectType, CPUBIND_THREAD, CpuSet};
use std::thread;
use std::sync::{Arc,Mutex};
use std::env;


fn sieve(n: usize) 
{ 
    let mut prime = vec![true; n+1]; 
    let mut p = 2;
    loop {
        if prime[p] { 
            for i in (p*p..n+1).step_by(p) {
                prime[i] = false;
	    }
        } 
	p = p + 1;
	if p*p > n {
	    break;
	}
    } 
  
    let mut cnt = 0;
    for p in 2..n+1 {
	if prime[p] {
	    cnt = cnt + 1;
//    	    print!("{} ",p); 
	}
    }
    println!("found {} primes",cnt);

} 


fn main() {
    let n = env::args().nth(1).expect("give number of cpu").parse::<usize>().unwrap();
    let topo = Arc::new(Mutex::new(Topology::new()));

    let num_cores = {
        let topo_rc = topo.clone();
        let topo_locked = topo_rc.lock().unwrap();
        (*topo_locked).objects_with_type(&ObjectType::Core).unwrap().len()
    };
    println!("Found {} cores.", num_cores);

    let handles: Vec<_> = (0..num_cores).map(|i| {
            let child_topo = topo.clone();
            thread::spawn(move || {
		{
                    let tid = unsafe { libc::pthread_self() };
	            let mut locked_topo = child_topo.lock().unwrap();
    	            let before = locked_topo.get_cpubind_for_thread(tid, CPUBIND_THREAD);
        	    let bind_to = cpuset_for_core(&*locked_topo, i % n);
            	    locked_topo.set_cpubind_for_thread(tid, bind_to, CPUBIND_THREAD).unwrap();
                    let after = locked_topo.get_cpubind_for_thread(tid, CPUBIND_THREAD);
	            println!("Thread {}: Before {:?}, After {:?}", i, before, after);
		}
		sieve(10000000);
            })
        }).collect();

        for h in handles {
            h.join().unwrap();
        }
}

fn cpuset_for_core(topology: &Topology, idx: usize) -> CpuSet {
    let cores = (*topology).objects_with_type(&ObjectType::Core).unwrap();
    match cores.get(idx) {
        Some(val) => val.cpuset().unwrap(),
        None => panic!("No Core found with id {}", idx)
    }
}

