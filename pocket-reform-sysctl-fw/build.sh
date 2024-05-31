#!/bin/bash

export PICO_SDK_PATH=$PWD/../pico-sdk
export PICO_EXTRAS_PATH=$PWD/../pico-extras

mkdir -p build
cd build
cmake ..

make
