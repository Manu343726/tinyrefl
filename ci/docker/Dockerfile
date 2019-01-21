ARG BASE_IMAGE
FROM $BASE_IMAGE
MAINTAINER Manu Sanchez <Manu343726@gmail.com>

USER root

# Install libtinfo required by LLVM
RUN echo "deb http://us.archive.ubuntu.com/ubuntu/ trusty multiverse" >> /etc/apt/sources.list && \
    apt update && \
    apt install -y libtinfo-dev libz-dev python3-pip && \
    apt clean

ADD toolchain.cmake /usr/share/toolchain.cmake
ADD build_toolchain.sh build_toolchain.sh
RUN chmod +x build_toolchain.sh && ./build_toolchain.sh

ARG BOOST_VERSION
ENV BOOST_VERSION=$BOOST_VERSION
ADD install_boost.sh install_boost.sh
RUN chmod +x install_boost.sh && ./install_boost.sh

RUN pip uninstall conan -y
RUN pip3 install conan

USER conan
WORKDIR /home/conan

ARG HOST_CC
ARG HOST_CXX
ARG CROSS_BUILDING

ENV CC=$HOST_CC
ENV CXX=$HOST_CXX
ENV CROSS_BUILDING=$CROSS_BUILDING
ENV CMAKE_TOOLCHAIN_FILE=/usr/share/toolchain.cmake

RUN echo CC=$CC && $CC --version
RUN echo CXX=$CXX && $CXX --version
RUN echo CROSS_BUILDING=$CROSS_BUILDING
RUN echo CMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN_FILE
