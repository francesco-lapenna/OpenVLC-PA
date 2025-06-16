#!/bin/bash

cd /home/debian/OpenVLC-PA/OpenVLC_VL_IR/Driver

sudo chmod +x load_test.sh
sudo ./load_test.sh
sudo ifconfig vlc0 192.168.0.1

cd /usr/share/ti/cgt-pru
sudo mkdir bin
cd bin
sudo ln -s /usr/bin/clpru clpru
sudo ln -s /usr/bin/lnkpru lnkpru

cd /home/debian/OpenVLC-PA/OpenVLC_VL_IR/PRU/TX_VL_IR_Dimming_0

sudo chmod +x deploy.sh
sudo ./deploy.sh
