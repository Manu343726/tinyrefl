ARG BASE_IMAGE
FROM $BASE_IMAGE
MAINTAINER Manu Sanchez <Manu343726@gmail.com>

USER root

# Install libtinfo required by LLVM
RUN echo "deb http://us.archive.ubuntu.com/ubuntu/ trusty multiverse" >> /etc/apt/sources.list && \
    apt update && \
    apt install -y libtinfo-dev

ADD toolchain.cmake /usr/share/toolchain.cmake
ADD build_toolchain.sh build_toolchain.sh
RUN chmod +x build_toolchain.sh && ./build_toolchain.sh

ARG BOOST_VERSION
ENV BOOST_VERSION=$BOOST_VERSION
ADD install_boost.sh install_boost.sh
RUN chmod +x install_boost.sh && ./install_boost.sh

USER conan
WORKDIR /home/conan

ARG HOST_CC
ARG HOST_CXX

ENV CC=$HOST_CC
ENV CXX=$HOST_CXX

RUN $CC --version && $CXX --version
