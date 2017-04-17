## Wifi Setup for Raspberry Pi on first boot

If you want to connect to the Raspberry Pi through wifi on first boot, there are some initial steps that need to be done before you plug in the SD card into the Pi.

> Steps below have been tested on Raspbian. It might be different depending on distribution.

> All paths below are from the mount point of the SD card and all commands need to be run as `root`.

> All files need to edited with Linux line endings ('\n')

#### Steps:


1. With the SD card still mounted, open the default `.conf` file for wpa_supplicant. Usually, this is `/etc/wpa_supplicant/wpa_supplicant.conf`. Check your distribution's documentation for actual file name and location. On Raspbian, this can also be written in `/boot/wpa_supplicant.conf` and it will be appended to the actual file on boot up.

2. Add the following to the file:

    **For WPA-PSK setup (most home Wifi connections)**

        network={
            ssid="<SSID">
            psk="<PASSWORD>"
            key_mgmt=WPA-PSK
        }

    **For WPA-EAP setup (most enterprise/university connections)**

        network={
            ssid="<SSID>"
            scan_ssid=1
            key_mgmt=WPA-EAP
            eap=PEAP
            identity="<ID>"
            password="<PASSWORD>"
            phase1="peaplabel=0"
            phase2="auth=MSCHAPV2"
        }

3. If you do not wish have plain-text passwords, run `echo "<PASSWORD>" | iconv -t utf16le | openssl md4 | sed 's/(stdin)= /hash:/'` and copy the output as the password.
