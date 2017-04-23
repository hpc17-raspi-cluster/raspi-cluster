#!/usr/bin/env bash

if [[ $EUID > 0 ]] ; then
    echo "Please run as root"
    exit
fi

mkdir -pv /mnt/usb
mount -v /dev/sda1 /mnt/usb
