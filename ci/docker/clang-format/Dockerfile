# Image based on https://hub.docker.com/r/unibeautify/clang-format
# but adds git so we can diff the clang-format result on CI

FROM debian:sid-slim
MAINTAINER Manu Sanchez <Manu343726@gmail.com>

ARG CLANG_FORMAT_VERSION_ARG=6.0
ENV CLANG_FORMAT_VERSION=$CLANG_FORMAT_VERSION_ARG

RUN apt-get update && \
apt-get install -y clang-format-${CLANG_FORMAT_VERSION} git python-pip && \
apt-get clean

RUN update-alternatives \
    --install /usr/bin/clang-format clang-format /usr/bin/clang-format-${CLANG_FORMAT_VERSION} 50

# switch to uid/gid identical to host uid/gid (forks/users), if not doing this, files that
# clang-format written will change user and group all to root.
USER 1000:100

ENTRYPOINT ["clang-format"]
CMD ["--help"]
