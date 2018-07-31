
extern crate ring;
use ring::{ rand, signature};

extern crate untrusted;

use std::path::Path;
use std::fs::File;
use std::io::Read;
use std::io::prelude::*;

#[macro_use]
extern crate clap;
use clap::App;


// Create an `RSAKeyPair` from the DER-encoded bytes. This example uses
// a 2048-bit key, but larger keys are also supported.
fn sign_rsa(message: &[u8], private_key_path: &std::path::Path, sig: &String) -> Result<(), MyError> {
	let private_key_der = read_file(private_key_path)?;
	let private_key_der = untrusted::Input::from(&private_key_der);
	let key_pair = signature::RSAKeyPair::from_der(private_key_der)
    				.map_err(|ring::error::Unspecified| MyError::BadPrivateKey)?;

	// Create a signing state.
	let key_pair = std::sync::Arc::new(key_pair);
	let mut signing_state = signature::RSASigningState::new(key_pair)
    				.map_err(|ring::error::Unspecified| MyError::OOM)?;

	// Sign the message "hello, world", using PKCS#1 v1.5 padding and the
	// SHA256 digest algorithm.
	
	let rng = rand::SystemRandom::new();
	let mut signature = vec![0; signing_state.key_pair().public_modulus_len()];
	signing_state.sign(&signature::RSA_PKCS1_SHA256, &rng, message, &mut signature)
    				.map_err(|ring::error::Unspecified| MyError::OOM)?;

	save_file(sig,&signature).map_err(|e| MyError::IO(e))?;
	Ok(())
}

// Verify the signature.
fn verify_rsa(message: &[u8], public_key_path: &std::path::Path, sig: &String) -> Result<(), MyError>
{
	let public_key_der = read_file(public_key_path)?;
	let public_key_der = untrusted::Input::from(&public_key_der);
	let message = untrusted::Input::from(message);
	
	let signature1 = read_file(Path::new(sig))?;
	let signature = untrusted::Input::from(&signature1);
	
	signature::verify(&signature::RSA_PKCS1_2048_8192_SHA256,  public_key_der, message, signature)
    				.map_err(|ring::error::Unspecified| MyError::BadSignature)?;

	Ok(())
}


#[derive(Debug)]
enum MyError {
   IO(std::io::Error),
   BadPrivateKey,
   OOM,
   BadSignature,
}

fn read_file(path: &std::path::Path) -> Result<Vec<u8>, MyError> {
    let mut file = std::fs::File::open(path).map_err(|e| MyError::IO(e))?;
    let mut contents: Vec<u8> = Vec::new();
    file.read_to_end(&mut contents).map_err(|e| MyError::IO(e))?;
    Ok(contents)
}



fn save_file(fnm: &String, sig: &Vec<u8>) -> std::io::Result<()>
{
	let mut file = File::create(fnm)?;
    file.write_all(sig)?;
    Ok(())
}


fn main()
{
	let yaml = load_yaml!("cli.yaml");
    let matches = App::from_yaml(yaml).get_matches();

	let input = matches.value_of("INPUT").unwrap();
    println!("Input file: {}", input);

	let key = matches.value_of("key").unwrap();
    println!("Key: {}", key);

	let msg = read_file(Path::new(input)).unwrap();
	let sig = format!("{}.sig",input);
	
	if matches.is_present("sign") {
		sign_rsa(&msg, Path::new(key), &sig).unwrap();
		println!("signed.");
	} else if matches.is_present("verify") {
		let result = verify_rsa(&msg, Path::new(key), &sig);
		if result.is_ok() {
			println!("signature ok.");
		} else {
			println!("error {:?}",result);
		}
	} else {
		println!("no sing or verify option");
	}
}


