trait Foo {
    fn foo(&mut self);
}

struct Fooo {
    a: u16,
}

impl Foo for Fooo {
    fn foo(&mut self) {
	println!("Fooo {}",self.a);
	self.a += 1;
    }
}

struct MySt {
    f: Box<Foo+Send>,
}

impl MySt {
    fn new(f: Box<Foo+Send>) -> Self {
	Self{ f }
    }
    
    fn p(&mut self) {
	self.f.foo();
    }
}

fn main() {
    let mut myst = MySt::new(Box::new(Fooo{ a: 1 }));
    myst.p();
    
    std::thread::spawn(move || {
	myst.p();	
    })
    .join()
    .unwrap();
}
