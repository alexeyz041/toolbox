extern crate lib_test;
use lib_test::{Fern, run_simulation};

fn main()
{
    let mut fern = Fern {
	size: 1.0,
	growth_rate: 0.001
    };
    run_simulation(&mut fern, 1000);
    println!("final fern size: {}", fern.size);
}
