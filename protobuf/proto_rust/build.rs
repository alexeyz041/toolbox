/*
extern crate protoc_rust;

use protoc_rust::Customize;

fn main() {
    protoc_rust::run(protoc_rust::Args {
        out_dir: "src/protos",
        input: &["src/protos/example.proto"],
        includes: &["src/protos"],
        customize: Customize {
            ..Default::default()
        },
    })
    .expect("protoc");
}
*/

extern crate protoc_rust;
use protoc_rust::Codegen;

fn main() {
    Codegen::new()
        .protoc_path("protoc")
        .out_dir("src/protos")
        .inputs(&["src/protos/example.proto"])
        .includes(&["src/protos"])
        .run()
        .unwrap();
}
