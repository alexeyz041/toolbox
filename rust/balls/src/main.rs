
extern crate termion;

use termion::event::Key;
use termion::input::TermRead;
use termion::raw::IntoRawMode;
use termion::screen::*;
use termion::terminal_size;

use std::sync::{Arc, Mutex};
use std::io::{Write, stdout, stdin};
use std::thread;
use std::time::Duration;
use std::sync::atomic::{AtomicBool, Ordering};

extern crate rand;
use rand::Rng;


fn putcxy<W: Write>(screen: &mut W, x: i16, y: i16, c: char)
{
    write!(screen, "{}{}",
           termion::cursor::Goto(x as u16, y as u16),
           c)
           .unwrap();
}

struct Ball {
	x: i16,
	y: i16,
	dx: i16,
	dy: i16
}

impl Ball {

	fn new() -> Ball {
		let size = terminal_size().unwrap();
    	let px = rand::thread_rng().gen_range(2,size.0-1);
    	let py = rand::thread_rng().gen_range(2,size.1-1);
    	let bx = rand::thread_rng().gen::<bool>();
    	let by = rand::thread_rng().gen::<bool>();
		Ball {x:px as i16, y:py as i16, dx: if bx { 1 } else { -1 }, dy: if by { 1 } else { -1 } }
	}

	fn step<W: Write>(&mut self,screen: &mut W) {
		let size = terminal_size().unwrap();
        putcxy(screen,self.x,self.y,' ');
        if self.x+self.dx < size.0 as i16 && self.x+self.dx > 0 {
        	self.x = self.x + self.dx;
        } else {
        	self.dx = -self.dx;
        }
        if self.y+self.dy < size.1 as i16 && self.y+self.dy > 0 {
        	self.y = self.y + self.dy;
        } else {
        	self.dy = -self.dy;
        }
        putcxy(screen,self.x,self.y,'O');
	}
}


const N_THREADS : i16 = 20;


fn main() {
	//let done = Arc::new(Mutex::new(false));
	let done = Arc::new(AtomicBool::new(false));
	
    let stdin = stdin();
    let screen = Arc::new(Mutex::new(AlternateScreen::from(stdout().into_raw_mode().unwrap())));
    {
		let screen = screen.clone();
		let mut screen = screen.lock().unwrap();
	    write!(screen, "{}{}",
    		termion::cursor::Hide,
    		termion::clear::All)
	    	.unwrap();
	   screen.flush().unwrap();
	}

	let mut handle = vec![];
	for _ in 1..N_THREADS {
		let done = done.clone();
		let screen = screen.clone();
		handle.push( thread::spawn(move || {
			let mut ball = Ball::new();
			loop {
				{
					let mut screen = screen.lock().unwrap();
					ball.step(&mut *screen);
					screen.flush().unwrap();
				}

				thread::sleep(Duration::from_millis(100));

				{
					if done.load(Ordering::SeqCst) {
						break;
					}
				}
			}
		}));
	}

    for c in stdin.keys() {
        match c.unwrap() {
            Key::Char('q') => break,
            _ => {}
        }
    }
    
//	thread::sleep(Duration::from_millis(10000));

  	{
  		let done = done.clone();
	    done.store(true, Ordering::SeqCst);
    }
    for i in handle {
	    i.join().unwrap();
    }
    {
		let screen = screen.clone();
		let mut screen = screen.lock().unwrap();
    	write!(screen, "{}", ToMainScreen).unwrap();
    	write!(screen, "{}", termion::cursor::Show).unwrap();
    }

}

