ARG BASE_IMAGE
FROM $BASE_IMAGE
MAINTAINER Manu Sanchez <Manu343726@gmail.com>

USER root

# Install libtinfo required by LLVM
RUN apt update && \
    apt install -y libz-dev python3-pip libtinfo5 && \
    apt remove -y cmake && \
    apt clean

ARG CMAKE_VERSION_MAJOR=3
ARG CMAKE_VERSION_MINOR=11
ARG CMAKE_VERSION_FIX=4

ENV CMAKE_VERSION_MAJOR=$CMAKE_VERSION_MAJOR
ENV CMAKE_VERSION_MINOR=$CMAKE_VERSION_MINOR
ENV CMAKE_VERSION_FIX=$CMAKE_VERSION_FIX
ENV CMAKE_VERSION=$CMAKE_VERSION_MAJOR.$CMAKE_VERSION_MINOR.$CMAKE_VERSION_FIX
RUN wget -qO- "https://cmake.org/files/v$CMAKE_VERSION_MAJOR.$CMAKE_VERSION_MINOR/cmake-$CMAKE_VERSION-Linux-x86_64.tar.gz" | tar --strip-components=1 -xz -C /usr/local

ADD toolchain.cmake /usr/share/toolchain.cmake
ADD build_toolchain.sh build_toolchain.sh
RUN chmod +x build_toolchain.sh && ./build_toolchain.sh

ARG BOOST_VERSION
ENV BOOST_VERSION=$BOOST_VERSION
ADD install_boost.sh install_boost.sh
RUN chmod +x install_boost.sh && ./install_boost.sh

RUN pip uninstall conan -y
RUN CC=gcc CXX=g++ pip3 install conan

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
