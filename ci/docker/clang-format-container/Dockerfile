ARG BASE_IMAGE
FROM $BASE_IMAGE
MAINTAINER Manu Sanchez <Manu343726@gmail.com>

ARG CLANG_FORMAT_VERSION

RUN apt-get update && apt-get install -y --no-install-recommends \
  clang-format-$CLANG_FORMAT_VERSION \
  git \
  ca-certificates \
 && rm -rf /var/lib/apt/lists/*

RUN ln -s /usr/bin/clang-format-$CLANG_FORMAT_VERSION /usr/bin/clang-format
