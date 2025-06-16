#!/bin/bash

echo "nameserver 8.8.8.8" | sudo tee /etc/resolv.conf

sudo dhclient -v usb0

ping -c 5 google.com