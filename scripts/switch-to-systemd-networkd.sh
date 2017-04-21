#!/usr/bin/env bash

if [[ $EUID > 0 ]]; then
    echo "Please run as root"
    exit
fi

systemctl disable NetworkManager dhcpcd5 networking wpa_supplicant                 &&
install -v -d -m 755 /etc/systemd/network/                                         &&
install -v -m 644 etc/systemd/network/10-*.network           /etc/systemd/network/ &&
install -v -m 644 etc/systemd/system/wpa_supplicant@.service /etc/systemd/system/  &&
systemctl daemon-reload                                                            &&
systemctl enable systemd-network systemd-resolved wpa_supplicant@wlan0
echo "Successfully switched to systemd-networkd. Ethernet and wireless configured as DHCP"
echo "Network files are located in /etc/systemd/network/"
echo "systemd-networkd will be used on reboot"
