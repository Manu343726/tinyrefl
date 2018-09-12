#!/bin/bash

set -e

# Full path to the ci/docker/ directory, where this script is located
DOCKER_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"

if [[ -z "$REPO" ]]; then
    REPO="manu343726"
fi
if [[ -z "$NAME" ]]; then
    NAME="tinyrefl"
fi
if [[ -z "$BOOST_VERSION" ]]; then
    BOOST_VERSION="1.66.0"
fi

NO_CACHE=$1

build()
{
    BASE_IMAGE=$1
    TAG=$2
    HOST_CC=$3
    HOST_CXX=$4
    CROSS_BUILDING=$5

    if [[ -z "$HOST_CC" ]]; then
        HOST_CC=gcc
    fi

    if [[ -z "$HOST_CXX" ]]; then
        HOST_CXX=g++
    fi

    if [[ "$CROSS_BUILDING" =~ "CROSS_BUILD" ]]; then
        CROSS_BUILDING_VALUE=YES
    else
        CROSS_BUILDING_VALUE=NO
    fi

    HOST_CC_ARG="--build-arg HOST_CC=$HOST_CC"
    HOST_CXX_ARG="--build-arg HOST_CXX=$HOST_CXX"
    CROSS_BUILDING_ARG="--build-arg CROSS_BUILDING=$CROSS_BUILDING_VALUE"

    echo build $NO_CACHE $REPO/$NAME:$TAG HOST_CC=$HOST_CC HOST_CXX=$HOST_CXX CROSS_BUILDING=$CROSS_BUILDING_VALUE
    docker build $NO_CACHE --build-arg BOOST_VERSION=$BOOST_VERSION --build-arg BASE_IMAGE=$BASE_IMAGE $HOST_CC_ARG $HOST_CXX_ARG $CROSS_BUILDING_ARG --tag ${REPO}/${NAME}:${TAG} $DOCKER_DIR
    docker push ${REPO}/${NAME}:${TAG}
}

build lasote/conanclang40    clang40-x86 clang clang++
build lasote/conanclang50    clang50-x86 clang clang++
build lasote/conanclang60    clang60-x86 clang clang++
build lasote/conangcc5-armv7 gcc5-armv7  gcc   g++     CROSS_BUILDING
build lasote/conangcc5       gcc5-x86    gcc   g++
build lasote/conangcc6-armv7 gcc6-armv7  gcc   g++     CROSS_BUILDING
build lasote/conangcc6       gcc6-x86    gcc   g++
build lasote/conangcc7-armv7 gcc7-armv7  gcc   g++     CROSS_BUILDING
build lasote/conangcc7       gcc7-x86    gcc   g++
build lasote/conangcc8-armv7 gcc8-armv7  gcc   g++     CROSS_BUILDING
build lasote/conangcc8       gcc8-x86    gcc   g++
