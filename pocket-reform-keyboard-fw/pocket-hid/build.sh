#!/bin/bash

export PICO_SDK_PATH=$(pwd)/../../pico-sdk

mkdir -p build
cd build
cmake -DFAMILY=rp2040 ..

make
