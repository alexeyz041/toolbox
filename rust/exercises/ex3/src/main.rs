

use std::fmt::Write;
use Op::*;

enum Op {
	Plus,
	Minus,
	Link
}

fn gen(o: &[Op]) -> String
{
	let mut val = "0".to_string();
	let mut i  = 1;
	while i <= 9 {
		match o[i] {
			Plus => write!(val," + {}",i),
			Minus => write!(val," - {}",i),
			Link => write!(val,"{}",i),
		}.unwrap();
		i += 1;
	}
	val
}


fn eval(s: &String) -> Option<i64>
{
	let v = s.split_whitespace().collect::<Vec<&str>>();
	let mut sum = str::parse::<i64>(v[0]).unwrap();
	if v.len() == 1 {
		return Some(sum);
	}
	let mut i = 1;
	while i <= v.len()-2 {
		let o = v[i]; 
		let a2 = str::parse::<i64>(v[i+1]).unwrap(); 
		match o {
			"+" => sum += a2,
			"-" => sum -= a2,
			_ => return None
		}
		i+=2;
	}
	Some(sum)
}

fn calc(o: &[Op]) ->i64
{
	let s = gen(o);
    eval(&s).unwrap()
}


fn solve(mut op: &mut [Op], val: i64, pos: usize) -> bool
{
	if pos == 10 {
		if calc(&op) == val {
			println!("{} = {}",gen(&op),val);
			return true;
		}
		return false;
	}
	
	op[pos] = Plus;
	if solve(&mut op,val,pos+1) {
		return true;
	}
	
	op[pos] = Minus;
	if solve(&mut op,val,pos+1) {
		return true;
	}
	
	op[pos] = Link;
	if solve(&mut op,val,pos+1) {
		return true;
	}
	
	return false;
}


fn main()
{
	for i in 0..1000 {
		let mut op = vec![ Plus, Plus, Plus, Plus, Plus, Plus, Plus, Plus, Plus, Plus];
		if !solve(&mut op,i,0) {
			println!("no solution for {}",i);
			break;
		}
	}
}


