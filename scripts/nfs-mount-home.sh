#!/usr/bin/env bash

if [[ $EUID > 0 ]] ; then
    echo "Please run as root"
    exit
fi

mkdir -pv /mnt/usb                  &&
mount -v 192.168.1.1:/home /mnt/usb &&
echo "192.168.1.1:/home /home nfs noauto,x-systemd.automount,x-systemd.device-timeout=10,timeo=14,x-systemd.idle-timeout=1min 0 0" >> /etc/fstab
echo "Updated /etc/fstab to mount /home from network"
umount -v /mnt/usb
