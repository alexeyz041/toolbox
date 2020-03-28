use std::fs::File;
use std::io::BufReader;

use futures::StreamExt;
use actix_web::{middleware, web, App, HttpRequest, HttpResponse, HttpServer, Error};

#[macro_use] extern crate log;

use rustls::internal::pemfile::{certs, pkcs8_private_keys}; //rsa_private_keys
use rustls::{NoClientAuth, ServerConfig};


async fn get(req: HttpRequest, path: web::Path<(String,)>) -> Result<HttpResponse, Error> {
    info!("{:?}", req);

    Ok(HttpResponse::Ok()
        .content_type("text/plain")
        .body(format!("Hello {}!", path.0)))
}



async fn post(mut payload: web::Payload) -> Result<HttpResponse, Error> {

    let mut bytes = web::BytesMut::new();
    while let Some(item) = payload.next().await {
        bytes.extend_from_slice(&item?);
    }

    // body is loaded, now we can process request
    info!("body = {}", String::from_utf8(bytes.to_vec()).unwrap());
    Ok(HttpResponse::Ok()
        .content_type("text/plain")
        .body("Hello post!"))
}

#[actix_rt::main]
async fn main() -> std::io::Result<()> {
    std::env::set_var("RUST_LOG", "actix_server=info,actix_web=info");
    env_logger::init();

    // load ssl keys
    let mut config = ServerConfig::new(NoClientAuth::new());
    let cert_file = &mut BufReader::new(File::open("cert.pem").unwrap());
    let key_file = &mut BufReader::new(File::open("key1.pem").unwrap());
    let cert_chain = certs(cert_file).unwrap();
//    let mut keys = rsa_private_keys(key_file).unwrap();
    let mut keys = pkcs8_private_keys(key_file).unwrap();
    config.set_single_cert(cert_chain, keys.remove(0)).unwrap();

    HttpServer::new(|| {
        App::new()
            .wrap(middleware::DefaultHeaders::new().header("X-Version", "0.2"))
            .wrap(middleware::Compress::default())
            .wrap(middleware::Logger::default())
            .service(web::resource("/get/{name}").route(web::get().to(get)))
            .service(web::resource("/post").route(web::post().to(post)))
    })
    .bind_rustls("127.0.0.1:8443", config)? //.bind("127.0.0.1:8080")?
    .workers(1)
    .run()
    .await
}
