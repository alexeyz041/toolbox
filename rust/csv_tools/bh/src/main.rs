
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

#[derive(Debug,Clone,Copy)]
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



fn load(fnm: &str, n: usize, u: bool) -> Vec<Data<f64>>
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
		let v = if u {
			convert_u(val.parse::<u16>().unwrap())
		} else {
			convert_c(val.parse::<u16>().unwrap())
		};
      	data.push(Data{ val: v, time: convert_t(t) });
    }
	data
}

fn draw_axis(cr: &Context, nx: i32, ny: i32)
{
    cr.set_source_rgb(0.0, 0.0, 0.0);
    cr.move_to(0.0, 0.9);	//x
    cr.line_to(1.0, 0.9);

    cr.move_to(0.1, 0.0);	//y
    cr.line_to(0.1, 1.0);
	
    let stepx = 0.9 / (nx as f64);
    for x in 0..nx {
	cr.move_to(0.1+(x as f64)*stepx, 0.9-0.01);
	cr.line_to(0.1+(x as f64)*stepx, 0.9+0.01);
    }
	
    let stepy = 0.9 / (ny as f64);
    for y in 1..ny {
	cr.move_to(0.1+0.01, 0.9-(y as f64)*stepy);
	cr.line_to(0.1-0.01, 0.9-(y as f64)*stepy);
    }
	
    // arrows
    cr.move_to(0.1, 0.0); 	cr.line_to(0.1+0.01, 0.02);
    cr.move_to(0.1, 0.0); 	cr.line_to(0.1-0.01, 0.02);

    cr.move_to(1.0, 0.9); 	cr.line_to(1.-0.02, 0.9-0.01);
    cr.move_to(1.0, 0.9); 	cr.line_to(1.-0.02, 0.9+0.01);
    cr.stroke().expect("stroke failed");
}


fn draw_grid(cr: &Context, nx: i32, ny: i32)
{
    cr.set_dash(&[0.005,0.01], 0.);
    cr.set_line_cap(LineCap::Round);
    cr.set_line_join(LineJoin::Bevel);
	
    let stepx = 0.9 / (nx as f64);
    for x in 1..nx {
	cr.move_to(0.1+(x as f64)*stepx, 0.01);
	cr.line_to(0.1+(x as f64)*stepx, 0.89);
    }
	
    let stepy = 0.9 / (ny as f64);
    for y in 1..ny {
	cr.move_to(0.1+0.02, 0.9-(y as f64)*stepy);
	cr.line_to(1.0-0.01, 0.9-(y as f64)*stepy);
    }

    cr.stroke().expect("stroke failed");
    cr.set_dash(&[1.0], 0.);
}

fn draw_metrics(cr: &Context, nx: i32, ny: i32,sx: f64,sy: f64) -> Result<(), cairo::Error>
{
    cr.set_font_size(0.025);

    let stepx = 0.9 / (nx as f64);
    for x in 1..nx {
	cr.move_to(0.1+(x as f64)*stepx, 0.9+0.03);
	let val = sx * (x as f64);
	cr.show_text(&format!("{}",val))?;
    }
	
    let stepy = 0.9 / (ny as f64);
    for y in 1..ny {
	cr.move_to(0.03, 0.9-(y as f64)*stepy);
	let val = sy * (y as f64);
	cr.show_text(&format!("{}",val))?;
    }
	
    cr.move_to(0.03, 0.03);
    cr.show_text("mV")?;
    cr.move_to(1.0-0.04, 0.9+0.03);
    cr.show_text("mA")
}


struct Serie {
    i_data: Vec<Data<f64>>,
    minc: f64,
    maxc: f64,
    u_data: Vec<Data<f64>>,
    minu: f64,
    maxu: f64,
    maxp: usize,
    minp: usize,
}

fn main() {
    let s = env::args().skip(1).next().expect("Missing start offset").parse::<usize>().unwrap();
    let n = env::args().skip(2).next().expect("Missing length").parse::<usize>().unwrap();
    let mut i = 3;
    let mut series = Vec::new();
    while i < env::args().count() {
	let fnm = env::args().skip(i).next().expect("Missing input file");
	let fnm2 = env::args().skip(i+1).next().expect("Missing input file");
	i += 2;
 	let i_data = load(&fnm, n, false);
 	let u_data = load(&fnm2, n, true);

	let (maxc, maxt) = i_data.iter().map(|d| (OrderedFloat(d.val), d.time))
		.max_by(|(x,_),(y,_)| x.cmp(y)).unwrap();
	let maxc = maxc.into_inner();
	let maxp = i_data.iter().position(|d| d.time == maxt).unwrap();
		
	let (minc, mint) = i_data.iter().map(|d| (OrderedFloat(d.val), d.time))
		.min_by(|(x,_),(y,_)| x.cmp(y)).unwrap();
	let minc = minc.into_inner();
	let minp = i_data.iter().position(|d| d.time == mint).unwrap();

	println!("max ct {} {} {}",maxc,maxt,maxp);
	println!("min ct {} {} {}",minc,mint,minp);
		
	let maxu = u_data.iter().map(|d| OrderedFloat(d.val)).max().unwrap().into_inner();
	let minu = u_data.iter().map(|d| OrderedFloat(d.val)).min().unwrap().into_inner();

	println!("max u {}",maxu);
	println!("min u {}",minu);

	series.push( Serie { i_data, u_data, maxc, minc, maxu, minu, maxp, minp } );
    }

    let w = 800;
    let h = 800;
    let surface = ImageSurface::create(Format::ARgb32, w, h).expect("Can't create surface");
    let cr = Context::new(&surface).expect("new context");
    
    cr.scale(w.into(), h.into());

    cr.set_line_width(0.001);
    cr.set_source_rgb(1.0, 1.0, 1.0);
    cr.rectangle(0.0, 0.0, 1.0, 1.0);
    cr.fill().expect("fill failed");
  	
    let mut sx = 10.;
    let mut maxx = series.iter().map(|s| OrderedFloat(s.maxc)).max().unwrap().into_inner();
    let maxx2 = series.iter().map(|s| OrderedFloat(f64::abs(s.minc))).max().unwrap().into_inner();
    if maxx < maxx2 {
	maxx = maxx2;
    }
    let mut nx = 1 + ((maxx/sx).floor() as i32);  	
    while nx > 14 {
	sx *= 2.;
	nx = 1 + ((maxx/sx).floor() as i32);
    }
	
    let mut sy = 10.;
    let mut maxy = series.iter().map(|s| OrderedFloat(s.maxu)).max().unwrap().into_inner();
    let maxy2 = series.iter().map(|s| OrderedFloat(f64::abs(s.minu))).max().unwrap().into_inner();
    if maxy < maxy2 {
  	maxy = maxy2;
    }
    let mut ny = 1 + ((maxy/sy).floor() as i32);
    while ny > 14 {
	sy *= 2.;
	ny = 1 + ((maxy/sy).floor() as i32);
    }

    println!("nx {} ny {}",nx,ny);

    draw_axis(&cr,nx,ny);
    draw_grid(&cr,nx,ny);
    draw_metrics(&cr,nx,ny,sx,sy).expect("draw metrics");
  	
    let kx = 0.9 / ((nx as f64) * sx);
    let ky = 0.9 / ((ny as f64) * sy); 
    let mut count = 0;
    for se in &series {
        match count {
	    0 => cr.set_source_rgb(1.0, 0.0, 0.0),
   	    1 => cr.set_source_rgb(0.0, 0.0, 1.0),
   	    _ => cr.set_source_rgb(0.0, 1.0, 0.0),
	}

        for i in s..se.minp {
	    cr.move_to(0.1-kx*se.i_data[i].val,   0.9+ky*se.u_data[i].val);
	    cr.line_to(0.1-kx*se.i_data[i+1].val, 0.9+ky*se.u_data[i+1].val);
        }
        cr.stroke().expect("stroke failed");

	if series.len() == 1 {
	    cr.set_source_rgb(0.0, 0.0, 1.0);
	}
	for i in se.maxp..n-1 {
		cr.move_to(0.1+kx*se.i_data[i].val,   0.9+ky*se.u_data[i].val);
		cr.line_to(0.1+kx*se.i_data[i+1].val, 0.9+ky*se.u_data[i+1].val);
	}
        cr.stroke().expect("stroke failed");
	count += 1;
    }
    let ofn = "bh.png";
    let mut file = File::create(&ofn).expect("Couldn't create output file");
    match surface.write_to_png(&mut file) {
        Ok(_) => println!("{} created",&ofn),
        Err(_) => println!("Error creating {}",&ofn),
    }
}

    