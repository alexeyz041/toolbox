use std::time::Duration;
use async_std::task::{sleep, spawn};
use std::io;

async fn sleepus() -> u32 {
    for i in 1..=10 {
        println!("Sleepus {}", i);
        sleep(Duration::from_millis(500)).await;
    }
    0
}

fn sleepus1() -> impl std::future::Future<Output=Result<u32,io::Error>> {
    async {
        for i in 1..=10 {
            println!("Sleepus {}", i);
            sleep(Duration::from_millis(500)).await;
        }
	Ok(1)
    }
}

async fn interruptus() {
    for i in 1..=5 {
        println!("Interruptus {}", i);
        sleep(Duration::from_millis(1000)).await;
    }
}

#[async_std::main]
async fn main() {

    let sleepus = spawn(sleepus());
    let sleepus1 = spawn(sleepus1());
    let i = spawn(interruptus());
    i.await;
    let a = sleepus.await;
    let b = sleepus1.await;
    println!("a = {} b = {:?}", a, b);
}
