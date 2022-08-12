#!/bin/bash

if [ $(id -u) != 0 ]; then
  echo "Must run as root"
  exit 1
fi
modprobe vcan
ip link add vcan0 type vcan
ip link set vcan0 up