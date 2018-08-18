#!/bin/bash

REPO="manu343726"
NAME="tinyrefl"
BOOST_VERSION="1.66.0"

build()
{
    BASE_IMAGE=$1
    TAG=$2
    HOST_CC=$3
    HOST_CXX=$4

    if [[ -z "$HOST_CC" ]]; then
        HOST_CC=gcc
    fi

    if [[ -z "$HOST_CXX" ]]; then
        HOST_CXX=g++
    fi

    HOST_CC_ARG="--build-arg HOST_CC=$HOST_CC"
    HOST_CXX_ARG="--build-arg HOST_CXX=$HOST_CXX"

    echo build $REPO/$NAME:$TAG HOST_CC=$HOST_CC HOST_CXX=$HOST_CXX
    docker build --build-arg BOOST_VERSION=$BOOST_VERSION --build-arg BASE_IMAGE=$BASE_IMAGE $HOST_CC_ARG $HOST_CXX_ARG --tag ${REPO}/${NAME}:${TAG} .
    docker push ${REPO}/${NAME}:${TAG}
}

build lasote/conanclang50 clang50-x86 clang clang++
build lasote/conangcc5-armv7 gcc5-armv7
build lasote/conangcc5 gcc5-x86
build lasote/conangcc6-armv7 gcc6-armv7
build lasote/conangcc6 gcc6-x86
build lasote/conangcc7-armv7 gcc7-armv7
build lasote/conangcc7 gcc7-x86
