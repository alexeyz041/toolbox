FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive
RUN apt update

RUN apt install --no-install-recommends -y cmake build-essential msitools mingw-w64 git wget ca-certificates

RUN update-ca-certificates

RUN mkdir -p /appstream/build.mingw64

RUN cd /appstream/build.mingw64 && \
    wget https://gstreamer.freedesktop.org/data/pkg/windows/1.18.5/mingw/gstreamer-1.0-mingw-x86_64-1.18.5.msi && \
    msiextract gstreamer-1.0-mingw-x86_64-1.18.5.msi

RUN cd /appstream/build.mingw64 && \
    wget https://gstreamer.freedesktop.org/data/pkg/windows/1.18.5/mingw/gstreamer-1.0-devel-mingw-x86_64-1.18.5.msi && \
    msiextract gstreamer-1.0-devel-mingw-x86_64-1.18.5.msi

RUN apt install --no-install-recommends -y autoconf automake libtool

RUN cd /appstream/build.mingw64 && \
    git clone https://github.com/yaml/libyaml.git && \
    cd libyaml && \
    ./bootstrap && \
    ./configure --host=x86_64-w64-mingw32 && \
    make

COPY *.c *.h /appstream/build.mingw64/

COPY build.mingw64/build.sh build.mingw64/copy.sh /appstream/build.mingw64/

RUN mkdir -p /appstream/build.mingw64/out

RUN apt install --no-install-recommends -y dos2unix
RUN cd /appstream/build.mingw64 && dos2unix build.sh copy.sh
RUN cd /appstream/build.mingw64 && ./build.sh

WORKDIR "/appstream/build.mingw64"
ENTRYPOINT [ "./copy.sh" ]
