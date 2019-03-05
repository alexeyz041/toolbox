
const VERSION: &'static str = env!("CARGO_PKG_VERSION");
const DESC: &'static str = env!("CARGO_PKG_DESCRIPTION");


fn main()
{
    println!("version {}", VERSION);
    println!("description {}", DESC);
}
