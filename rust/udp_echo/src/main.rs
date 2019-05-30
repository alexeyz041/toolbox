extern crate tokio;
use tokio::net::UdpFramed;
use tokio::prelude::*;
use tokio::runtime::Builder;

extern crate futures;
use futures::sync::mpsc::unbounded;

extern crate tokio_udp;
use tokio_udp::UdpSocket;

use std::env;
use std::io;
use std::net::SocketAddr;
use std::sync::Arc;
use std::thread::sleep;
use std::time::Duration;

extern crate tokio_codec;
use tokio_codec::Decoder;
use tokio_codec::Encoder;

extern crate bytes;
use bytes::BufMut;
use bytes::BytesMut;

#[derive(Debug)]
struct Msg {
    buf: Vec<u8>,
}

struct MsgCodec {}

impl MsgCodec {
    fn new() -> MsgCodec {
        MsgCodec {}
    }
}

impl Decoder for MsgCodec {
    type Item = Msg;
    type Error = io::Error;

    fn decode(&mut self, buf: &mut BytesMut) -> Result<Option<Msg>, io::Error> {
        Ok(Some(Msg { buf: buf.to_vec() }))
    }
}

impl Encoder for MsgCodec {
    type Item = Msg;
    type Error = io::Error;

    fn encode(&mut self, msg: Msg, buf: &mut BytesMut) -> Result<(), io::Error> {
        buf.reserve(msg.buf.len());
        buf.put(msg.buf);
        Ok(())
    }
}

//extern crate tokio_threadpool;

fn main() {
    let mut rt = Builder::new().build().unwrap();

    let addr = env::args().nth(1).expect("Missing address");
    let addr = addr.parse::<SocketAddr>().expect("Can't parse address");

    let sock = UdpSocket::bind(&addr).expect("unable to bind TCP listener");
    let framed = UdpFramed::new(sock, MsgCodec::new());
    let (sink, stream) = framed.split();

    let (sender, rcvr) = unbounded::<(Msg,SocketAddr)>();
    let sender = Arc::new(sender);

    let server = stream
        .map_err(|err| println!("socket erro {}", err))
        .for_each(move |(msg, addr)| {
            println!("rcvd: mag len {} from {}", msg.buf.len(), addr);
	    sleep(Duration::from_millis(10));
            let _ = sender.unbounded_send((msg,addr));
            Ok(())
        });

    let sending = sink
        .sink_map_err(|err| println!("sink error {}", err))
        .send_all(rcvr.map(|(msg,addr)| {
            println!("sending msg len {} to {:?}", msg.buf.len(), addr);
            (msg, addr)
        })).then(|_| Ok(()));

    rt.spawn(server);
    rt.spawn(sending);
    rt.shutdown_on_idle().wait().unwrap();
}
