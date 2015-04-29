#OnioNS - the Onion Name Service
### Tor-Powered Distributed DNS for Tor Hidden Services

The Onion Name Service (OnioNS) is a distributed DNS that allows hidden service operator to construct strong associations between a unique human-meaningful .tor domain name and their .onion hidden service address. The system is powered by the Tor network and relies on a distributed database. It also enables Tor clients to query against OnioNS resolvers in an easy and secure fashion.

### Supported Systems

#### Linux

**amd64 systems running *Ubuntu 14.04+, Debian 8+, or Linux Mint 17+**

The code does not compile on Debian Wheezy; I spent several hours attempting to get OnioNS to build on Wheezy and concluded that things were too old. If you can figure out the procedures, please let me know. If you successfully compile OnioNS on any other Linux distribution, send me the procedures and I'll try to support it going forward.

#### Windows

Not currently supported at the moment, though the **client** software will land there eventually. I do not have any plans of porting nor supporting the **server** software on Windows; you probably shouldn't be running Tor relays on Windows anyway.

#### OS-X and *BSD

Not currently supported, support planned in the far future. I am willing to provide upstream support to anyone who wishes to port the client functionality over there. My current primary focus is developing the Linux edition.

### Installation

There are several methods to install the OnioNS software. The method of choice depends on your system. If you are on Ubuntu or an Ubuntu-based system (*Ubuntu, Mint) please use the PPA method. If you are running Debian or prefer not to use my PPA, please use the .deb method. Otherwise, for all other distributions and any other architecture other than amd64, please install from source.

* **Install from PPA**

> 1. **sudo add-apt-repository ppa:jvictors/tor-dev**
> 2. **sudo apt-get update**
> 3. **sudo apt-get install tor-onions**

This is the recommended method as it's very easy to stay up-to-date with my releases. Note that I only currently provide amd64 builds on my PPA.

* **Install from .deb file**

Please see the [Releases section](https://github.com/Jesse-V/OnioNS/releases) at the top of this Github repo. If you prefer, you may also download them from [my PPA](https://launchpad.net/~jvictors/+archive/tor-dev/+packages). They are equivalent.

* **Install from source**

> 1. **sudo apt-get install botan1.10-dev g++ cmake make libseccomp-dev libevent-dev libasio-dev libboost-system-dev**
> 2. **./buildClient.sh**
> 3. **./buildServer.sh**
> 4. **sudo make install**

I have included build scripts for Clang if you, as I do, prefer that compiler. Please let me know if you encounter any issues, I can update these instructions so that it's easier for everyone going forward.

### Initialization

> 1. Download the Tor Brower from torproject.org
> 2. Open it up, and right-click on the Tor Button in the upper-left corner.
> 3. Select Preferences.
> 4. Click "Use Custom Proxy Settings"
> 5. Change port to 9050 and hit OK.
> 6. Close the Tor Browser.

### Usage

> 1. Open two terminals, A and B.
> 3. In Terminal A, run **./tor-client**
> 2. In Terminal B, run **./tor-onions client -v**
> 4. Open the Tor Browser and just ignore the Tor warning.
> 5. Type "example.tor" into the URL field.
> 6. In a moment, you should arrive at a hidden service.
> 7. Note the debug output in both Terminals.
