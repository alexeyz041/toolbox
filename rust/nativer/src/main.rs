
include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

use std::ffi::CStr;
use std::os::raw::c_char;

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


#[no_mangle]
pub extern "C" fn print(value: *const c_char)
{
    let value = unsafe { CStr::from_ptr(value).to_string_lossy().into_owned() };
    println!("rust: {}",value);
}