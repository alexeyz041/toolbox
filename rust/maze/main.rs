use std::time::Duration;
use std::collections::HashMap;

use tokio_modbus::prelude::*;
use tokio_modbus::client::sync::Context;

const MAP_SIZE: i32 = 25;

const MOVE_REG: u16 = 1;

const NAV1_REG: u16 = 21;
const NAV2_REG: u16 = 22;
const NAV3_REG: u16 = 23;
const NAV4_REG: u16 = 24;

const KEY_REG: u16 = 15;
const DOOR_REG: u16 = 6;
const STATE_REG: u16 = 20;

const MAP_ADDR_REG: u16 = 8;
const MAP_CONTENT_REG: u16 = 18;

const NORTH: u16 = 1;
const EAST: u16 = 2;
const SOUTH: u16 = 3;
const WEST: u16 = 4;

struct Walker {
    ctx: Context,
    key: u32,
    around: HashMap<(i32, i32), u16>,
    door_x: i32,
    door_y: i32,
    escape: Vec<(i32, i32)>,
    path1: Vec<(i32, i32)>,
    path2: Vec<(i32, i32)>,
    path3: Vec<(i32, i32)>,
    path4: Vec<(i32, i32)>,
}

impl Default for Walker {
    fn default() -> Self {
        let socket_addr = "127.0.0.1:5020".parse().unwrap();
        let mut ctx = sync::tcp::connect(socket_addr).unwrap();
        ctx.set_timeout(Duration::new(10, 0));
        Self {
            ctx,
            key : 0,
            around: HashMap::new(),
            door_x: 0,
            door_y: 0,
            escape: Vec::new(),
            path1:  Vec::new(),
            path2:  Vec::new(),
            path3:  Vec::new(),
            path4:  Vec::new(),
        }
    }
}

impl Walker {
    fn read_reg(&mut self, addr: u16) -> u16 {
        let data = self.ctx.read_input_registers(addr - 1, 1).unwrap().unwrap();
        data[0]
    }

    fn read_reg32(&mut self, addr: u16) -> u32 {
        let data = self.ctx.read_input_registers(addr - 1, 2).unwrap().unwrap();
        ((data[0] as u32) | ((data[1] as u32) << 16)).into()
    }

    fn write_reg(&mut self, addr: u16, val: u16) {
        self.ctx.write_single_register(addr - 1, val).unwrap().unwrap();
    }

    fn write_reg32(&mut self, addr: u16, val: u32) {
        let values : [u16; 2] = [
            (val & 0xffff) as u16,
            ((val >> 16) & 0xffff) as u16
        ];
        self.ctx.write_multiple_registers(addr - 1, &values).unwrap().unwrap();
    }

    fn get_map(&mut self, y: i32, x: i32) -> u16 {
        self.write_reg32(MAP_ADDR_REG, ((x & 0xffff) as u32) | (((y & 0xffff) as u32) << 16));
        self.read_reg(MAP_CONTENT_REG)
    }

    fn find_key(&mut self, nav: u16) -> Vec<u16> {
        let mut path = Vec::new();
        loop {
            let n = self.read_reg(nav);
            if n == 0 {
                println!("found key {}", nav);
                self.key ^= self.read_reg32(KEY_REG);
                return path;
            }
            println!("move {}", n);
            self.write_reg(MOVE_REG, n);
            path.push(n);
        }
    }

    fn open_door(&mut self) {
        self.write_reg32(DOOR_REG, self.key);
        if self.read_reg(STATE_REG) == 12 {
            println!("door is open");
        } else {
            panic!("wrong key");
        }
    }

    fn print_map(&mut self) {
        for y in -MAP_SIZE..MAP_SIZE {
            for x in -MAP_SIZE..MAP_SIZE {
                let mut val = if self.path_contains(&self.escape, (x, y)) { 10 } else { 0 };
                //if val == 0 {val += if self.path_contains(&self.path1, (x, y)) { 20 } else { 0 }};
                //if val == 0 {val += if self.path_contains(&self.path2, (x, y)) { 30 } else { 0 }};
                //if val == 0 {val += if self.path_contains(&self.path3, (x, y)) { 40 } else { 0 }};
                //if val == 0 {val += if self.path_contains(&self.path4, (x, y)) { 50 } else { 0 }};

                match self.scan(x, y) + val {
                    0 => eprint!(" "), // Empty
                    1 => eprint!("█"), // Wall
                    2 => eprint!("K"), // Key
                    3 => eprint!("D"), // Door

                    10 => eprint!("."), // Escape path
                    12 => eprint!("K"), // Key
                    13 => eprint!("d"), // door

                    20 => eprint!("a"), // way to key 1
                    22 => eprint!("K"), // Key
                    30 => eprint!("b"), // way to key 2
                    32 => eprint!("K"), // Key
                    40 => eprint!("c"), // way to key 3
                    42 => eprint!("K"), // Key
                    50 => eprint!("e"), // way to key 4
                    52 => eprint!("K"), // Key

                    _ => todo!(),
                }
            }
            println!();
        }
    }

    fn scan(&mut self, x: i32, y: i32) -> u16 {
        if let Some(m) = self.around.get(&(x, y)) {
            return *m
        }
        let m = self.get_map(x, y);
        self.around.insert((x,y), m);
        m
    }

    fn scan_around(&mut self) {
        for delta in 1..MAP_SIZE {
            println!("delta {}", delta);
            for offset in 0..delta {
                 if self.scan(offset, -delta) == 3 {
                    (self.door_x, self.door_y) = (offset, -delta);
                    println!("found door at {} {}", self.door_x, self.door_y);
                    return;
                }
                if self.scan(offset, delta) == 3 {
                    (self.door_x, self.door_y) = (offset, delta);
                    println!("found door at {} {}", self.door_x, self.door_y);
                    return;
                }
                if self.scan(delta, offset) == 3 {
                    (self.door_x, self.door_y) = (delta, offset);
                    println!("found door at {} {}", self.door_x, self.door_y);
                    return;
                }
                if self.scan(-delta, offset) == 3 {
                    (self.door_x, self.door_y) = (-delta, offset);
                    println!("found door at {} {}", self.door_x, self.door_y);
                    return;
                }
            }
        }
        panic!("no door");
    }

    fn steps_from(&mut self, x: i32, y: i32) -> Vec<(i32, i32)> {
        let mut steps = Vec::new();
        if self.scan(x, y + 1) != 1 {
            steps.push((x, y + 1));
        }
        if self.scan(x, y - 1) != 1 {
            steps.push((x, y - 1));
        }
        if self.scan(x + 1, y) != 1 {
            steps.push((x + 1, y));
        }
        if self.scan(x - 1 , y) != 1 {
            steps.push((x - 1, y));
        }
        steps
    }

    fn path_contains(&self, path: &Vec<(i32, i32)>, step: (i32, i32)) -> bool {
        for p in path {
            if p == &step {
                return true;
            }
        }
        false
    }

    fn path_to_door(&mut self, path: Vec<(i32, i32)>) {
        if path.last().unwrap() == &(self.door_x, self.door_y) {
            if path.len() < self.escape.len() || self.escape.len() == 0 {
                println!("found path to door {}", path.len());
                self.escape = path;
            }
            return;
        }
        let steps = self.steps_from(path.last().unwrap().0, path.last().unwrap().1);
        for step in steps {
            if self.path_contains(&path, step) {
                continue;
            }
            let mut path = path.clone();
            path.push(step);
            self.path_to_door(path);
        }
    }

    fn find_path_to_door(&mut self) -> Vec<u16> {
        self.path_to_door(vec![(0, 0)]);
        let mut nav = Vec::new();
        let mut x = 0;
        let mut y = 0;
        for step in self.escape.clone() {
            if step.0 < x { nav.push(EAST); }
            else if step.0 > x { nav.push(WEST); }
            else if step.1 < y { nav.push(NORTH); }
            else if step.1 > y { nav.push(SOUTH); }
            (x, y) = step;
        }
        return nav;
    }

    fn go(&mut self, path: &[u16]) {
        for dir in path {
            println!("move {}", *dir);
            self.write_reg(MOVE_REG, *dir);
        }
        if self.read_reg(STATE_REG) == 42 {
            println!("at the door");
        } else {
            println!("wrong path");
        }
    }

    fn path_to_coordinates(&self, mut x: i32, mut y: i32, path: &Vec<u16>) -> Vec<(i32, i32)> {
        let mut nav = Vec::new();
        nav.push((x, y));
        for step in path.iter().rev() {
            match step {
              &EAST => x += 1,
              &WEST => x -= 1,
              &NORTH => y += 1,
              &SOUTH => y -= 1,
              _ => todo!(),
            }
            nav.push((x, y));
        }
        return nav;
    }

}


fn main() {
    let mut walker = Walker::default();
    let path1 = walker.find_key(NAV1_REG);  
    let path2 = walker.find_key(NAV2_REG);
    let path3 = walker.find_key(NAV3_REG);  
    let path4 = walker.find_key(NAV4_REG);  

    walker.path4 = walker.path_to_coordinates(0, 0, &path4);
    walker.path3 = walker.path_to_coordinates(walker.path4.last().unwrap().0, walker.path4.last().unwrap().1, &path3);
    walker.path2 = walker.path_to_coordinates(walker.path3.last().unwrap().0, walker.path3.last().unwrap().1, &path2);
    walker.path1 = walker.path_to_coordinates(walker.path2.last().unwrap().0, walker.path2.last().unwrap().1, &path1);

    walker.open_door();
    walker.scan_around();
    let path = walker.find_path_to_door();
    walker.go(&path);
    walker.print_map();
}

