use std::fmt::Write;


fn dups(ch: char, n : usize) -> String
{
    let mut buf = String::new();
    for _ in 0..n {
	let _ = write!(buf,"{}",ch);
    }
    buf
}

fn print_framed(s : &str)
{
    let rows = s.split_whitespace().collect::<Vec<&str>>();
    let m = rows.iter().map(|r|r.len()).max().unwrap();
    println!("{}",dups('*',m+4));
    for r in &rows {
	println!("* {}{} *",r,dups(' ',m-r.len()));
    }
    println!("{}",dups('*',m+4));
}


fn main()
{
    let s = "Hello Rust World in a frame";
    print_framed(&s);
}
