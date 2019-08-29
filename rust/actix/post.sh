#!/bin/bash

openssl ecparam -name brainpoolP512t1 -genkey -noout -out key.pem 
openssl req -new -key key.pem -batch -out csr.pem -subj "/C=FI/ST=Pm/O=Client 1/CN=User"
curl -v -X POST --data-binary @csr.pem http://127.0.0.1:8080/sign > cert.pem
cat cert.pem

