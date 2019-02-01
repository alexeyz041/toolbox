
use std::ops::Drop;
use std::sync::Arc;
use std::sync::Mutex;
use std::sync::Weak;

extern crate lazycell;
use lazycell::LazyCell;


struct HasDrop {
    me: LazyCell<Weak<Mutex<HasDrop>>>,
}

impl HasDrop {
    fn new() -> Arc<Mutex<Self>> {
	let s = Arc::new(Mutex::new(Self { me: LazyCell::new() }));
	{
	    let this = s.lock().unwrap();
	    let _ = this.me.fill(Arc::downgrade(&s));
	}
	s
    }

    fn get_me(&self) -> Arc<Mutex<Self>> {
	if let Some(me) = self.me.borrow() {
	    if let Some(me) = me.upgrade() {
		me
	    } else {
		panic!("1")
	    }
	} else {
	    panic!("2")
	}
    }
    
    fn do_someting(&self) {
	println!("do_someting");
    }
}



impl Drop for HasDrop {
    fn drop(&mut self) {
        println!("Dropping!");
    }
}

fn main() {
    let x = HasDrop::new();
    
    let me = {
	let this = x.lock().unwrap();
	this.get_me()
    };
    
    std::thread::spawn(move || {
        let this = me.lock().unwrap();
        this.do_someting();
    }).join().unwrap();
}
