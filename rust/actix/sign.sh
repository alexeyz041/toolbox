#!/bin/bash

set -x

# create files for CA operations
if [ ! -f index.txt ]; then
    echo -n >index.txt
    echo "unique_subject = no" >index.txt.attr
    echo "01" >serial
fi

openssl ca -batch -in /tmp/csr.pem -out /tmp/cert.pem -config clca.conf



