#!/usr/bin/env bash

if [[ $EUID > 0 ]] ; then
    echo "Please run as root"
    exit
fi

apt-get -y install dnsmasq                      &&
install -v -m 644 etc/resolv.dnsmasq.conf /etc/ &&
install -v -m 644 etc/dnsmasq.conf /etc/
echo "dnsmasq setup successfully"
echo "Checking for resolvconf settings and editing"
if [[ -f /etc/default/dnsmasq ]] ; then
    echo "IGNORE_RESOLVCONF=yes" >> /etc/default/resolv.conf
fi
echo "Configured to use public DNS servers from Google"
echo "for upstream DNS resolution"
echo "If you wish to to use your own DNS servers,"
echo "please edit /etc/resolv.dnsmasq.conf"
echo "Trying to start dnsmasq...in case of errors"
echo "please make sure that no other service is"
echo "overwriting /etc/resolv.conf"
systemctl restart dnsmasq
