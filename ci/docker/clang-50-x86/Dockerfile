FROM geordiejones/cpp14-build-machine
MAINTAINER Manu Sanchez <Manu343726@gmail.com>

# Install libtinfo required by LLVM
RUN echo "deb http://us.archive.ubuntu.com/ubuntu/ trusty multiverse" >> /etc/apt/sources.list && \
    apt update && \
    apt install -y libtinfo-dev
