## Raspberry Pi cluster setup

### Required Hardware

* *n* x Raspberry Pis
* *n* x ethernet patch cables
* *n* x MicroSD cards with at least 8 GB size
* Power supply for the Pis and switches
* USB drive with at least 8 GB capacity
* *(Optional*) 1 or more ethernet switches with enough ports for the cluster
* *(Optional)* USB to ethernet connector

### Installing the OS:

* Download an image of the distribution you intend to run on the cluster.
* Copy it onto one SD card.
  Make sure that SSH is enabled. (On Raspbian, this is done by creating a `/boot/ssh` file in the SD card boot partition.)
  If you intend to connect via Wifi, make sure you enable Wifi on the Pi
  and add your conenction details to `.conf` file used by `wpa_supplicant` (usually `wpa_supplicant.conf`). See [Wifi setup at boot](./wifi-setup.md) for some hints.
* Insert the SD card into one of the Pi's, power it up and ssh into it.
* Setup your locale, timezone, and other settings as required.
* *(Recommended)* Switch to `systemd-networkd` for network management. The rest of this document assumes that `systemd-networkd` is being used. You can use the script in `scripts/switch-to-systemd-networkd.sh` on Raspbian Jessie or [execute the steps](./systemd-networkd-setup.md) manually. This will configure both ethernet (`eth0`) and wifi (`wlan0`) as DHCP initially. That will be changed later for the access node. If you choose not to use `systemd-networkd`, the network configuration and boot time service startup in the scripts will not work. You will have to manually do that using your init systems commands.
* *(Optional)* Install git and clone this repository using `git clone`

### Installing Open MPI:

* Download the latest stable source code for Open MPI from https://www.open-mpi.org/software/ompi/ or clone their Github repo with `git clone https://github.com/open-mpi/ompi`
* To install Open MPI with default settings, execute `scripts/open-mpi-install.sh` as `root` user on the Pi. It is a wrapper script that will ask about some installation options and might be helpful for those unfamiliar to installing from source on Linux. If you are comfortable installing from source code, you may do so directly using the standard `./configure && make && make install` method. Note that depending on your choice of distribution, you may need to install additional packages to get some extra features of Open MPI working. Some features that are not enabled by default on Open MPI are Valgrind support, Fortran support and experimental Java support.
* If you wish, you can also install the Python Open MPI library, `py4mpi`, to enable Open MPI support in Python.

<hr />

### Cloning the SD card

* Remove the SD card from the Pi and insert it back into your computer.
* Copy the contents of the SD card to a file either manually or by executing the script in `scripts/copy-sd-card.sh`.
* Clone the `.img` file onto the other SD cards.
* Insert the cards into the Pis and boot all the Pis up.
* Log into each Pi and give them unique hostnames using `sudo hostname <HOSTNAME>`.
* *(Optional)* Save image for future use to easily add more Pis to the cluster.

<hr />

### Setting up network for the cluster

For the cluster to be useful, there needs to be a network for the Pis to talk to each other and for OpenMPI communicators. Different scenarios of how this can be done are listed below.

#### Static IP setup for home clusters

If your cluster is going to stay in your home network or a network where you have access to the router settings, the easiest way to do this is to setup static IPs. Avoid doing this through `systemd-networkd` unless you are sure that there will be no IP conflicts.

* Log into your router home page, and search for the section which allows you to create the static IPs. You will need the MAC address of the Pis, which should be available in the router itself, or can be obtained by running `ip link show`. Note the IP address and hostname used for each Pi.
* Now log into each Pi and add all the IP address and hostnames to the `/etc/hosts` file.

You may skip to "Setting up a shared file system" now

#### Ethernet switch setups

These setups are more flexible but require some more configuration and installation than the static IP setup. They require an ethernet switch for the cluster to communicate and also require an access node that will act as DNS and DHCP server to the rest of the Pis. The possible ways to do this are:

1. Use Wifi for SSH access and Ethernet for cluster communication
2. Add a second ethernet port and use that for SSH access
3. Virtualize the ethernet port

For all three setups, `dnsmasq` needs to be installed on the access node:

* Install dnsmasq through the package manager. Since most package managers will automatically enable and start freshly installed services, disable it and stop it while it is setup.
* Copy the `etc/dnsmasq.conf` and `etc/resolv.dnsmasq.conf` script from this repo to `/etc` directory on the Pi. The configuration provided runs `dnsmasq` as a DHCP and DNS server on the `eth0` interface. Rename the interfaces in the `dnsmasq.conf` script if needed.
Also, it will use Google's public DNS servers for upstream DNS resolution. Change those in `resolv.dnsmasq.conf` if you wish.
* Copy the systemd unit files `etc/systemd/system/{dnsmasq.conf,fix-ethernet-issue}.service` to `/etc/systemd/system/` and the `scripts/fix-ethernet-issue.sh` script to `/usr/local/bin/`.
* Replace the systemd network file `/etc/systemd/network/10-ethernet.network` with the `etc/systemd/network/20-ethernet-dnsmasq.network` from this repo, or if you wish to keep the old file, just change `DHCP=no` within the file.

All these steps can be run on Raspbian through the `scripts/setup-dnsmasq.sh` script in this repo. This node will now provide DNS and DHCP server services to the rest of the cluster.

##### Using Wifi

Nothing more needs to be done here. The above steps have already setup the DHCP and DNS server for wifi. At this point, you can ssh directly into any of the Pis from your wifi, if you know their IP addresses. However, internet connection won't work on any node other than access node.

##### Using USB to ethernet dongle

Here, you need to create an additional systemd network file for the new ethernet interface, `eth1`.

* Copy the `etc/systemd/network/11-ethernet-eth1.network` file to `/etc/systemd/network/`

The script `scripts/eth1-setup.sh` will run the above step for you. At this point, you can ssh into the access node and then ssh into any of the Pis from there. Internet connection won't work any node other than access node.

##### Using virtualized ethernet setup

//TODO

<hr />

>It is strongly recommended that you reboot the Pi now and have a serial cable or some alternate means of interacting with it handy since networking may not work if something goes wrong.

You can now setup a static IP address for the access node using the MAC address of the interface that is connected to the outside world.

### Setting up internet access

The networking setup so far only allows the Pis to talk to each other. To setup internet access, packet forwarding needs to be setup on the access node.

* First, make sure `iptables` is installed.
* Enable IP forwarding by copying the file `etc/sysctl.d/30-ipforwarding.conf` in this repo to `/etc/sysctl.d/`. Execute `sysctl $(/etc/sysctl.d/30-ipforwarding.conf)` to start IP forwarding and check that it works using `sysctl -a | grep forward`.
* After that, create a directory called `/etc/systemd/scripts` on the access node.
* If you are using `eth1` instead of `wlan0` to access the Pi, run `sed -i s/wlan0/eth1/' etc/systemd/scripts/iptables`. Now, copy the `etc/systemd/scripts/iptables` file from this repo to the newly created directory. Make sure that the script can be executed by root.
* Copy the `etc/systemd/system/iptables.service` file to `/etc/systemd/system/` directory on the Pi to enable the rules at boot time using systemd.
* Enable the iptables service using `sudo systemctl enable iptables` and reboot the Pi.

The steps above can be run automatically using the `scripts/iptables-setup.sh` script in this repo.
After this, you should be able to access internet from any of the Pis in the cluster.

### Setting up a shared file system

For OpenMPI to work, there needs to a file system that is accessible from all nodes with the same path. In this step, we create a Network File System (NFS) by starting a NFS server on the access node and mounting on all other nodes.

Instead of creating a separate mount point for the shared system, we will use `/home` itself as our shared file system. This way, all users can access their settings on all nodes without any need for copying files. This is especially useful with SSH keys that are required by OpenMPI.  
You may choose not to share `/home` and share some other mount point. The rest of the document will note extra steps that need to be done or steps that can be skipped for such cases.

* First, create a temporary mount point on the access node with `mkdir -pv /mnt/usb` and mount the USB drive using `mount /dev/sda1 /mnt/usb`. Repalce `/dev/sda1` with the proper name for your drive. Run `scripts/mount-usb.sh` from this repo to automatically do this.
* Make sure that the drive format is either ntfs or ext4. Run `mount | grep sda1` and see that it says ntfs or ext4. If the file system is of type vfat, the USB will have to be formatted. Otherwise, you can skip to the sharing `/home` section.

##### Formatting the USB

> NOTE: This will erase all existing data from the USB drive. Backup any data the you wish to save.

* Unmount the USB with `umount /dev/sda1`.
* Create a new partition table on the drive with `parted /dev/sda mklabel gpt`
* Next, create at least one partition with `parted /dev/sda mkpart primary ext4 0% 100%`. If you wish, you can change the partition size and create more partitions of different file types.
* Format the partition using `mkfs -t ext4 /dev/sda1`.
* Remount the USB with `mount -v /dev/sda1 /mnt/usb`.

These commands have been automated in the `scripts/format-usb.sh` script.

##### Sharing `/home`

This consists of two parts, first moving the `/home` directory to a the USB dirve and second sharing it using NFS server. If you are not using `/home` as the shared file system, you can skip the first part. Make sure to change the commands in the second part accordingly.

###### Moving to a new home:

* To move `/home`, first install `rsync` on the access node.
* Then run `rsync -avx /home /mnt/usb` to replicate all existing files on the USB. This will fail if the partition type is not ntfs or ext4.
* Now mount this on top of the existing `/home` directory with `mount /dev/sda1 /home` Note that this doesn't actually delete the existing directories in `/home`, only masks them. You can still get back your old directories by unmounting.
* Get the UUID of the USB drive using `blkid /dev/sda1`. If you want to store the UUID in an variable, run `UUID=$(blkid /dev/sda1 | sed  's/.*UUID="\(.*\)" TYPE.*/\1/')`.
* Make this mount permanent using by adding the following line to `/etc/fstab`:
`<UUID> /home ext4 defaults 0 2`
* Reboot the Pi to make sure that all settings have been applied correctly. Run `df -h` and you should see `/dev/sda1` mounted on `/home`. If you want to delete your old `/home`, you can do so now, but it is not recommended.

These steps can be run automatically using `scripts/move-home.sh`.

###### Sharing the new home:

* Install `nfs-kernel-server` on the access node.
* Install two systemd unit files to fix a bug in Raspbian/Debian Jessie that prevents `rpcbind` and consequently `nfs-kernel-server` from starting on boot. These files are `etc/systemd/system/rpcbind.service` and `etc/systemd/system/nfs-common.service`. They have to be installed in `/etc/systemd/system/` directory on the access node.
* Add the below line to `/etc/exports` replacing `/home` with whatever directory you want to share:
`/home 192.168.1.0/255.255.255.0(rw,sync)`
* Enable the boot time startup of NFS server with `systemctl enable rpcbind nfs-common` and reboot.
* After rebooting, run `showmount -e` to make sure that the file system is exported.

These steps (except the reboot) can be run from `scripts/share-home.sh`

Now log into to one of the other Pis and test that this directory can be mounted correctly.

* Run `mkdir -pv /mnt/usb && mount -v 192.168.1.1:/home /mnt/usb`.
* If the mount works, make it permanent by adding the below line to `/etc/fstab`:
`192.168.1.1:/home /home nfs noauto,x-systemd.automount,x-systemd.device-timeout=10,timeo=14,x-systemd.idle-timeout=1min 0 0`
* Reboot the Pi to and run `df -h` to make sure the settings are working fine. You should see a line `192.168.1.1:/home` in the output.

Now, add the line to `/etc/fstab` on all other Pis except the access node. This can be automated by running the `scripts/nfs-mount-home.sh` script from this repo on the Pis.

### Creating a SSH key pair

To allow password-less access from one node to other, a SSH key pair is required. This is needed for OpenMPI programs to run across multiple nodes.

* Generate a SSH key pair by running `ssh-keygen -t rsa -b 2048 -C "<your-email-address"` on the access node. Do NOT enter a passphrase otherwise OpenMPI will not be able to use the keys.
* If your `/home` directory is NFS mounted, run `cat .ssh/id_rsa.pub >> .ssh/authorized_keys`.
* Otherwise, for each Pi, run `ssh <Pi-hostname-or-IP> mkdir -p .ssh` and then `cat .ssh/id_rsa.pub | ssh <Pi-hostname-or-IP> 'cat >> .ssh/authorized_keys'`

That's it for all the setup part! You can now use the cluster to run OpenMPI programs in this repository for testing. The cluster setup is generic enough to easily allow use of any other distributed memory framework as well.  
Adding new nodes to the cluster can also be done easily by copying the disk image from one of the non-access nodes to a new Pi and plugging it into the network switch. Just make sure that the hostname for the Pi does not conflict with existing hostnames.
