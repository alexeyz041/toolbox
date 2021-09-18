
use std::io::BufReader;
use std::io::BufRead;
use std::fs::File;
use std::env;

extern crate ordered_float;

use ordered_float::*;


#[derive(Debug)]
struct Data<T> {
	current: T,
	time: T
}

fn convert_t(t: u16) -> f64 {
	t as f64 * 1.17f64
}

const RS : f64 = 0.5;

fn convert_c(c: u16) -> f64 {
	1000f64 * (c as f64 - 2048f64) * 3.3f64 / 4096f64 / RS
}

fn load(fnm: &str, n: usize) -> Vec<Data<f64>>
{
    println!("Loading {}",fnm);
    let mut data = Vec::new();
    let f = File::open(fnm).unwrap();
    let file = BufReader::new(&f);
    for line in file.lines() {
    	if n != 0 && data.len() >= n {
    		break;
    	}
    	let l = line.unwrap();
    	let w = l.split(',').collect::<Vec<&str>>();
    	
    	let time = &w[0];
    	let current = &w[1];
    	
	let t = time.parse::<u16>().unwrap();
	let c = current.parse::<u16>().unwrap();
      	data.push(Data{ current: convert_c(c), time: convert_t(t) });
    }
    data
}

fn integ(data: &[Data<f64>], start_t: usize, end_t: usize) -> f64 {
	let mut ig = 0.0;
	let dt = data[1].time - data[0].time;
	for i in start_t..end_t+1 {
		let val0 = if i > 0 { data[i-1].current } else { data[0].current };
		let val1 = data[i].current;
		ig += (val1+val0)*dt/2.;
	}
	ig
}


fn main()
{   
    let fnm = env::args().skip(1).next().expect("Missing input file");
    let data = load(&fnm, 200);

    let (maxc, maxt) = data.iter().map(|d| (OrderedFloat(d.current), d.time))
		.max_by(|(x,_),(y,_)| x.cmp(y)).unwrap();
    let maxc = maxc.into_inner();
    let maxp = data.iter().position(|d| d.time == maxt).unwrap();
		
    let (minc, mint) = data.iter().map(|d| (OrderedFloat(d.current), d.time))
		.min_by(|(x,_),(y,_)| x.cmp(y)).unwrap();
    let minc = minc.into_inner();
    let minp = data.iter().position(|d| d.time == mint).unwrap();
	
    println!("max ct {:.1} ma {:.1} us {:.1} pt", maxc, maxt, maxp);
    println!("min ct {:.1} ma {:.1} us {:.1} pt", minc, mint, minp);

    let t0 = (maxp+minp)/2;
    let i1 = integ(&data, 0, t0);
    let i2 = integ(&data, t0, data.len()-1);
	
    println!("i1 = {:.1} uJ", i1/1000.);
    println!("i2 = {:.1} uJ", i2/1000.);
    println!("COP = {:.1}%", f64::abs(100.*i2/i1));
}

    