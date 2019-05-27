extern crate serde;
extern crate serde_json;
#[macro_use] extern crate serde_derive;

#[derive(Serialize, Deserialize, Debug)]
struct Address {
    street: String,
    city: String,
}


fn main() {
    let address = Address {
        street: "10 Downing Street".to_owned(),
        city: "London".to_owned(),
    };

    let j = serde_json::to_string(&address).unwrap();
    println!("{}", j);

    let a: Address = serde_json::from_str(&j).unwrap();
    println!("{:?}", a);
}

