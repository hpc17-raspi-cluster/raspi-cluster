#!/usr/bin/env bash

if [[ $EUID > 0 ]] ; then
    echo "Please run as root"
    exit
fi

mkdir -pv /mnt/usb                  &&
mount -v 192.168.1.1:/home /mnt/usb &&
echo "192.168.1.1:/home /home nfs auto,nofail,x-systemd.automount,x-systemd.device-timeout=10,timeo=110 0 0" >> /etc/fstab
echo "Updated /etc/fstab to mount /home from network"
umount -v /mnt/usb
