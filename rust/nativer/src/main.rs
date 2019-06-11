
include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

fn main()
{
    println!("Hello, world!");

    let mut buf = vec![0u8; 1000];

    let mut ctx = context_t { x:1, y:1, len: buf.len() as u32, p: buf.as_mut_ptr() };
    unsafe {
       hello(&mut ctx);
    }
    println!("Hello, world! {},{}",ctx.x,ctx.y);

    println!("len = {} buf = {:?}", ctx.len, buf);
}
