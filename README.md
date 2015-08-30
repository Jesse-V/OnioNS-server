#OnioNS - the Onion Name System
### Tor-Powered Distributed DNS for Tor Hidden Services

OnioNS is a distributed, privacy-enhanced, metadata-free, and highly usable DNS for Tor hidden services. OnioNS allows hidden service operators to select a meaningful and globally-unique domain name for their service, which users can then reference from the Tor Browser. The system is powered by the Tor network, relies on a distributed database, and provides anonymity to both operators and users. This project aims to address the major usability issue that has been with Tor hidden services since their introduction in 2002.

### Repository Details [![Build Status](https://travis-ci.org/Jesse-V/OnioNS-server.svg)](https://travis-ci.org/Jesse-V/OnioNS-server)

This repository provides the networking infrastructure for OnioNS nodes.

### Supported Systems

**Debian 7 and 8, Ubuntu 14.04 - 15.10, Mint 17 - 17.2, Fedora 21 - 23**

Please see the [OnioNS-common README](https://github.com/Jesse-V/OnioNS-common#supported-systems) for more information.

### Installation

There are several methods to install the OnioNS software. The method of choice depends on your system. If you are on Ubuntu or an Ubuntu-based system (Lubuntu, Kubuntu, Mint) please use the PPA method. If you are running Debian Wheezy, please use the .deb method. Otherwise, for all other distributions, please install from source.

* **Install from PPA**

> 1. **sudo add-apt-repository ppa:jvictors/tor-dev**
> 2. **sudo apt-get update**
> 3. **sudo apt-get install tor-onions-server**

This is the recommended method as it's very easy to stay up-to-date with my releases.

* **Install from .deb file**

I provide builds for Debian Wheezy in the [Releases section](https://github.com/Jesse-V/OnioNS-server/releases) for several architectures. For other architectures, you may download from [my PPA](https://launchpad.net/~jvictors/+archive/tor-dev/+packages).

* **Install from source**

> 1. Install tor-onions-common by following [these instructions](https://github.com/Jesse-V/OnioNS-common#installation).
> 2. Download and extract the latest release from the [Releases page](https://github.com/Jesse-V/OnioNS-server/releases).
> 3. **(mkdir build; cd build; cmake ../src; make; sudo make install)**

The rebuild.sh script is available if you are actively developing OnioNS. You will need to install *clang-format-3.6* and *cppcheck* as the script also styles the code and performs a static analysis check.

You can cleanup your build with **rm -rf build**

### Administration

First, enable communication to your server by adding the following to your Tor's configuration file (torrc), typically found at /etc/tor/torrc:

> * HiddenServiceDir OnioNS_Mirror
> * HiddenServicePort 10053 127.53.53.53:10053

Reload or restart Tor, then get your server's address with:

> **cat /var/lib/tor/OnioNS_Mirror/hostname**

Send it to me if you want to contribute to the network. Otherwise, replace the address in /var/lib/tor-onions/mirrors.json with your value to allow your client to perform lookups against your server.

Launch the onions-server executable in the following way. Root is not necessary and the --output flag is optional. No additional incoming firewall rules are needed; if Tor can reach the Internet, you're set.

> **onions-server --output onions.log**

Your server should now be online. No user interaction should be necessary, but you can keep an eye on the log in case anything goes wrong.

### Getting Help

A manpage is available for your convenience. You can also type **onions-server --help** for a list of flags and usage examples. Contact me on IRC or by email (see below) if you need further assistance.

### Bug Reporting

Please open a ticket on Github. If you do not have a Github account, please contact kernelcorn on #tor-dev on OFTC IRC, or email kernelcorn at riseup dot net. Please follow the same process for filing enhancement requests. I use PGP key 0xC20BEC80. I accept pull requests if you want to contribute.

### Security Vulnerabilities

Usually, security vulnerabilities may be reported through the same communication channels as bug reports. However, if the impact is significant and you wish to report it privately, please contact me on IRC and I'll open a private conversation with you, or you can send me an email (PGP 0xC20BEC80). Please allow me time to respond, patch, and push out an update before reporting it publicly, which shouldn't take long.
