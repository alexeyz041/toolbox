use std::thread::{sleep};
use std::thread;
use std::time::Duration;

fn sleepus() {
    for i in 1..=10 {
        println!("Sleepus {}", i);
        sleep(Duration::from_millis(500));
    }
}

fn interruptus() {
    for i in 1..=5 {
        println!("Interruptus {}", i);
        sleep(Duration::from_millis(1000));
    }
}

fn main() {
    let t1 = thread::spawn(move || sleepus());
    let t2 = thread::spawn(move || interruptus());
    t1.join().unwrap();
    t2.join().unwrap();
}
