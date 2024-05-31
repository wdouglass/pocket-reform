#!/bin/bash

sudo apt install build-essential pkg-config libusb-1.0-0-dev cmake gcc-arm-none-eabi tio

git clone --depth 1 https://github.com/raspberrypi/pico-sdk
git clone --depth 1 https://github.com/raspberrypi/pico-extras
git clone --depth 1 https://github.com/hathach/tinyusb

export PICO_SDK_PATH="$(pwd)/pico-sdk"
git clone --depth 1 https://github.com/raspberrypi/picotool
cd picotool
mkdir build
cd build
cmake ..
make -j4
sudo make install
