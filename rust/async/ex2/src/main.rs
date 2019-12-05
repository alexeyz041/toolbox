use std::time::Duration;
use async_std::task::{sleep, spawn};


async fn sleepus() {
    for i in 1..=10 {
        println!("Sleepus {}", i);
        sleep(Duration::from_millis(500)).await;
    }
}

fn sleepus1() -> impl std::future::Future<Output=()> {
    async {
        for i in 1..=10 {
            println!("Sleepus {}", i);
            sleep(Duration::from_millis(500)).await;
        }
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
    sleepus.await;
    sleepus1.await;
}
