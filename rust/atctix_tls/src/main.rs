
use actix_web::{ middleware, web, App, HttpRequest, HttpResponse, HttpServer };

use rustls::internal::pemfile::{certs, rsa_private_keys};
use rustls::{NoClientAuth, ServerConfig};
use std::fs::File;
use std::io::BufReader;

#[macro_use] extern crate log;
extern crate env_logger;


fn get(req: HttpRequest, path: web::Path<(String,)>) -> HttpResponse {
    info!("{:?}", req);

    HttpResponse::Ok()
        .content_type("text/plain")
        .body(format!("Hello {}!", path.0))
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
            .service(web::resource("/get/{name}").route(web::get().to(get)))
    })
    .bind_rustls("127.0.0.1:8443", config)?
    .run()
}
