
extern crate num_traits;
use num_traits::Float;


fn min<T: Float>(value1: T, value2: T) -> T {
	if value1 <= value2 {
		value1
	} else {
		value2
	}
}



fn main()
{
    println!("Hello, generic world! {} {}",min::<f32>(1.1,2_f32), min::<f64>(1.1,2.2));
}
