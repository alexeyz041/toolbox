#!/bin/bash

rm -f 123
wget --ca-certificate=ca.pem https://127.0.0.1:8443/get/123
cat 123


