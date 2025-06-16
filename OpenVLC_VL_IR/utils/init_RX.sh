#!/bin/bash

cd /home/debian/OpenVLC-PA/OpenVLC_VL_IR/Driver

sudo chmod +x load_test.sh
sudo ./load_test.sh

cd /usr/share/ti/cgt-pru
sudo mkdir bin
cd bin
sudo ln -s /usr/bin/clpru clpru
sudo ln -s /usr/bin/lnkpru lnkpru

cd /home/debian/OpenVLC-PA/OpenVLC_VL_IR//PRU/RX

sudo chmod +x deploy.sh
sudo ./deploy.sh
