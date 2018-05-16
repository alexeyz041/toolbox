use std::thread;
use std::time::Duration;
use std::sync::{Mutex, Arc};

extern crate rand;
use rand::Rng;

    


struct Philosopher {
    name: String,
    left: usize,
    right: usize
}

impl Philosopher {
    fn new(name: &str, left: usize, right: usize) -> Philosopher {
	Philosopher {
	    name: name.to_string(),
	    left: left,
	    right: right
	}
    }

    fn eat(&self, table: &Table) {
	println!("{} going to eat.", self.name);
        let _left = table.forks[self.left].lock().unwrap();
        let _right = table.forks[self.right].lock().unwrap();
	println!("{} is eating.", self.name);
	let rng_number = rand::thread_rng().gen_range(100,1000);
        thread::sleep(Duration::from_millis(rng_number));
        println!("{} is done eating.", self.name);
    }
}

struct Table {
    forks: Vec<Mutex<()>>,
}



fn main()
{
    let table = Arc::new(Table { forks: vec![
        Mutex::new(()),
        Mutex::new(()),
        Mutex::new(()),
        Mutex::new(()),
        Mutex::new(()),
    ]});

    let philosophers = vec![
	Philosopher::new("Baruch Spinoza",0,1),
	Philosopher::new("Gilles Deleuze",1,2),
	Philosopher::new("Karl Marx",2,3),
	Philosopher::new("Friedrich Nietzsche",3,4),
	Philosopher::new("Michel Foucault",4,0),
    ];

    let handles: Vec<_> = philosophers.into_iter().map(|p| {
	let table = table.clone();
        thread::spawn(move || {
            p.eat(&table);
        })
    }).collect();

    for h in handles {
        h.join().unwrap();
    }
}
