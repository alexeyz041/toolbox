FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive
RUN apt update

RUN apt install --no-install-recommends -y cmake build-essential git python3-pip ninja-build flex bison pkg-config mc
RUN pip3 install --user meson

RUN mkdir -p /gstreamer
COPY build-gstreamer.sh /gstreamer/

RUN cd /gstreamer && ./build-gstreamer.sh

WORKDIR "/gstreamer"
ENTRYPOINT [ "./gst-launch-1.0" ]
