
#[macro_use] extern crate log;
extern crate env_logger;


fn main() -> std::io::Result<()> {
    std::env::set_var("RUST_LOG", "request=debug");
    env_logger::init();

    let body = reqwest::get("http://127.0.0.1:8080/get/123").unwrap()
	.text().unwrap();

    println!("body = {:?}", body);

    let client = reqwest::Client::new();
    let mut res = client.post("http://127.0.0.1:8080/sign")
        .body("the exact body that is sent")
	.send().unwrap();

    println!("body = {:?} {:?}", res.text().unwrap(), res.status());

    Ok(())
}
