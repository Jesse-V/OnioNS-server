#OnioNS - the Onion Name Service
### Tor-Powered Distributed DNS for Tor Hidden Services

Onion Name System (OnioNS) is a privacy-enhanced, distributed, and highly usable DNS for Tor hidden services. It allows users to reference a hidden service by a meaningful globally-unique domain name chosen by the hidden service operator. The system is powered by the Tor network and relies on a distributed database. This project aims to address the major usability issue that has been with Tor hidden services since their introduction in 2002. The official project page is onions55e7yam27n.onion, which is example.tor under OnioNS.

### Supported Systems

#### Linux

** *Ubuntu 14.04+, Debian 8+, or Linux Mint 17+**

All architectures that can run Tor are supported. If you have an ARM board (Pi, BBB, Odroid, etc) feel free to test it.

#### Windows

Not currently supported at the moment, though the **client** functionality will land there eventually. I do not have any plans of porting nor supporting the **server** software on Windows; you probably shouldn't be running Tor relays on Windows anyway.

#### OS-X and *BSD

Not currently supported, support planned in the far future. I am willing to provide upstream support to anyone who wishes to port the client functionality over there. My current primary focus is developing the Linux edition.

### Installation

There are several methods to install the OnioNS software. The method of choice depends on your system. If you are on Ubuntu or an Ubuntu-based system (Lubuntu, Kubuntu, Mint) please use the PPA method. If you are running Debian or prefer not to use my PPA, please use the .deb method. Otherwise, for all other distributions, please install from source.

* **Install from PPA**

> 1. **sudo add-apt-repository ppa:jvictors/tor-dev**
> 2. **sudo apt-get update**
> 3. **sudo apt-get install tor-onions**

This is the recommended method as it's very easy to stay up-to-date with my releases.

* **Install from .deb file**

Please see the [Releases section](https://github.com/Jesse-V/OnioNS/releases) at the top of this Github repo. If you prefer, you may also download them from [my PPA](https://launchpad.net/~jvictors/+archive/tor-dev/+packages). They are equivalent.

* **Install from source**

> 1. **sudo apt-get install python-pip botan1.10-dev g++ cmake make libasio-dev libboost-system-dev**
> 2. **./build.sh**
> 3. **cd build/**
> 4. **sudo make install**

The ClangBuild.sh script is available if you prefer the Clang compiler. This script is recommended if you are developing or hacking OnioNS. You will need to run **sudo apt-get install clang-format-3.6** before running the script as the script will also re-style your code to the official development style, which is based on Chromium.

The code does not compile on Debian Wheezy but if you can figure out the procedures, please let me know. If you successfully compile OnioNS on any other Linux distribution, please describe the procedures in a Github ticket and I'll try to support it going forward.

### Usage

> 1. Open the Tor Browser.
> 2. Open two terminals, A and B.
> 3. In Terminal A, run **onions client -v**
> 4. In Terminal B, run **python client.py**
> 5. Type "example.tor" into the Tor Browser.
> 6. In a moment, you should arrive at a hidden service.
> 7. Close the Tor Browser.
> 8. Control-C the processes in both terminals.

### Registering a Domain Name

> 1. **onions hs --domain=mydomain.tor --hskey=/var/lib/tor-onions/example.key -v**
> 2. Wait for the computational work to complete.
> 3. Please send final JSON to me over IRC or email (see below).

### Bug Reporting

Please open a ticket on Github. If you do not have a Github account, please contact kernelcorn on #tor-dev on OFTC IRC, or email kernelcorn at riseup dot net. Please follow the same process for filing enhancement requests. I use PGP key 0xC20BEC80.
