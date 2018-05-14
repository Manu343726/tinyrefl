FROM lasote/conangcc7
MAINTAINER Manu Sanchez <Manu343726@gmail.com>

USER root

# Install libtinfo required by LLVM
RUN echo "deb http://us.archive.ubuntu.com/ubuntu/ trusty multiverse" >> /etc/apt/sources.list && \
    apt update && \
    apt install -y libtinfo-dev

USER conan
WORKDIR /home/conan
