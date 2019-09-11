
use actix_web::{ error, middleware, web, App, Error, HttpRequest, HttpResponse, HttpServer };
use bytes::BytesMut;
use futures::{Future, Stream};

#[macro_use] extern crate log;
extern crate env_logger;

use std::fs::File;
use std::io::Read;
use std::io::Write;
use std::process::Command;

const MAX_SIZE: usize = 262_144; // max payload size is 256k


fn sign(payload: web::Payload) -> impl Future<Item = HttpResponse, Error = Error> {
    // payload is a stream of Bytes objects
    payload
        .from_err()
        .fold(BytesMut::new(), move |mut body, chunk| {
            // limit max size of in-memory payload
            if (body.len() + chunk.len()) > MAX_SIZE {
                Err(error::ErrorBadRequest("overflow"))
            } else {
                body.extend_from_slice(&chunk);
                Ok(body)
            }
        })
        .and_then(|body| {
            // body is loaded, now we can process request
	    save_file("/tmp/csr.pem", &body)?;
	    cmd("./sign.sh")?;
            Ok(HttpResponse::Ok()
                .content_type("text/plain")
		.body(read_file("/tmp/cert.pem")?))
        })
}


fn get(req: HttpRequest, path: web::Path<(String,)>) -> HttpResponse {
    println!("{:?}", req);

    HttpResponse::Ok()
        .content_type("text/plain")
        .body(format!("Hello {}!", path.0))
}


fn read_file(fnm: &str) -> Result<Vec<u8>, Error>
{
    let mut file = File::open(fnm)?;
    let mut contents: Vec<u8> = Vec::new();
    file.read_to_end(&mut contents)?;
    Ok(contents)
}



fn save_file(fnm: &str, sig: &[u8]) -> Result<(), Error>
{
    let mut file = File::create(fnm)?;
    file.write_all(sig)?;
    Ok(())
}


fn cmd(c: &str) -> Result<(), Error>
{
    debug!("cmd {}", c);
    Command::new(c)
        .spawn()
        .expect("command failed to start");
    Ok(())
}


fn main() -> std::io::Result<()> {
    std::env::set_var("RUST_LOG", "actix_web=info,actix_server=info,example=debug");
    env_logger::init();

    HttpServer::new(|| {
        App::new()
            // enable logger
            .wrap(middleware::Logger::default())
            .service(web::resource("/sign").route(web::post().to_async(sign)))
            .service(web::resource("/get/{name}").route(web::get().to(get)))
    })
    .bind("127.0.0.1:8080")?
    .run()
}
