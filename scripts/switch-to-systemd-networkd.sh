#!/usr/bin/env bash

if [[ $EUID > 0 ]]; then
    echo "Please run as root"
    exit
fi

systemctl disable NetworkManager  &&
systemctl enable systemd-networkd &&
systemctl enable systemd-resolved &&
systemctl start systemd-resolved  &&
rm -vf /etc/resolv.conf           &&
ln -vs /run/systemd/resolve/resolv.conf /etc/resolv.conf &&
install -v -d -m 755 /etc/systemd/network/ &&
install -v -m 644 etc/systemd/network/10-ethernet.network /etc/systemd/network/10-ethernet.network &&
install -v -m 644 etc/systemd/network/10-wireless.network /etc/systemd/network/10-wireless.network &&
echo "Successfully switched to systemd-networkd. Ethernet and wireless configured as DHCP" &&
echo "Network files are located in /etc/systemd/network/"
