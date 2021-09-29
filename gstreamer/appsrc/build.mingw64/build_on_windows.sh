#!/bin/bash
set -x

cd ..
docker build -t appstream-mingw64 -f build.mingw64/Dockerfile .
mkdir -p c:/out
docker run --rm -v c:/out:/appstream/build.mingw64/out appstream-mingw64
#sleep 60
