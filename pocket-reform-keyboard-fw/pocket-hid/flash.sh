#!/bin/bash

sudo picotool load build/pocket-hid.uf2 --bus 1 -f
sleep 1
sudo picotool reboot --bus 1 -f

