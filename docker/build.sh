#!/bin/bash

REPO="manu343726"
NAME="tinyrefl"

docker build --tag ${REPO}/${NAME}:clang50-x86 docker/clang50-x86
docker build --tag ${REPO}/${NAME}:gcc5-x86 docker/gcc-5-x86
docker build --tag ${REPO}/${NAME}:gcc6-x86 docker/gcc-6-x86
docker build --tag ${REPO}/${NAME}:gcc7-x86 docker/gcc-7-x86
docker build --tag ${REPO}/${NAME}:gcc5-armv7 docker/gcc-5-armv7
docker build --tag ${REPO}/${NAME}:gcc6-armv7 docker/gcc-6-armv7
docker build --tag ${REPO}/${NAME}:gcc7-armv7 docker/gcc-7-armv7
docker push ${REPO}/${NAME}:clang50-x86
docker push ${REPO}/${NAME}:gcc5-x86
docker push ${REPO}/${NAME}:gcc6-x86
docker push ${REPO}/${NAME}:gcc7-x86
docker push ${REPO}/${NAME}:gcc5-armv7
docker push ${REPO}/${NAME}:gcc6-armv7
docker push ${REPO}/${NAME}:gcc7-armv7
