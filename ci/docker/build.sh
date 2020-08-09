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
    BOOST_VERSION="1.69.0"
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

build conanio/clang40     clang40-x86  clang  clang++
build conanio/clang50     clang50-x86  clang  clang++
build conanio/clang60     clang60-x86  clang  clang++
build conanio/clang7      clang70-x86  clang  clang++
build conanio/clang8      clang80-x86  clang  clang++
build conanio/clang9      clang90-x86  clang  clang++
build conanio/clang10     clang100-x86 clang  clang++
build conanio/gcc5-armv7  gcc5-armv7   gcc-5  g++-5 CROSS_BUILDING
build conanio/gcc5        gcc5-x86     gcc-5  g++-5
build conanio/gcc6-armv7  gcc6-armv7   gcc-6  g++-6 CROSS_BUILDING
build conanio/gcc6        gcc6-x86     gcc-6  g++-6
build conanio/gcc7-armv7  gcc7-armv7   gcc-7  g++-7 CROSS_BUILDING
build conanio/gcc7        gcc7-x86     gcc-7  g++-7
build conanio/gcc8-armv7  gcc8-armv7   gcc-8  g++-8 CROSS_BUILDING
build conanio/gcc8        gcc8-x86     gcc-8  g++-8
build conanio/gcc9-armv7  gcc9-armv7   gcc-9  g++-9 CROSS_BUILDING
build conanio/gcc9        gcc9-x86     gcc-9  g++-9
build conanio/gcc10-armv7 gcc10-armv7  gcc-10 g++-10 CROSS_BUILDING
build conanio/gcc10       gcc10-x86    gcc-10 g++-10

docker build ${DOCKER_DIR}/clang-format    -t manu343726/tinyrefl:clang-format    && docker push manu343726/tinyrefl:clang-format
docker build ${DOCKER_DIR}/clang-format-ci -t manu343726/tinyrefl:clang-format-ci && docker push manu343726/tinyrefl:clang-format-ci
