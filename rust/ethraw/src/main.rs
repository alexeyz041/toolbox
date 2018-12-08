// Copyright (c) 2014, 2015 Robert Clipsham <robert@octarineparrot.com>
//
// Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
// http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
// <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
// option. This file may not be copied, modified, or distributed
// except according to those terms.

/// This example shows a basic packet logger using libpnet
//
// extended example how to handle ARP, ICMP and UDP messages
//

#[macro_use]
extern crate lazy_static;
extern crate pnet;
#[macro_use]
extern crate serde_derive;
extern crate serde_yaml;
extern crate serde;

extern crate tun_tap;
use tun_tap::{Iface, Mode};

use pnet::packet::MutablePacket;
//use pnet::datalink::{self, NetworkInterface};
//use pnet::datalink::Channel::Ethernet;

use pnet::packet::arp::{ArpPacket, MutableArpPacket, ArpHardwareTypes, ArpOperation, ArpOperations};
use pnet::packet::ethernet::{EtherTypes, EthernetPacket, MutableEthernetPacket};
use pnet::packet::icmp::{echo_reply, echo_request, IcmpPacket, IcmpTypes, IcmpType};
use pnet::packet::icmpv6::Icmpv6Packet;
use pnet::packet::ip::{IpNextHeaderProtocol, IpNextHeaderProtocols};
use pnet::packet::ipv4::{Ipv4Packet,MutableIpv4Packet,Ipv4Flags};
use pnet::packet::ipv6::Ipv6Packet;
use pnet::packet::tcp::TcpPacket;
use pnet::packet::udp::{UdpPacket,MutableUdpPacket};
use pnet::packet::Packet;
use pnet::util::MacAddr;
use pnet::packet::ipv4;
use pnet::packet::udp::ipv4_checksum;
use pnet::packet::icmp::echo_reply::{MutableEchoReplyPacket,IcmpCodes};
use pnet::packet::icmp;

//use std::env;
//use std::io::{self, Write};
use std::fs::File;
use std::net::IpAddr;
use std::net::Ipv4Addr;
//use std::process;
use std::io::Read;
use std::process::Command;

const ETH_HEADER_LEN: usize = 14;
const IPV4_HEADER_LEN: usize = 20;
const UDP_HEADER_LEN: usize = 8;
const ICMP_HEADER_LEN: usize = 8;
const ARP_HEADER_LEN: usize = 28;

#[derive(Serialize, Deserialize)]
#[serde(remote = "MacAddr")]
struct MacAddrDef(u8, u8, u8, u8, u8, u8);

#[derive(Debug, PartialEq, Serialize, Deserialize)]
struct Config {
	my_ip: Ipv4Addr,
	#[serde(with = "MacAddrDef")]
	my_mac: MacAddr,
}

impl Config {
	fn load(file: &str) -> Self {
		let mut f = File::open(file).expect("config file not found");
	    let mut s = String::new();
    	f.read_to_string(&mut s).expect("something went wrong reading config file");
		let cfg: Config = serde_yaml::from_str(&s).unwrap();
		cfg
	}
}


lazy_static! {
    static ref config : Config = Config::load("config.yaml");
}


fn send_arp_packet(source_ip: Ipv4Addr,source_mac: MacAddr, target_ip: Ipv4Addr, target_mac: MacAddr,
    arp_operation: ArpOperation) -> Vec<u8> {
    
    let mut ethernet_buffer = [0u8; ETH_HEADER_LEN + ARP_HEADER_LEN];
    let mut ethernet_packet = MutableEthernetPacket::new(&mut ethernet_buffer).unwrap();

    ethernet_packet.set_ethertype(EtherTypes::Arp);
    ethernet_packet.set_source(source_mac);
    ethernet_packet.set_destination(target_mac);

    let mut arp_buffer = [0u8; ARP_HEADER_LEN];
    let mut arp_packet = MutableArpPacket::new(&mut arp_buffer).unwrap();

    arp_packet.set_hardware_type(ArpHardwareTypes::Ethernet);
    arp_packet.set_protocol_type(EtherTypes::Ipv4);
    arp_packet.set_hw_addr_len(6);
    arp_packet.set_proto_addr_len(4);
    arp_packet.set_operation(arp_operation);

    arp_packet.set_sender_hw_addr(source_mac);
    arp_packet.set_sender_proto_addr(source_ip);
    arp_packet.set_target_hw_addr(target_mac);
    arp_packet.set_target_proto_addr(target_ip);

    ethernet_packet.set_payload(arp_packet.packet_mut());
    ethernet_packet.packet().to_vec()
}


fn send_icmp_packet(source_ip: Ipv4Addr,source_mac: MacAddr, target_ip: Ipv4Addr, target_mac: MacAddr,
    icmp_type: IcmpType, seq: u16, id: u16, payload: &[u8], id2: u16) -> Vec<u8> {

    let mut ethernet_buffer = vec![0u8; ETH_HEADER_LEN+IPV4_HEADER_LEN+ICMP_HEADER_LEN+payload.len()];
    let mut ethernet_packet = MutableEthernetPacket::new(&mut ethernet_buffer).unwrap();
    ethernet_packet.set_ethertype(EtherTypes::Ipv4);
    ethernet_packet.set_source(source_mac);
    ethernet_packet.set_destination(target_mac);

    let mut ip_buffer = vec![0u8; IPV4_HEADER_LEN+ICMP_HEADER_LEN+payload.len()];
	let mut ip_packet = MutableIpv4Packet::new(&mut ip_buffer).unwrap();
    let total_len = (IPV4_HEADER_LEN + ICMP_HEADER_LEN + payload.len()) as u16;
    ip_packet.set_version(4);
    ip_packet.set_header_length(5);
    ip_packet.set_total_length(total_len);
    ip_packet.set_ttl(64);
    ip_packet.set_flags(Ipv4Flags::DontFragment);
    ip_packet.set_identification(id2);
    ip_packet.set_source(source_ip);
    ip_packet.set_destination(target_ip);
    ip_packet.set_next_level_protocol(IpNextHeaderProtocols::Icmp);
    let checksum = ipv4::checksum(&ip_packet.to_immutable());
    ip_packet.set_checksum(checksum);

    let mut icmp_buffer = vec![0u8; ICMP_HEADER_LEN+payload.len()];
    let mut icmp_packet = MutableEchoReplyPacket::new(&mut icmp_buffer).unwrap();
    
    icmp_packet.set_icmp_type(icmp_type);
    icmp_packet.set_icmp_code(IcmpCodes::NoCode);
    icmp_packet.set_identifier(id);
    icmp_packet.set_sequence_number(seq);
    icmp_packet.set_payload(payload);
    let checksum = {
    	let p = icmp_packet.to_immutable();
    	let pkt = IcmpPacket::new(p.packet()).unwrap();
    	icmp::checksum(&pkt)
    };
    icmp_packet.set_checksum(checksum);

	ip_packet.set_payload(icmp_packet.to_immutable().packet());
    ethernet_packet.set_payload(ip_packet.to_immutable().packet());
	ethernet_packet.packet().to_vec()
}


fn send_udp_packet(source_ip: Ipv4Addr,source_mac: MacAddr, target_ip: Ipv4Addr, target_mac: MacAddr,
    source_port: u16, target_port: u16, payload: &[u8]) -> Vec<u8> {
    
    let mut ethernet_buffer = vec![0u8; ETH_HEADER_LEN+IPV4_HEADER_LEN+UDP_HEADER_LEN+payload.len()];
    let mut ethernet_packet = MutableEthernetPacket::new(&mut ethernet_buffer).unwrap();

    ethernet_packet.set_destination(target_mac);
    ethernet_packet.set_source(source_mac);
    ethernet_packet.set_ethertype(EtherTypes::Arp);

    let mut ip_buffer = vec![0u8; IPV4_HEADER_LEN+UDP_HEADER_LEN+payload.len()];
	let mut ip_packet = MutableIpv4Packet::new(&mut ip_buffer).unwrap();
    let total_len = (IPV4_HEADER_LEN + UDP_HEADER_LEN + payload.len()) as u16;
    ip_packet.set_version(4);
    ip_packet.set_header_length(5);
    ip_packet.set_total_length(total_len);
    ip_packet.set_ttl(4);
    ip_packet.set_source(source_ip);
    ip_packet.set_destination(target_ip);
    ip_packet.set_next_level_protocol(IpNextHeaderProtocols::Udp);
    let checksum = ipv4::checksum(&ip_packet.to_immutable());
    ip_packet.set_checksum(checksum);
    
    let mut udp_buffer = vec![0u8; UDP_HEADER_LEN+payload.len()];
    let mut udp_packet = MutableUdpPacket::new(&mut udp_buffer).unwrap();
    udp_packet.set_source(source_port);
    udp_packet.set_destination(target_port);
    udp_packet.set_length((UDP_HEADER_LEN + payload.len()) as u16);
    udp_packet.set_payload(payload);
    let checksum = ipv4_checksum(&udp_packet.to_immutable(), &source_ip, &target_ip);
    udp_packet.set_checksum(checksum);
    
    ip_packet.set_payload(&udp_packet.to_immutable().packet());
    ethernet_packet.set_payload(ip_packet.to_immutable().packet());
    ethernet_packet.packet().to_vec()
}

//=============================================================================
    
fn handle_udp_packet(source: IpAddr, destination: IpAddr, packet: &[u8], source_mac: MacAddr) -> Option<Vec<u8>> {
    let udp = UdpPacket::new(packet);
    if let Some(udp) = udp {
        println!(
            "UDP Packet: {}:{} > {}:{}; length: {}",
            source,
            udp.get_source(),
            destination,
            udp.get_destination(),
            udp.get_length()
        );
        if destination == config.my_ip && udp.get_destination() == 7 {
        	if let IpAddr::V4(source) = source {
		        return Some(send_udp_packet(config.my_ip,config.my_mac, source, source_mac,7,udp.get_source(),udp.payload()));
		    }
        }
    } else {
        println!("Malformed UDP Packet");
    }
    None
}


fn handle_icmp_packet(source: IpAddr, destination: IpAddr, packet: &[u8], source_mac: MacAddr, id: u16) -> Option<Vec<u8>> {
    let icmp_packet = IcmpPacket::new(packet);
    if let Some(icmp_packet) = icmp_packet {
        match icmp_packet.get_icmp_type() {
            IcmpTypes::EchoReply => {
                let echo_reply_packet = echo_reply::EchoReplyPacket::new(packet).unwrap();
                println!(
                    "ICMP echo reply {} -> {} (seq={:?}, id={:?})",
                    source,
                    destination,
                    echo_reply_packet.get_sequence_number(),
                    echo_reply_packet.get_identifier()
                );
            }
            
            IcmpTypes::EchoRequest => {
                let echo_request_packet = echo_request::EchoRequestPacket::new(packet).unwrap();
                println!(
                    "ICMP echo request {} -> {} (seq={:?}, id={:?})",
                    source,
                    destination,
                    echo_request_packet.get_sequence_number(),
                    echo_request_packet.get_identifier()
                );
                
//                if let IpAddr::V4(destination) = destination {
	                if destination == config.my_ip {
			        	if let IpAddr::V4(source) = source {
			        		println!("answering...");
	    		            return Some(send_icmp_packet(config.my_ip, config.my_mac,
	                			source, source_mac,
	                			IcmpTypes::EchoReply,
			                    echo_request_packet.get_sequence_number(),
			                    echo_request_packet.get_identifier(),
			                    echo_request_packet.payload(),
			                    id));
			            }
					}
//                }
            }
            
            _ => println!(
                "ICMP packet {} -> {} (type={:?})",
                source,
                destination,
                icmp_packet.get_icmp_type()
            ),
        }
    } else {
        println!("Malformed ICMP Packet");
    }
    None
}

fn handle_icmpv6_packet(source: IpAddr, destination: IpAddr, packet: &[u8]) -> Option<Vec<u8>> {
    let icmpv6_packet = Icmpv6Packet::new(packet);
    if let Some(icmpv6_packet) = icmpv6_packet {
        println!(
            "ICMPv6 packet {} -> {} (type={:?})",
            source,
            destination,
            icmpv6_packet.get_icmpv6_type()
        )
    } else {
        println!("Malformed ICMPv6 Packet");
    }
    None
}

fn handle_tcp_packet(source: IpAddr, destination: IpAddr, packet: &[u8]) -> Option<Vec<u8>> {
    let tcp = TcpPacket::new(packet);
    if let Some(tcp) = tcp {
        println!(
            "TCP Packet: {}:{} > {}:{}; length: {}",
            source,
            tcp.get_source(),
            destination,
            tcp.get_destination(),
            packet.len()
        );
    } else {
        println!("Malformed TCP Packet");
    }
    None
}

fn handle_transport_protocol(source: IpAddr,destination: IpAddr, protocol: IpNextHeaderProtocol, packet: &[u8], source_mac: MacAddr, id: u16) -> Option<Vec<u8>> {
    match protocol {
        IpNextHeaderProtocols::Udp => {
            handle_udp_packet(source, destination, packet, source_mac)
        }
        IpNextHeaderProtocols::Tcp => {
            handle_tcp_packet(source, destination, packet)
        }
        IpNextHeaderProtocols::Icmp => {
            handle_icmp_packet(source, destination, packet, source_mac, id)
        }
        IpNextHeaderProtocols::Icmpv6 => {
            handle_icmpv6_packet(source, destination, packet)
        }
        _ => {
        	println!(
	            "Unknown {} packet: {} > {}; protocol: {:?} length: {}",
    	        match source {
        	        IpAddr::V4(..) => "IPv4",
            	    _ => "IPv6",
	            },
    	        source,
        	    destination,
            	protocol,
	            packet.len()
    	    );
    	    None
    	},
    }
}

fn handle_ipv4_packet(ethernet: &EthernetPacket) -> Option<Vec<u8>> {
    let header = Ipv4Packet::new(ethernet.payload());
    if let Some(header) = header {
        handle_transport_protocol(
            IpAddr::V4(header.get_source()),
            IpAddr::V4(header.get_destination()),
            header.get_next_level_protocol(),
            header.payload(),
            ethernet.get_source(),
            header.get_identification()
        )
    } else {
        println!("Malformed IPv4 Packet");
	    None
    }
}

fn handle_ipv6_packet(ethernet: &EthernetPacket) -> Option<Vec<u8>> {
    let header = Ipv6Packet::new(ethernet.payload());
    if let Some(header) = header {
        handle_transport_protocol(
            IpAddr::V6(header.get_source()),
            IpAddr::V6(header.get_destination()),
            header.get_next_header(),
            header.payload(),
            ethernet.get_source(),
            0
        )
    } else {
        println!("Malformed IPv6 Packet");
	    None
    }
}

fn handle_arp_packet(ethernet: &EthernetPacket) -> Option<Vec<u8>> {
    let header = ArpPacket::new(ethernet.payload());
    if let Some(header) = header {
        println!(
            "ARP packet: {}({}) > {}({}); operation: {:?}",
            ethernet.get_source(),
            header.get_sender_proto_addr(),
            ethernet.get_destination(),
            header.get_target_proto_addr(),
            header.get_operation()
        );
        if header.get_target_proto_addr() == config.my_ip && header.get_operation() == ArpOperations::Request {
        	println!("answering...");
        	let source_ip = config.my_ip;
        	let source_mac = config.my_mac;
        	let target_ip = header.get_sender_proto_addr();
        	let target_mac = ethernet.get_source();
        	return Some(send_arp_packet(source_ip,source_mac,target_ip,target_mac,ArpOperations::Reply));
        }
    } else {
        println!("Malformed ARP Packet");
    }
    None
}


fn handle_ethernet_frame(ethernet: &EthernetPacket) -> Option<Vec<u8>> {

    match ethernet.get_ethertype() {
        EtherTypes::Ipv4 => handle_ipv4_packet(ethernet),
        EtherTypes::Ipv6 => handle_ipv6_packet(ethernet),
        EtherTypes::Arp => handle_arp_packet(ethernet),
        _ => {
        	println!(
            	"Unknown packet: {} > {}; ethertype: {:?} length: {}",
	            ethernet.get_source(),
    	        ethernet.get_destination(),
        	    ethernet.get_ethertype(),
            	ethernet.packet().len()
	        );
	        None
	    },
    }
}

fn print_buf(msg: &str, data: &[u8])
{
	println!("{}",msg);
	for i in 0..data.len() {
		print!("{:02x} ",data[i]);
		if (i & 0x0f) == 0x0f {
			println!("");
		}
	}
	println!("");
}


/// Run a shell command. Panic if it fails in any way.
fn cmd(cmd: &str, args: &[&str]) {
    let ecode = Command::new("ip")
        .args(args)
        .spawn()
        .unwrap()
        .wait()
        .unwrap();
    assert!(ecode.success(), "Failed to execte {}", cmd);
}

fn main() {

//	let cfg = Config {
//		my_ip: "10.0.0.1".parse::<Ipv4Addr>().unwrap(),
//		my_mac: MacAddr::new(1,2,3,4,5,6),
//	};
//	let serialized = serde_yaml::to_string(&cfg).unwrap();
//    println!("serialized = {}", serialized);

//   let iface_name = match env::args().nth(1) {
//        Some(n) => n,
//        None => {
//            writeln!(io::stderr(), "Usage: ethraw <NETWORK INTERFACE>").unwrap();
//            process::exit(1);
//        }
//    };

/*
    // Find the network interface with the provided name
    let interfaces = datalink::interfaces();
    let interface = interfaces
        .into_iter()
        .find(|iface: &NetworkInterface| iface.name == iface_name)
        .expect("interface not found");

    // Create a channel to receive on
    let (mut tx, mut rx) = match datalink::channel(&interface, Default::default()) {
        Ok(Ethernet(tx, rx)) => (tx, rx),
        Ok(_) => panic!("unhandled channel type: {}"),
        Err(e) => panic!("unable to create channel: {}", e),
    };
*/
	let iface = Iface::without_packet_info("raw1", Mode::Tap).unwrap();
	cmd("ip", &["link", "set", "up", "dev", iface.name()]);
    let mut buffer = vec![0; 1504];
    loop {
		let size = iface.recv(&mut buffer).unwrap();
    
//        match rx.next() {
//            Ok(packet) => {
                if let Some(answ) = handle_ethernet_frame(&EthernetPacket::new(&buffer[..size]).unwrap()) {
                	print_buf("answ:",&answ);
                	iface.send(&answ).unwrap();
                }
//            }
//            Err(e) => panic!("unable to receive packet: {}", e),
//        }
    }
}
