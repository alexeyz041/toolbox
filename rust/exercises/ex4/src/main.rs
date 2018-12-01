
extern crate ordered_float;

use ordered_float::*;

fn main()
{
    let a = vec![1_f32, 2_f32, 3_f32];
    println!("max = {}",a.iter().map(|x| OrderedFloat(*x)).max().unwrap());
}
