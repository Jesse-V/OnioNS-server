#OnioNS - the Onion Name Service
### Tor-Powered Distributed DNS for Tor Hidden Services

The Onion Name Service (OnioNS) is a distributed DNS that allows hidden service operator to construct strong associations between a human-meaningful .tor domain name and their .onion hidden service address. The system is powered by the Tor network and relies on a distributed database. It also enables Tor clients to query against OnioNS resolvers in an easy and secure fashion.

### Installation

There are several methods to install the OnioNS software. The method of choice depends on your system. If you are on Ubuntu or an Ubuntu-based system (*Ubuntu, Mint) please use the PPA method. If you are running Debian or prefer not to use my PPA, please use the .deb method. Otherwise, for all other distributions and any other architecture other than amd64, please install from source.

I do not support Windows at the moment, though the client edition will land there eventually. I do not have any plans of porting nor supporting the server edition on Windows; you probably shouldn't be running Tor relays on Windows anyway. I am willing to provide upstream support to anyone who wishes to port the client edition to OS-X. My current primary focus is developing the Linux edition.

* **Install from PPA**

> 1. **sudo add-apt-repository ppa:jvictors/tor-dev**
> 2. **sudo apt-get update**
> 3. **sudo apt-get install tor-onions**

This is the recommended method as it's very easy to stay up-to-date with my releases. Note that I only provide amd64 builds on my PPA currently.

* **Install from .deb file**

Please see the [Releases section](https://github.com/Jesse-V/OnioNS/releases) at the top of this Github repo. If you prefer, you may also download them from [my PPA](https://launchpad.net/~jvictors/+archive/tor-dev/+packages). They are equivalent.

* **Install from source**

> 1. **sudo apt-get install botan1.10-dev g++ cmake make libseccomp-dev libevent-dev libasio-dev**
> 2. **./buildClient.sh**
> 3. **./buildServer.sh**

I have included build scripts for Clang if you, as I do, prefer that compiler. Please let me know if you encounter any issues, I can update these instructions so that it's easier for everyone going forward.

If you are on an older system such as Debian Wheezy or Ubuntu Precise, note that my code is written in C++11, so you may have to update GCC or change "--std=c++11" to --std=c++0x" in src/CMakeLists.txt before compiling.

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
> 2. In Terminal B, run **./tor-onions --mode=client --verbose**
> 4. Open the Tor Browser and just ignore the Tor warning.
> 5. Type "example.tor" into the URL field.
> 6. In a moment, you should arrive at a hidden service.
> 7. Note the debug output in both Terminals.
