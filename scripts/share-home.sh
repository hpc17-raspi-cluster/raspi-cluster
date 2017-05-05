#!/usr/bin/env bash

if [[ $EUID > 0 ]] ; then
    echo "Please run as root"
    exit
fi

apt-get -y install nfs-kernel-server  &&
install -v -m 644 etc/systemd/system/{rpcbind,nfs-common}.service \
                 /etc/systemd/system/ &&
echo "/home 192.168.1.0/255.255.255.0(rw,sync)" >> /etc/exports &&
systemctl daemon-reload               &&
systemctl enable rpcbind nfs-common   &&
echo "NFS server configured...starting server now"
systemctl restart rpcbind nfs-common nfs-kernel-server
echo "It is recommended to reboot the Pi and test that services"
echo "are running as expected after reboot"
