#!/usr/bin/env bash

if [[ $EUID > 0 ]] ; then
    echo "Please run as root"
    exit
fi

echo "Moving /home to USB drive"
apt-get -y install rsync                      &&
rsync -avx /home/ /mnt/usb                    &&
mount /dev/sda1 /home                         &&
echo "Mounted USB as /home successfully"
echo "Updating fstab table"
UUID=$(blkid /dev/sda1 | sed  's/.*UUID="\(.*\)" TYPE.*/\1/') &&
echo "UUID=$UUID /home ext4 defaults 0 2" >> /etc/fstab       &&
echo "/etc/fstab updated. Please reboot and run mount to confirm"
