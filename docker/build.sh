#!/bin/bash

REPO="manu343726"
NAME="tinyrefl"

docker build --file docker/clang-50/Dockerfile --tag ${REPO}/${NAME}:clang50 .
docker build --file docker/gcc-5/Dockerfile --tag ${REPO}/${NAME}:gcc5 .
docker push ${REPO}/${NAME}:clang50
docker push ${REPO}/${NAME}:gcc5
