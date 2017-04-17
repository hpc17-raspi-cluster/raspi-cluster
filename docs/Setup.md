## Raspberry Pi cluster setup

### Required Hardware

* *n* x Raspberry Pis
* *n* x ethernet patch cables
* *n* x MicroSD cards with at least 8 GB size
* Power supply for the Pis and switches
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
* *(Optional)* Install git and clone this repository using `git clone`

### Installing Open MPI:

* Download the latest stable source code for Open MPI from https://www.open-mpi.org/software/ompi/ or clone their Github repo with `git clone https://github.com/open-mpi/ompi`
* To install Open MPI with default settings, execute `scripts/open-mpi-install.sh` as `root` user on the Pi. It is a wrapper script that will ask about some installation options and might be helpful for those unfamiliar to installing from source on Linux. If you are comfortable installing from source code, you may do so directly using the standard `./configure && make && make install` method. Note that depending on your choice of distribution, you may need to install additional packages to get some extra features of Open MPI working. Some features that are not enabled by default on Open MPI are Valgrind support, Fortran support and experimental Java support.
* If you wish, you can also install the Python Open MPI library, `py4mpi`, to enable Open MPI support in Python.

### Cloning the SD card

* Remove the SD card from the Pi and insert it back into your computer.
* Copy the contents of the SD card to a file either manually or by executing the script in `scripts/copy-sd-card.sh`.
* Clone the `.img` file onto the other SD cards.
* Insert the cards into the Pis and boot all the Pis up.
* Log into each Pi and give them unique hostnames using `sudo hostname <HOSTNAME>`.
* *(Optional)* Save image for future use.

### Setting up networking on access node

One or more of the Pis need to work as an access node for the cluster. Different scenarios of how this can be done are listed below.

#### Static IP setup for home clusters

If your cluster is going to stay in your home network or a network where you have access to the router settings, the easiest way to do this is to setup static IPs for the Pis. Avoid doing this through the OS unless you are sure that there will be no IP conflicts.  
Log into your router home page, and search for the section which allows you to create the static IPs. You will probably need the MAC address of the Pis, which should be available in the router itself, or can be obtained by running `ip link show`.

You may skip to "Setting up a shared file system" now

#### Ethernet switch setups

These setups are more flexible but require some more configuration and installation than the static IP setup. They require an ethernet switch for the cluster. The possible setups are:

1. Use Wifi for SSH access and Ethernet for cluster communication
2. Add a second ethernet port and use that for SSH access
3. Virtualize the ethernet port

##### Using Wifi

This is probably the easiest setup since the hardware is already ready and only the software needs to be installed. You will need to have the hostnames and the MAC addresses of your Pis ethernet ports available.

* Set your ethernet to static IP address. How to do this usually varies by OS and you will have to refer to your OS's help manual for this.
* 