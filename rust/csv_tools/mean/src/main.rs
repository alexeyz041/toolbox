
use std::io::Write;
use std::io::BufReader;
use std::io::BufRead;
use std::fs::File;
use std::env;
use std::io::BufWriter;
use std::fs;
use std::path::Path;


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


fn list_files(dir: &str) -> Vec<String> {
    let dir = Path::new(dir);
    let mut list = Vec::new();
    for entry in fs::read_dir(dir).unwrap() {
        let entry = entry.unwrap();
        let path = entry.path();
        if !path.is_dir() {
	    if let Some(ext) = path.extension() {
		if ext == "txt" {
            	    list.push(path.to_str().unwrap().to_string());
		}
	    }
        }
    }
    list
}


fn main()
{   
	let mut series = Vec::new();
	let mut n = 0;
	let mut i = 1;
	let mut fnm = String::new();
	while env::args().len() > i {
	    fnm = env::args().skip(i).next().expect("Missing input file");
    	    if fnm == "-l" {
		n = env::args().skip(i+1).next().expect("Missing length").parse::<usize>().unwrap();
		i += 2;
	    }
	    break;
	}

	if fnm.len() == 0 {
	    let list = list_files(".");
	    for fnm in &list {
         	    let data = load(&fnm, n);
    		series.push(data);
	    }
	} else {
     	    let data = load(&fnm, n);
 	    series.push(data);
	    i += 1;

	    while env::args().len() > i {
		let fnm2 = env::args().skip(i).next().expect("Missing input file");
 		let data2 = load(&fnm2, n);
 		series.push(data2);
 		i += 1;
	    }
	}

	let mut res = Vec::new();
    for i in 0..series[0].len() {
    	let mut sum : f64 = 0.0;
    	for j in 0..series.len() {
    		sum += series[j][i].val as f64;
    	}
    	res.push(Data { val: (sum/(series.len() as f64)) as u16, time: series[0][i].time })
    }
    
    {
		let ofn = "mean.txt";
		let file = File::create(&ofn).expect("Couldn't create output file");
		let mut writer = BufWriter::new(&file);
		
		for i in 0..res.len() {
		    writeln!(&mut writer,"{},{}",res[i].time,res[i].val).unwrap();
		}
    }
}
