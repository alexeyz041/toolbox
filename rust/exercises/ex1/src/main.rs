
#![feature(test)]
extern crate test;
use List::*;
use std::fmt::Write;

enum List {
    Cons(u32, Box<List>),
    Nil,
}


impl List {
    fn new() -> List {
        Nil
    }

	// Add new element to the list
    fn prepend(self, elem: u32) -> List {
        Cons(elem, Box::new(self))
    }

    // Return the length of the list
    fn len(&self) -> Box<u32> {
        match *self {
            Cons(_, ref tail) => Box::new(1 + *tail.len()),
            Nil => Box::new(0)
        }
    }
    
    fn len_nr(&self) -> Box<u32> {
        let mut len = 0;
        let mut p = self;
        while let Cons(_, ref tail) = *p {
    		len += 1;
	    	p = tail;	
        }
		Box::new(len)
    }
    
    
	// convert list to string    
    fn stringify(&self) -> String {
        match *self {
            Cons(head, ref tail) => {
                format!("{}, {}", head, tail.stringify())
            },
            Nil => {
                format!("Nil")
            },
        }
    }
    
    fn stringify_nr(&self) -> String {
		let mut buf = String::new();
		let mut p = self;
		loop {
    		match *p {
        		Cons(head, ref tail) => {
            		write!(buf,"{}, ", head).unwrap();
            		p = tail;
        		},
        		Nil => {
            		write!(buf,"Nil").unwrap();
            		break;
        		},
	    	}
        }
        buf
    }
    
}


fn main() {
    let mut list = List::new();

    // Prepend some elements
    list = list.prepend(1);
    list = list.prepend(2);
    list = list.prepend(3);

    // Show the final state of the list
    println!("linked list has length: {}", list.len());
    println!("{}", list.stringify());
}

#[cfg(test)]
mod tests {
    use super::*;
    use test::Bencher;


    #[bench]
    fn bench_len(b: &mut Bencher) {
		let mut list = List::new();
    	for i in 1..10000 {
	    	list = list.prepend(i);
    	}
        b.iter(|| list.len());
    }

    #[bench]
    fn bench_len_nr(b: &mut Bencher) {
		let mut list = List::new();
    	for i in 1..10000 {
	    	list = list.prepend(i);
    	}
        b.iter(|| list.len_nr());
    }

    #[bench]
    fn bench_stringify(b: &mut Bencher) {
		let mut list = List::new();
    	for i in 1..10000 {
	    	list = list.prepend(i);
    	}
        b.iter(|| {
        	let _ = list.stringify();
        });
    }

	#[bench]
    fn bench_stringify_nr(b: &mut Bencher) {
		let mut list = List::new();
    	for i in 1..10000 {
	    	list = list.prepend(i);
    	}
        b.iter(|| {
        	let _ = list.stringify_nr();
        });
    }
}
 

