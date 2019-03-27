
use std::io::BufReader;
use std::io::BufRead;
use std::fs::File;
use std::env;

extern crate ordered_float;

use ordered_float::*;

extern crate cairo;

use cairo::Context;
use cairo::ImageSurface;
use cairo::Format;
use cairo::LineCap;
use cairo::LineJoin;


#[derive(Debug)]
struct Data<T> {
	val: T,
	time: T
}

fn convert_t(t: u16) -> f64 {
	t as f64 * 1.17f64
}

const RS : f64 = 0.5;

fn convert_c(c: u16) -> f64 {
	1000f64 * (c as f64 - 2048f64) * 3.3f64 / 4096f64 / RS
}

fn convert_u(c: u16) -> f64 {
	1000f64 * (c as f64 - 2048f64) * 3.3f64 / 4096f64
}


fn load(fnm: &str, n: usize, u: bool) -> (Vec<Data<f64>>,f64,f64,f64)
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
	if u {
      	    data.push(Data{ val: convert_u(v), time: convert_t(t) });
	} else {
	    data.push(Data{ val: convert_c(v), time: convert_t(t) });
	}
    }

    let maxc = data.iter().map(|d| OrderedFloat(d.val)).max().unwrap().into_inner();
    let minc = data.iter().map(|d| OrderedFloat(d.val)).min().unwrap().into_inner();
    let maxt = data.last().unwrap().time;

    (data, maxt, minc, maxc)
}


fn draw_axis(cr: &Context, nx: i32, ny: i32)
{
	cr.set_source_rgb(0.0, 0.0, 0.0);
	cr.move_to(0.0, 0.5);	//x
	cr.line_to(1.0, 0.5);

	cr.move_to(0.1, 0.0);	//y
	cr.line_to(0.1, 1.0);
	
	let stepx = 0.9 / (nx as f64);
	for x in 0..nx {
		cr.move_to(0.1+(x as f64)*stepx, 0.5-0.01);
		cr.line_to(0.1+(x as f64)*stepx, 0.5+0.01);
	}
	
	let stepy = 0.5 / (ny as f64);
	for y in 1..2*ny {
		cr.move_to(0.1+0.01, (y as f64)*stepy);
		cr.line_to(0.1-0.01, (y as f64)*stepy);
	}
	
	// arrows
	cr.move_to(0.1, 0.0); 	cr.line_to(0.1+0.01, 0.02);
	cr.move_to(0.1, 0.0); 	cr.line_to(0.1-0.01, 0.02);

	cr.move_to(1.0, 0.5); 	cr.line_to(1.-0.02, 0.5-0.01);
	cr.move_to(1.0, 0.5); 	cr.line_to(1.-0.02, 0.5+0.01);
	cr.stroke();
	
}


fn draw_grid(cr: &Context, nx: i32, ny: i32)
{
	cr.set_dash(&[0.005,0.01], 0.);
    cr.set_line_cap(LineCap::Round);
    cr.set_line_join(LineJoin::Bevel);
	
	let stepx = 0.9 / (nx as f64);
	for x in 1..nx {
		cr.move_to(0.1+(x as f64)*stepx, 0.01);
		cr.line_to(0.1+(x as f64)*stepx, 0.99);
	}
	
	let stepy = 1.0 / (ny as f64);
	for y in 1..ny {
		cr.move_to(0.1+0.02, 0.5+(y as f64)*stepy/2.);
		cr.line_to(1.0-0.01, 0.5+(y as f64)*stepy/2.);
		cr.move_to(0.1+0.02, 0.5-(y as f64)*stepy/2.);
		cr.line_to(1.0-0.01, 0.5-(y as f64)*stepy/2.);
	}

	cr.stroke();
	cr.set_dash(&[1.0], 0.);
}


fn draw_metrics(cr: &Context, nx: i32, ny: i32,sx: f64,sy: f64, u:bool)
{
	cr.set_font_size(0.025);

	let stepx = 0.9 / (nx as f64);
	for x in 1..nx {
		cr.move_to(0.1+(x as f64)*stepx, 0.5+0.03);
		let val = sx * (x as f64);
		cr.show_text(&format!("{}",val));
	}
	
	let stepy = 1.0 / (ny as f64);
	for y in 1..ny {
		cr.move_to(0.03, 0.5-(y as f64)*stepy/2.);
		let val = sy * (y as f64);
		cr.show_text(&format!("{}",val));

		cr.move_to(0.03, 0.5+(y as f64)*stepy/2.);
		let val = - sy * (y as f64);
		cr.show_text(&format!("{}",val));
	}
	
	cr.move_to(0.03, 0.03);
	if u {
	    cr.show_text("mV");
	} else {
	    cr.show_text("mA");
	}
	cr.move_to(1.0-0.03, 0.5+0.03);
	cr.show_text("uS");
}


fn main()
{   
	let mut series = Vec::new();
	let mut n = 0;
	let mut i = 1;
	let mut u = false;
	let mut fnm = String::new();
	while env::args().len() > i {
	    fnm = env::args().skip(i).next().expect("Missing input file");
	    if fnm == "-l" {
    		n = env::args().skip(i+1).next().expect("Missing length").parse::<usize>().unwrap();
		i += 2;
		continue;
	    }
	    if fnm == "-u" {
		u = true;
		i += 1;
		continue;
	    } 
	    break;	    
	}

 	let (data, mut maxt, mut minc, mut maxc) = load(&fnm, n, u);
 	series.push(data);
	i += 1;

	while env::args().len() > i {
		let fnm2 = env::args().skip(i).next().expect("Missing input file");
 		let (data2, maxt2, minc2, maxc2) = load(&fnm2, n, u);
 		if minc2 < minc {
 			minc = minc2;
 		}
 		if maxc2 > maxc {
 			maxc = maxc2;
 		}
 		if maxt2 > maxt {
 			maxt = maxt2;
 		}
 		series.push(data2);
 		i += 1;
	}


	let w = 800;
	let h = 800;
	let surface = ImageSurface::create(Format::ARgb32, w, h).expect("Can't create surface");
    let cr = Context::new(&surface);
    
    cr.scale(w.into(), h.into());

    cr.set_line_width(0.001);
    cr.set_source_rgb(1.0, 1.0, 1.0);
	cr.rectangle(0.0, 0.0, 1.0, 1.0);
	cr.fill();
  	
  	let mut sx = 10.;
 	let mut nx = 1 + ((maxt/sx).floor() as i32);  	
	while nx > 14 {
		sx *= 2.;
		nx = 1 + ((maxt/sx).floor() as i32);
	}
	
  	let mut sy = 10.;
  	let mut maxy = maxc;
  	if maxy < f64::abs(minc) {
  		maxy = f64::abs(minc);
  	}
  	let mut ny = 1 + ((maxy/sy).floor() as i32);
	while ny > 14 {
		sy *= 2.;
		ny = 1 + ((maxy/sy).floor() as i32);
	}

  	draw_axis(&cr,nx,ny);
  	draw_grid(&cr,nx,ny);
  	draw_metrics(&cr,nx,ny,sx,sy,u);
  	
  	let kx = 0.9 / ((nx as f64) * sx);
  	let ky = 1.0 / ((2.*ny as f64) * sy); 

	let mut count = 0;
	for d in series {
		if count == 0 {
	    	cr.set_source_rgb(1.0, 0.0, 0.0);
	    } else if count == 1 {
	    	cr.set_source_rgb(0.0, 1.0, 0.0);
	    } else {
	    	cr.set_source_rgb(0.0, 0.0, 1.0);
	    }
		for i in 0..d.len()-1 {
  			cr.move_to(0.1+kx*d[i].time,   0.5-ky*d[i].val);
  			cr.line_to(0.1+kx*d[i+1].time, 0.5-ky*d[i+1].val);
  		}
  		count += 1;
	    cr.stroke();
    }
    
	let ofn = fnm+&".png";
    let mut file = File::create(&ofn).expect("Couldn't create output file");
    match surface.write_to_png(&mut file) {
        Ok(_) => println!("{} created",&ofn),
        Err(_) => println!("Error creating {}",&ofn),
	}
}

    