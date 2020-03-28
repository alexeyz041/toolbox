#!/bin/bash

rm -f 123
wget --ca-certificate=ca.pem https://127.0.0.1:8443/get/123
cat 123

curl -X POST --data-binary 'Hello' https://127.0.0.1:8443/post --cacert ca.pem

rm -f 123
wget --ca-certificate=ca.pem https://localhost:8443/get/123
cat 123

curl -X POST --data-binary 'Hello' https://localhost:8443/post --cacert ca.pem




