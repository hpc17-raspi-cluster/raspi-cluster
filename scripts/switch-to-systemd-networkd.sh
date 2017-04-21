#!/usr/bin/env bash

if [[ $EUID > 0 ]]; then
    echo "Please run as root"
    exit
fi

systemctl disable NetworkManager                               &&
systemctl disable dhcpcd5                                      &&
systemctl disable networking                                   &&
systemctl disable wpa_supplicant                               &&
systemctl enable systemd-networkd                              &&
systemctl enable systemd-resolved                              &&
install -v -d -m 755 /etc/systemd/network/                     &&
install -v -m 644 etc/systemd/network/10-ethernet.network \
                  /etc/systemd/network/10-ethernet.network     &&
install -v -m 644 etc/systemd/network/10-wireless.network \
                  /etc/systemd/network/10-wireless.network     &&
install -v -m 644 etc/systemd/system/wpa_supplicant@.service \
                  /etc/systemd/system/wpa_supplicant@.service  &&
systemctl daemon-reload                                        &&
systemctl enable wpa_supplicant@wlan0
echo "Successfully switched to systemd-networkd. Ethernet and wireless configured as DHCP" &&
echo "Network files are located in /etc/systemd/network/"
echo "systemd-networkd will be used on reboot"
