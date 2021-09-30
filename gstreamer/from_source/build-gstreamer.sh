#!/bin/bash

# Set your target branch
BRANCH="1.19.2"

exec > >(tee build-gstreamer.log)
exec 2>&1

git config --global http.sslVerify false
git clone https://gitlab.freedesktop.org/gstreamer/gst-build.git

export PATH=$PATH:~/.local/bin
cd gst-build
mkdir build
meson build
ninja -C build
meson install -C build
ldconfig
gst-launch-1.0 --version

