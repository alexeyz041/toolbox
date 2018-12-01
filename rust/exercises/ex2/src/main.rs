//https://doc.rust-lang.org/rust-by-example/std_misc/threads/testcase_mapreduce.html


use std::thread;

fn main()
{
    let data = "869678977374164 71853297327050364959
				118613225755647 23963297542624962850
				708562347018608 51907960690014725639
				383979667071060 94172783238747669219
				523807952578882 36525459303330302837
				584953271357440 41048897885734297812
				699202164389808 73548808413720956532
				162784246374525 89860345374828574668";

    let mut children = vec![];
	let cd = data.replace(" ","").replace("\n","").replace("\t","").to_string();
	
	let chunked_data = cd.as_bytes()
		    .chunks(30)
		    .map(|x| String::from_utf8(x.to_vec()))
 	   		.collect::<Result<Vec<String>, _>>()
    		.unwrap();

	let mut i = 0;
    for data_segment in chunked_data {
        println!("data segment {} is \"{}\"", i, data_segment);

        children.push(thread::spawn(move || -> u32 {
            let result = data_segment
                        .chars()
                        .map(|c| c.to_digit(10).expect("should be a digit"))
                        .sum();

            println!("processed segment {}, result={}", i, result);
            result
        }));
        i += 1;
    }

    let mut intermediate_sums = vec![];
    for child in children {
        let intermediate_sum = child.join().unwrap();
        intermediate_sums.push(intermediate_sum);
    }

    let final_result = intermediate_sums.iter().sum::<u32>();
    println!("Final sum result: {}", final_result);
}

