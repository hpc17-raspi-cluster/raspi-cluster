#!/usr/bin/env bash

if [[ -$EUID > 0 ]] ; then
    echo "Please run as root"
    exit
fi

install -v -m 644 etc/systemd/network/11-ethernet-eth1.network \
                 /etc/systemd/network/10-ethernet.network
