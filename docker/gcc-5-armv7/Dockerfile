FROM lasote/conangcc5-armv7
MAINTAINER Manu Sanchez <Manu343726@gmail.com>

USER root

# Install libtinfo required by LLVM
RUN echo "deb http://us.archive.ubuntu.com/ubuntu/ trusty multiverse" >> /etc/apt/sources.list && \
    apt update && \
    apt install -y libtinfo-dev

ENV CC=gcc \
    CXX=g++ \
    CMAKE_C_COMPILER=gcc \
    CMAKE_CXX_COMPILER=g++

COPY toolchain.cmake /usr/share/

USER conan
WORKDIR /home/conan
