#!/bin/bash

if [[ -z "$REPO" ]]; then
    REPO="manu343726"
fi
if [[ -z "$NAME" ]]; then
    NAME="tinyrefl"
fi

docker pull $REPO/$NAME:clang50-x86
docker pull $REPO/$NAME:gcc5-armv7
docker pull $REPO/$NAME:gcc5-x86
docker pull $REPO/$NAME:gcc6-armv7
docker pull $REPO/$NAME:gcc6-x86
docker pull $REPO/$NAME:gcc7-armv7
docker pull $REPO/$NAME:gcc7-x86
docker pull $REPO/$NAME:gcc8-armv7
docker pull $REPO/$NAME:gcc8-x86
