// Copyright (c) 2014, 2015 Robert Clipsham <robert@octarineparrot.com>
//
// Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
// http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
// <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
// option. This file may not be copied, modified, or distributed
// except according to those terms.

extern crate termion;
#[macro_use]
extern crate lazy_static;
extern crate dns_lookup;

use dns_lookup::lookup_addr;

/// This example shows a basic packet logger using libpnet
extern crate pnet;

use pnet::datalink::{self, NetworkInterface};
use pnet::datalink::Channel::Ethernet;
use pnet::packet::arp::ArpPacket;
use pnet::packet::ethernet::{EtherTypes, EthernetPacket, MutableEthernetPacket};
use pnet::packet::icmp::{echo_reply, echo_request, IcmpPacket, IcmpTypes};
use pnet::packet::icmpv6::Icmpv6Packet;
use pnet::packet::ip::{IpNextHeaderProtocol, IpNextHeaderProtocols};
use pnet::packet::ipv4::Ipv4Packet;
use pnet::packet::ipv6::Ipv6Packet;
use pnet::packet::tcp::TcpPacket;
use pnet::packet::udp::UdpPacket;
use pnet::packet::Packet;
use pnet::util::MacAddr;

use std::env;
use std::io::{self, Write};
use std::net::IpAddr;
use std::process;
use std::collections::HashMap;
use std::sync::Mutex;

lazy_static! {
    static ref COMM: Mutex<HashMap<String, (u16, u64)>> = Mutex::new(HashMap::new());
    static ref HOSTS: Mutex<HashMap<String, String>> = Mutex::new(HashMap::new());
    static ref MYIP: Mutex<String> = Mutex::new(String::new());
}


fn handle_udp_packet(_interface_name: &str, source: IpAddr, destination: IpAddr, packet: &[u8]) {
    let udp = UdpPacket::new(packet);

    if let Some(udp) = udp {
//        println!(
//            "[{}]: UDP Packet: {}:{} > {}:{}; length: {}",
//            interface_name,
//            source,
//            udp.get_source(),
//            destination,
//            udp.get_destination(),
//            udp.get_length()
//        );

        dump(&format!("{}", source),
             &format!("{}", destination),
             "UDP",
             udp.get_destination(),
             udp.get_length() as u64);

    } else {
//        println!("[{}]: Malformed UDP Packet", interface_name);

        dump("?", "?", "UDP", 0, packet.len() as u64);
    }
}

fn handle_icmp_packet(_interface_name: &str, source: IpAddr, destination: IpAddr, packet: &[u8]) {
    let icmp_packet = IcmpPacket::new(packet);
    if let Some(icmp_packet) = icmp_packet {
        match icmp_packet.get_icmp_type() {
            IcmpTypes::EchoReply => {
                let _echo_reply_packet = echo_reply::EchoReplyPacket::new(packet).unwrap();
//                println!(
//                    "[{}]: ICMP echo reply {} -> {} (seq={:?}, id={:?})",
//                    interface_name,
//                    source,
//                    destination,
//                    echo_reply_packet.get_sequence_number(),
//                    echo_reply_packet.get_identifier()
//                );
                 dump(&format!("{}", source),
                      &format!("{}", destination),
                      "ICMP Rpl",
                      0,
                      packet.len() as u64);
            }
            IcmpTypes::EchoRequest => {
                let _echo_request_packet = echo_request::EchoRequestPacket::new(packet).unwrap();
//                println!(
//                    "[{}]: ICMP echo request {} -> {} (seq={:?}, id={:?})",
//                    interface_name,
//                    source,
//                    destination,
//                    echo_request_packet.get_sequence_number(),
//                    echo_request_packet.get_identifier()
//                );
                 dump(&format!("{}", source),
                      &format!("{}", destination),
                      "ICMP Req",
                      0,
                      packet.len() as u64);
            }
            _ => 
//                 println!(
//                    "[{}]: ICMP packet {} -> {} (type={:?})",
//                    interface_name,
//                    source,
//                    destination,
//                    icmp_packet.get_icmp_type())

                 dump(&format!("{}", source),
                      &format!("{}", destination),
                      &format!("ICMP {:?}",icmp_packet.get_icmp_type()),
                      0,
                      packet.len() as u64),
        }
    } else {
//        println!("[{}]: Malformed ICMP Packet", interface_name);
        dump("?", "?", "ICMP", 0, packet.len() as u64);
    }
}


fn handle_icmpv6_packet(_interface_name: &str, source: IpAddr, destination: IpAddr, packet: &[u8]) {
    let icmpv6_packet = Icmpv6Packet::new(packet);
    if let Some(icmpv6_packet) = icmpv6_packet {
//        println!(
//            "[{}]: ICMPv6 packet {} -> {} (type={:?})",
//            interface_name,
//            source,
//            destination,
//            icmpv6_packet.get_icmpv6_type())

             dump(&format!("{}", source),
                  &format!("{}", destination),
                  &format!("ICMPv6 {:?}",icmpv6_packet.get_icmpv6_type()),
                  0,
                  packet.len() as u64);

    } else {
//        println!("[{}]: Malformed ICMPv6 Packet", interface_name);
        dump("?", "?", "ICMPv6", 0, packet.len() as u64);
    }
}

fn handle_tcp_packet(_interface_name: &str, source: IpAddr, destination: IpAddr, packet: &[u8]) {
    let tcp = TcpPacket::new(packet);
    if let Some(tcp) = tcp {
//        println!(
//            "[{}]: TCP Packet: {}:{} > {}:{}; length: {}",
//            interface_name,
//            source,
//            tcp.get_source(),
//            destination,
//            tcp.get_destination(),
//            packet.len()
//        );

        dump(&format!("{}", source),
             &format!("{}", destination),
             "TCP",
             tcp.get_destination(),
             packet.len() as u64);

    } else {
//        println!("[{}]: Malformed TCP Packet", interface_name);
        dump("?", "?", "TCP", 0, packet.len() as u64);
    }
}


fn handle_transport_protocol(interface_name: &str, source: IpAddr, destination: IpAddr,
        protocol: IpNextHeaderProtocol, packet: &[u8]) {

    match protocol {
        IpNextHeaderProtocols::Udp => {
            handle_udp_packet(interface_name, source, destination, packet)
        }
        IpNextHeaderProtocols::Tcp => {
            handle_tcp_packet(interface_name, source, destination, packet)
        }
        IpNextHeaderProtocols::Icmp => {
            handle_icmp_packet(interface_name, source, destination, packet)
        }
        IpNextHeaderProtocols::Icmpv6 => {
            handle_icmpv6_packet(interface_name, source, destination, packet)
        }
        _ => 
//            println!("[{}]: Unknown {} packet: {} > {}; protocol: {:?} length: {}",
//                interface_name,
//                match source {
//                    IpAddr::V4(..) => "IPv4",
//                    _ => "IPv6",
//                },
//                source,
//                destination,
//                protocol,
//                packet.len())

        dump(&format!("{}", source),
             &format!("{}", destination),
             match source {
                IpAddr::V4(..) => "IPv4 ?",
                _ => "IPv6 ?",
             },
             0,
             packet.len() as u64),
    }
}


fn handle_ipv4_packet(interface_name: &str, ethernet: &EthernetPacket) {
    let header = Ipv4Packet::new(ethernet.payload());
    if let Some(header) = header {
        handle_transport_protocol(interface_name,
            IpAddr::V4(header.get_source()),
            IpAddr::V4(header.get_destination()),
            header.get_next_level_protocol(),
            header.payload());
    } else {
//        println!("[{}]: Malformed IPv4 Packet", interface_name);
        dump("?", "?", "IPv4", 0, ethernet.payload().len() as u64);
    }
}

fn handle_ipv6_packet(interface_name: &str, ethernet: &EthernetPacket) {
    let header = Ipv6Packet::new(ethernet.payload());
    if let Some(header) = header {
        handle_transport_protocol(interface_name,
            IpAddr::V6(header.get_source()),
            IpAddr::V6(header.get_destination()),
            header.get_next_header(),
            header.payload());
    } else {
//        println!("[{}]: Malformed IPv6 Packet", interface_name);
        dump("?", "?", "IPv6", 0, ethernet.payload().len() as u64);
    }
}


fn handle_arp_packet(_interface_name: &str, ethernet: &EthernetPacket) {
    let header = ArpPacket::new(ethernet.payload());
    if let Some(header) = header {
//        println!(
//            "[{}]: ARP packet: {}({}) > {}({}); operation: {:?}",
//            interface_name,
//            ethernet.get_source(),
//            header.get_sender_proto_addr(),
//            ethernet.get_destination(),
//            header.get_target_proto_addr(),
//            header.get_operation());

        dump(&format!("{}", header.get_sender_proto_addr()),
             &format!("{}", header.get_target_proto_addr()),
             "ARP",
             0,
             ethernet.payload().len() as u64);

    } else {
//        println!("[{}]: Malformed ARP Packet", interface_name);
        dump("?", "?", "ARP", 0, ethernet.payload().len() as u64);
    }
}


fn handle_ethernet_frame(interface: &NetworkInterface, ethernet: &EthernetPacket) {
    let interface_name = &interface.name[..];
    match ethernet.get_ethertype() {
        EtherTypes::Ipv4 => handle_ipv4_packet(interface_name, ethernet),
        EtherTypes::Ipv6 => handle_ipv6_packet(interface_name, ethernet),
        EtherTypes::Arp => handle_arp_packet(interface_name, ethernet),
        _ =>
//            println!("[{}]: Unknown packet: {} > {}; ethertype: {:?} length: {}",
//                interface_name,
//                ethernet.get_source(),
//                ethernet.get_destination(),
//                ethernet.get_ethertype(),
//                ethernet.packet().len())

        dump(&format!("{}", ethernet.get_source()),
             &format!("{}", ethernet.get_destination()),
             &format!("ET {:?}", ethernet.get_ethertype()),
             0,
             ethernet.payload().len() as u64),
    }
}


fn main() {
    let iface_name = match env::args().nth(1) {
        Some(n) => n,
        None => {
            writeln!(io::stderr(), "Usage: packetdump <network intrface>").unwrap();
            process::exit(1);
        }
    };
    let interface_names_match = |iface: &NetworkInterface| iface.name == iface_name;

    // Find the network interface with the provided name
    let interfaces = datalink::interfaces();
    let interface = interfaces
        .into_iter()
        .filter(interface_names_match)
        .next()
        .unwrap();

    {
        let mut my_ips = MYIP.lock().unwrap();
        let my_ip = interface
            .ips
            .iter()
            .find(|ip| ip.is_ipv4())
            .map(|ip| match ip.ip() {
                IpAddr::V4(ip) => ip,
                _ => unreachable!(),
             })
            .unwrap();
        *my_ips = format!("{}", my_ip);
    }

    // Create a channel to receive on
    let (_, mut rx) = match datalink::channel(&interface, Default::default()) {
        Ok(Ethernet(tx, rx)) => (tx, rx),
        Ok(_) => panic!("packetdump: unhandled channel type"),
        Err(e) => panic!("packetdump: unable to create channel: {}", e),
    };

    cls();
    loop {
        let mut buf: [u8; 1600] = [0u8; 1600];
        let mut fake_ethernet_frame = MutableEthernetPacket::new(&mut buf[..]).unwrap();
        match rx.next() {
            Ok(packet) => {
                if cfg!(target_os = "macos") && interface.is_up() && !interface.is_broadcast()
                    && !interface.is_loopback() && interface.is_point_to_point()
                {
                    // Maybe is TUN interface
                    let version = Ipv4Packet::new(&packet).unwrap().get_version();
                    if version == 4 {
                        fake_ethernet_frame.set_destination(MacAddr(0, 0, 0, 0, 0, 0));
                        fake_ethernet_frame.set_source(MacAddr(0, 0, 0, 0, 0, 0));
                        fake_ethernet_frame.set_ethertype(EtherTypes::Ipv4);
                        fake_ethernet_frame.set_payload(&packet);
                        handle_ethernet_frame(&interface, &fake_ethernet_frame.to_immutable());
                        continue;
                    } else if version == 6 {
                        fake_ethernet_frame.set_destination(MacAddr(0, 0, 0, 0, 0, 0));
                        fake_ethernet_frame.set_source(MacAddr(0, 0, 0, 0, 0, 0));
                        fake_ethernet_frame.set_ethertype(EtherTypes::Ipv6);
                        fake_ethernet_frame.set_payload(&packet);
                        handle_ethernet_frame(&interface, &fake_ethernet_frame.to_immutable());
                        continue;
                    }
                }
                handle_ethernet_frame(&interface, &EthernetPacket::new(packet).unwrap());
            }
            Err(e) => panic!("packetdump: unable to receive packet: {}", e),
        }
    }
}


fn cls() {
    print!("{}", termion::clear::All);
}

fn goto(x: u16, y: u16) {
    print!("{}", termion::cursor::Goto(x+1, y+1));
}

fn get_host_name(ip: &str) -> String {
    if ip == "239.255.255.250" {
        return "broadcast".to_string();
    }

    let mut hosts = HOSTS.lock().unwrap();
    if let Some(host) = hosts.get(&ip.to_string()) {
        return host.to_string();
    }
    let ip: IpAddr = ip.parse().unwrap();
    let h = lookup_addr(&ip).unwrap_or("".to_string());
    hosts.insert(ip.to_string(), h.clone());
    h
}


fn dump(from: &str, to: &str, tp: &str, port: u16, n: u64) {
    if to == "239.255.255.250" {
        dump1(from, to, "SSDP", port, n);
        return;
    }
    if to == "224.0.0.251" {
        dump1(from, to, "mDNS", 0, n);
        return;
    }

    dump1(from, to, tp, port, n);
}

fn dump1(from: &str, to: &str, tp: &str, port: u16, n: u64) {
    let key = format!("{}|{}|{}", from, to, tp);
    let rkey = format!("{}|{}|{}", to, from, tp);
    let mut c = COMM.lock().unwrap();
    let (row, count, rev) = if let Some((row, count)) = c.get(&key) {
        (*row, *count+n, false)
    } else if let Some((row, count)) = c.get(&rkey) {
        (*row, *count+n, true)
    } else {
        (c.len() as u16, n, false)
    };

    goto(0, row);
    let me = { MYIP.lock().unwrap() };
    let p = if port != 0 { format!("port {:<5}", port) } else { "".to_string() };
    if rev {
        c.insert(rkey, (row, count));
        println!("{:<15} {:<15} {:<10} {:<10} {:<10} {}",
            if to == *me { "me" } else { to },
            if from == *me { "me" } else { from },
            tp,
            count,
            p,
            get_host_name(from));
    } else {
        c.insert(key, (row, count));
        println!("{:<15} {:<15} {:<10} {:<10} {:<10} {}",
            if from == *me { "me" } else { from },
            if to == *me { "me" } else { to },
            tp,
            count,
            p,
            get_host_name(to));
    }
}

