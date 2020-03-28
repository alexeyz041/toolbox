
#[macro_use] extern crate log;
extern crate env_logger;
use std::fs::File;
use std::io::Read;

#[tokio::main]
async fn main() {
    std::env::set_var("RUST_LOG", "request=debug");
    env_logger::init();

    // sed -i -e 's/TRUSTED //g' ca.pem
    let mut buf = Vec::new();
    File::open("ca.pem").unwrap().read_to_end(&mut buf).unwrap();
    let cert = reqwest::Certificate::from_pem(&buf).unwrap();

//    File::open("ca.der").unwrap().read_to_end(&mut buf).unwrap();
//    let cert = reqwest::Certificate::from_der(&buf).unwrap();

    let client = reqwest::Client::builder()
		    .use_native_tls() //use_rustls_tls()
		    .add_root_certificate(cert) // .danger_accept_invalid_certs(true)
		    .build().unwrap();

    let body = client.get("https://localhost:8443/get/123").send().await.unwrap()
	.text().await.unwrap();
    info!("get body = {:?}", body);

    let body = client.post("https://localhost:8443/post").body("Hello from reqwest").send().await.unwrap()
	.text().await.unwrap();
    info!("post body = {:?}", body);
}
