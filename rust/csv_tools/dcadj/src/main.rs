
use std::io::Write;
use std::io::BufReader;
use std::io::BufRead;
use std::fs::File;
use std::env;
use std::io::BufWriter;


#[derive(Debug)]
struct Data<T> {
	val: T,
	time: T
}


fn load(fnm: &str, n: usize) -> Vec<Data<u16>>
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
    	    let val = &w[1];
    	
	    let t = time.parse::<u16>().unwrap();
	    let v = val.parse::<u16>().unwrap();
      	    data.push(Data{ val: v, time: t });
	}
	data
}



fn scale_k(pos: usize, m: f64, mt: f64) -> f64 {
    if pos < 5 {
	return 0.;
    }
    if pos >= 5 && pos < mt as usize {
	return ((pos-5) as f64)*m/mt;
    }
    m
}


fn main()
{   
    let m = env::args().skip(1).next().expect("Missing adj. value").parse::<f64>().unwrap();
    let mt = env::args().skip(2).next().expect("Missing adj. time").parse::<f64>().unwrap();
    let fnm = env::args().skip(3).next().expect("Missing input file");

    let data = load(&fnm, 0);

    let mut res = Vec::new();
    for i in 0..data.len() {
    	res.push(Data { val: (data[i].val as f64 - scale_k(i,m,mt)) as u16, time: data[i].time })
    }
    
    {
	let ofn = "adj.txt";
	let file = File::create(&ofn).expect("Couldn't create output file");
	let mut writer = BufWriter::new(&file);
		
	for i in 0..res.len() {
	    writeln!(&mut writer,"{},{}", res[i].time, res[i].val).unwrap();
	}
	println!("created {}",ofn);
    }
}
