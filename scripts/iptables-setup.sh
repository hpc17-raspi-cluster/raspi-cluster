#!/usr/bin/env bash

if [[ $EUID > 0 ]] ; then
    echo "Please run as root"
    exit
fi

apt-get -y install iptables
install -v -dm 755 /etc/systemd/scripts    &&
install -v -m 644 etc/systemd/system/iptables.service \
                 /etc/systemd/system       &&
install -v -m 744 etc/systemd/scripts/iptables \
                 /etc/systemd/scripts      &&
systemctl enable iptables
