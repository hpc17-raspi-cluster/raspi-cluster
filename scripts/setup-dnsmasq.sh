#!/usr/bin/env bash

if [[ $EUID > 0 ]] ; then
    echo "Please run as root"
    exit
fi

apt-get -y install dnsmasq                             &&
systemctl disable dnsmasq                              &&
systemctl stop dnsmasq                                 &&
install -v -m 644 etc/systemd/system/dnsmasq.service \
                  /etc/systemd/system/dnsmasq.service  &&
install -v -m 644 etc/resolv.dnsmasq.conf /etc/        &&
install -v -m 644 etc/dnsmasq.conf /etc/
echo "dnsmasq setup successfully"
echo "Configured to use public DNS servers from Google for upstream"
echo "DNS resolution. If you wish to use different DNS servers, please"
echo "/etc/resolv.dnsmasq.conf"
echo "Trying to start dnsmasq...in case of errors, please make sure that"
echo "no other service is overwriting /etc/resolv.conf"
systemctl daemon-reload  &&
systemctl enable dnsmasq &&
systemctl start dnsmasq
