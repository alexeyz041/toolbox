mod protos;
use protobuf::{parse_from_bytes, Message};
use protos::example::SimpleMessage;

fn main() {
    // Encode example request
    let mut out_msg = SimpleMessage::new();
    out_msg.set_lucky_number(41);
    let out_bytes: Vec<u8> = out_msg.write_to_bytes().unwrap();

    println!("{:02x?}", out_bytes);

    // Decode example request
    let in_msg = parse_from_bytes::<SimpleMessage>(&out_bytes).unwrap();
    let n = in_msg.get_lucky_number();
    assert_eq!(n, 41);
}
