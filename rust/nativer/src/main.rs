
include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

fn main()
{
    println!("Hello, world!");
  
    let mut ctx = context_t { x:1, y:1 };
	unsafe {
	    hello(&mut ctx);
	}
	println!("Hello, world! {},{}",ctx.x,ctx.y);

}
