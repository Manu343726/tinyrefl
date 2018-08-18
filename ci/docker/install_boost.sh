#!/bin/bash

set -e

if [[ -z "$BOOST_VERSION" ]]; then
    BOOST_VERSION="1.66.0"
fi

BOOST_VERSION_TAG="${BOOST_VERSION//./_}"
BOOST_PACKAGE="boost_${BOOST_VERSION_TAG}.tar.gz"
BOOST_SOURCES="boost_${BOOST_VERSION_TAG}"
BOOST_DOWNLOAD_URL="https://dl.bintray.com/boostorg/release/${BOOST_VERSION}/source/${BOOST_PACKAGE}"

echo Downloading and installing boost...
echo BOOST_VERSION=$BOOST_VERSION
echo BOOST_PACKAGE=$BOOST_PACKAGE
echo BOOST_DOWNLOAD_URL=$BOOST_DOWNLOAD_URL

wget --no-check-certificate --max-redirect 3 $BOOST_DOWNLOAD_URL
tar xfz $BOOST_PACKAGE
rm $BOOST_PACKAGE
cd $BOOST_SOURCES
./bootstrap.sh --prefix=/usr/local
./b2 install -j4
cd ..
rm -rf $BOOST_SOURCES
