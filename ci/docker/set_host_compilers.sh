#!/bin/bash

if [[ "$CC" =~ "arm" ]] || [[ -z "$CC" ]]; then
    echo setting host C compiler to gcc
    export CC=gcc
fi

if [[ "$CXX" =~ "arm" ]] || [[ -z "$CXX" ]]; then
    echo setting host C++ compiler to g++
    export CXX=g++
fi

echo CC=$CC
echo CXX=$CXX

echo export CC=$CC >> /etc/profile
echo export CXX=$CXX >> /etc/profile
