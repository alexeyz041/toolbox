
use std::env;

fn main() {

    let input = env::args().nth(1).expect("Missing input string");
    let words = input.split_whitespace().collect::<Vec<&str>>();
    let max_len = words.iter().map(|x|x.len()).max().unwrap();
    let header = "*".repeat(max_len + 4) + "\n";
    
    print!("{}{}{}",
		    header,
        	words.iter().map(|x| format!("* {:width$} *\n", x, width = max_len)).collect::<String>(),
        	header);
}
