#!/bin/bash

sudo iperf -c 192.168.0.2 -u -b 400k -l 800 -p 10001 -t 100
