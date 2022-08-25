#!/bin/bash

export PICO_SDK_PATH=$PWD/../../pico-sdk

mkdir -p build
cd build
cmake ..

make

