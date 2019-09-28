
#[macro_use] extern crate log;
extern crate env_logger;
use std::fs::File;
use std::io::Read;

fn main() -> std::io::Result<()> {
    std::env::set_var("RUST_LOG", "request=debug");
    env_logger::init();

    let mut buf = Vec::new();
//    File::open("ca.pem").unwrap().read_to_end(&mut buf).unwrap();
//    let cert = reqwest::Certificate::from_pem(&buf).unwrap();
    File::open("ca.der").unwrap().read_to_end(&mut buf).unwrap();
    let cert = reqwest::Certificate::from_der(&buf).unwrap();

    let client = reqwest::Client::builder()
		    .use_rustls_tls()
//		    .use_default_tls()
		    .add_root_certificate(cert)
		    .danger_accept_invalid_hostnames(true)
		    .danger_accept_invalid_certs(true)
		    .build().unwrap();

    let body = client.get("https://127.0.0.1:8443/get/123").send().unwrap()
	.text().unwrap();

    info!("body = {:?}", body);

    Ok(())
}
