## Setting up systemd-networkd on Raspbian Jessie

`systemd-networkd` is the network manager provided by the systemd init system. While many Linux distributions have switched to systemd as the init systemd, Raspbian Jessie provides a mix of rc init scripts and systemd services. This is especially apparent in the networking subsystem, where the default installation uses the networking service, dhcpcd and systemd-networkd all at the same time. This leads to slower boot process and multiple IP addresses for network interfaces. There is no good reason for this and it is better to just use one network manager. The script `scripts/switch-to-systemd-networkd.sh` sets up `systemd-networkd` as the default network manager on Raspbian Jessie.

The steps taken are:

* Disable various network managers. These are done using `systemctl disable <service-name>`. The following services need to be disable if they are present: `NetworkManager`, `dhcpcd5`, `networking` and `wpa_supplicant`. Do NOT stop them yet, only disable them.
* Create a directory `/etc/systemd/network` if it does not exist.
* Copy the systemd network files `etc/systemd/network/10-ethernet.network` and `etc/systemd/network/10-wireless.network` in this repo to the `/etc/systemd/network/` directory just created. These files will configure `eth0` and `wlan0` interfaces to use DHCP.
* Also, copy the systemd unit file `etc/systemd/system/wpa_supplicant@.service` from this repo into `/etc/systemd/system/` directory. This service runs `wpa_supplicant` only on a given network interface as a dbus service as opposed to the system wide `wpa_supplicant` setup on all network interfaces.
* Load the new files into systemd daemon using `systemctl daemon-reload`.
* Now, enable the systemd services using `systemctl enable systemd-networkd systemd-resolved wpa_supplicant@wlan0`. Change the name of the wireless interface to match the one on your system.

If systemd version is older than 221, then, to enable static IP setup using `systemd-networkd` on Raspbian Jessie, a couple more files need to copied. These files fix a timing issue in systemd versions < 221 that prevent `systemd-networkd` from detecting `eth0` when configured as static IP. The fix just restarts `systemd-networkd` after network is setup.

* Copy `etc/systemd/system/fix-ethernet-issue.service` from this repo to `/etc/systemd/system/ directory` and the `scripts/fix-ethernet-issue.sh` script to `/usr/local/bin` directory. If you choose to copy the script to some other directory like `/usr` or `/opt`, edit the systemd unit file accordingly.
* Enable the unit using `systemctl enable fix-ethernet-issue`.

For more documentation on the network files, refer to `systemd.network` man page https://www.freedesktop.org/software/systemd/man/systemd.network.html