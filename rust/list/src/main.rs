use std::fmt::Write;
use List::*;

enum List {
    // Cons: Tuple struct that wraps an element and a pointer to the next node
    Cons(u32, Box<List>),
    // Nil: A node that signifies the end of the linked list
    Nil,
}

struct ListIter<'a> {
    p: &'a List,
}

// Methods can be attached to an enum
impl List {
    // Create an empty list
    fn new() -> List {
        // `Nil` has type `List`
        Nil
    }

    // Consume a list, and return the same list with a new element at its front
    fn prepend(self, elem: u32) -> List {
        // `Cons` also has type List
        Cons(elem, Box::new(self))
    }

    fn rem(self) -> List {
        match self {
            Cons(_, tail) => *tail,
            Nil => Nil,
        }
    }
    /*
    fn get(&self) -> u32
    {
	match *self {
            Cons(ele, _) => ele,
            Nil => 0,
        }
    }
*/

    // Return the length of the list
    fn len(&self) -> u32 {
        match *self {
            Cons(_, ref tail) => 1 + tail.len(),
            Nil => 0,
        }
    }

    // Return the length of the list, non recursive version
    fn len_nr(&self) -> u32 {
        let mut len = 0;
        let mut p = self;
        loop {
            match *p {
                Cons(_, ref tail) => {
                    len += 1;
                    p = tail;
                }
                Nil => {
                    return len;
                }
            }
        }
    }

    fn len_nr2(&self) -> u32 {
        let mut len = 0;
        let mut p = self;
        while let Cons(_, ref tail) = *p {
            len += 1;
            p = tail;
        }
        len
    }

    // Return representation of the list as a (heap allocated) string
    fn stringify_nr(&self) -> String {
        let mut buf = String::new();
        let mut p = self;
        loop {
            match *p {
                Cons(head, ref tail) => {
                    write!(buf, "{}, ", head).unwrap();
                    p = tail;
                }
                Nil => {
                    write!(buf, "Nil").unwrap();
                    break;
                }
            }
        }
        buf
    }

    fn stringify(&self) -> String {
        match *self {
            Cons(head, ref tail) => format!("{}, {}", head, tail.stringify()),
            Nil => "Nil".to_string(),
        }
    }

    fn iter(&self) -> ListIter {
        ListIter { p: &self }
    }
}

impl<'a> Iterator for ListIter<'a> {
    type Item = u32;

    fn next(&mut self) -> Option<u32> {
        match *self.p {
            Cons(ele, ref tail) => {
                self.p = tail;
                Some(ele)
            }
            Nil => None,
        }
    }
}

impl<'a> IntoIterator for &'a List {
    type Item = u32;
    type IntoIter = ListIter<'a>;

    fn into_iter(self) -> ListIter<'a> {
        //ListIter { p: self }
        self.iter()
    }
}

fn main() {
    // Create an empty linked list
    let mut list = List::new();

    // Prepend some elements
    list = list.prepend(1);
    list = list.prepend(2);
    list = list.prepend(3);

    // Show the final state of the list
    println!(
        "linked list has length: {}, non reccursive {}",
        list.len(),
        list.len_nr()
    );
    println!("{}", list.stringify());

    for i in &list {
        println!("{}", i);
    }

    // remove some elements
    list = list.rem();
    list = list.rem();
    println!("{}", list.stringify_nr());
    println!(
        "linked list has length: {}, non reccursive {}",
        list.len(),
        list.len_nr2()
    );
}
