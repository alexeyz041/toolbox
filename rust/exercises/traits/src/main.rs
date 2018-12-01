#[derive(Debug)]
struct Sheep {
	naked: bool,
	name: String
}

struct Duck {
	name: String
}

trait Animal {
    // Static method signature; `Self` refers to the implementor type.
    fn new(name: String) -> Self;

    // Instance method signatures; these will return a string.
    fn name(&self) -> String;
    fn noise(&self) -> String;

    // Traits can provide default method definitions.
    fn talk(&self) {
        println!("{} says {}", self.name(), self.noise());
    }
}


impl Sheep {
    fn is_naked(&self) -> bool {
        self.naked
    }

    fn shear(&mut self) {
        if self.is_naked() {
            // Implementor methods can use the implementor's trait methods.
            println!("{} is already naked...", self.name());
        } else {
            println!("{} gets a haircut!", self.name);

            self.naked = true;
        }
    }
}


// Implement the `Animal` trait for `Sheep`.
impl Animal for Sheep {
    // `Self` is the implementor type: `Sheep`.
    fn new(name: String) -> Sheep {
        Sheep { name: name, naked: false }
    }

    fn name(&self) -> String {
        self.name.clone()
    }

    fn noise(&self) -> String {
        if self.is_naked() {
            "baaaaah?".to_string()
        } else {
            "baaaaah!".to_string()
        }
    }
    
    // Default trait methods can be overridden.
    fn talk(&self) {
        // For example, we can add some quiet contemplation.
        println!("{} pauses briefly... {}", self.name, self.noise());
    }
}


impl Animal for Duck {
    fn new(name: String) -> Duck {
    	Duck { name: name }
    }


    fn name(&self) -> String {
	    self.name.clone()
    }
    
    fn noise(&self) -> String {
    	"kra kra".to_string()
    }

}


fn main()
{
    let mut dolly: Sheep = Animal::new("Dolly".to_string());
    let duck: Duck = Animal::new("Duck".to_string());

    dolly.talk();
    dolly.shear();
    dolly.talk();

	duck.talk();

	println!("{:?}",dolly);
}

