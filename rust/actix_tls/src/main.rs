use std::fs::File;
use std::io::BufReader;
use bytes::BytesMut;

use futures::Future;
use futures::stream::Stream;
use futures::future::ok;

use actix_web::{ middleware, web, App, HttpRequest, HttpResponse, HttpServer, error, Error };
use rustls::internal::pemfile::{certs, rsa_private_keys};
use rustls::{NoClientAuth, ServerConfig};

#[macro_use] extern crate log;
extern crate env_logger;


fn get(req: HttpRequest, path: web::Path<(String,)>) -> impl Future<Item = HttpResponse, Error = Error> {
    info!("{:?}", req);

    ok(HttpResponse::Ok()
        .content_type("text/plain")
        .body(format!("Hello {}!", path.0)))
}


const MAX_SIZE: usize = 262_144; // max payload size is 256k

fn post(payload: web::Payload) -> impl Future<Item = HttpResponse, Error = Error> {
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
    	    info!("body = {}", String::from_utf8(body.to_vec()).unwrap());
    	    Ok(HttpResponse::Ok()
        	.content_type("text/plain")
		.body("Hello post!"))
        })
}


fn main() -> std::io::Result<()> {
    std::env::set_var("RUST_LOG", "actix_web=info,actix_server=info,example=debug");
    env_logger::init();

    // load ssl keys
    let mut config = ServerConfig::new(NoClientAuth::new());
    let cert_file = &mut BufReader::new(File::open("cert.pem").unwrap());
    let key_file = &mut BufReader::new(File::open("key.pem").unwrap());
    let cert_chain = certs(cert_file).unwrap();
    let mut keys = rsa_private_keys(key_file).unwrap();
    config.set_single_cert(cert_chain, keys.remove(0)).unwrap();

    HttpServer::new(|| {
        App::new()
            // enable logger
            .wrap(middleware::Logger::default())
            .service(web::resource("/get/{name}").route(web::get().to_async(get)))
	    .service(web::resource("/post").route(web::post().to_async(post)))
    })
    .bind_rustls("127.0.0.1:8443", config)?
    .run()
}
