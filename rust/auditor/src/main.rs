
extern crate sysinfo;

use sysinfo::SystemExt;
use sysinfo::ProcessExt;

use std::cmp::Ordering;
use std::{thread, time};
use std::collections::HashMap;

struct Proc {
	p : sysinfo::Process,
	id : sysinfo::Pid
}

impl Ord for Proc {
    fn cmp(&self, other: &Proc) -> Ordering {
        self.id.cmp(&other.id)
    }
}

impl PartialOrd for Proc {
    fn partial_cmp(&self, other: &Proc) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

impl PartialEq for Proc {
    fn eq(&self, other: &Proc) -> bool {
        self.id == other.id
    }
}

impl Eq for Proc {}


fn diff(old: &HashMap<sysinfo::Pid, sysinfo::Process>, new: &HashMap<sysinfo::Pid, sysinfo::Process>)
{
	for (pid, p) in old {
    	if !new.contains_key(pid) {
    		println!("{}:{} stopped", pid, p.name());
    	}
	}
	for (pid, p) in new {
    	if !old.contains_key(pid) {
    		println!("{}:{} started", pid, p.name());
    	}
	}
}



fn main()
{
	let mut system = sysinfo::System::new();
	system.refresh_all();

	let mut plist : Vec<Proc> = vec![];

	for (pid, proc_) in system.get_process_list() {
    	plist.push(Proc { p : proc_.clone(), id : *pid });
	}

	plist.sort();

	for p in plist {
    	println!("{}:{} => status: {:?}", p.id, p.p.name(), p.p.status());
	}

	loop {
		let mut system2 = sysinfo::System::new();
		system2.refresh_all();
		
		diff(system.get_process_list(),system2.get_process_list());
		
		system = system2;
		thread::sleep(time::Duration::from_millis(1000));
		println!(".");
	}
}
