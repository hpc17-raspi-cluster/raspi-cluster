#!/usr/bin/env bash

if [[ $EUID > 0 ]] ; then
    echo "Please run as root"
    exit
fi

umount -v /dev/sda1                           &&
parted /dev/sda mklabel gpt                   &&
parted /dev/sda mkpart primary ext4 0% 100%   &&
mkfs -t ext4 /dev/sda1                        &&
mkdir -pv /mnt/usb                            &&
mount /dev/sda1 /mnt/usb -o uid=root,gid=root &&
echo "USB drive mounted at /mnt/usb/ as root"
