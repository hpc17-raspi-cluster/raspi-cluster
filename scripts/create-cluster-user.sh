#!/usr/bin/env bash

groups='adm,dialout,cdrom,audio,video,plugdev,games,users,input,netdev,gpio,i2c,spi,sudo'

sudo useradd -U -G $groups -s /bin/bash -m $1 &&
    echo "$1:hpc17cluster" | sudo chpasswd
sudo chage -d 0 $1
parallel-ssh -i -H "rpi1 rpi2 rpi3" \
        "sudo useradd -U -G $groups -s /bin/bash -M $1 ; echo $1:raspberry | sudo chpasswd ; sudo chage -d 0 $1"
