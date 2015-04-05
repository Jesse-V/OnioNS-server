OnioNS
========

OnioNS (alpha) - Tor-Powered Distributed DNS for Tor Hidden Services

OnioNS is a distributed DNS that allows hidden service operator to construct strong associations between a human-meaningful .tor domain name and their .onion hidden service address. The system is powered by the Tor network and relies on a distributed database. It also enables Tor clients to query against OnioNS resolvers in an easy and secure fashion.

### Installation

There are several methods to install the OnioNS software. The method of choice depends on your system. If you are on Ubuntu or an Ubuntu-based system (*Ubuntu, Mint) please use the PPA method. At the moment I only provide packaging for amd64 systems, so if you are on x86 or any other architecture, please use the source method. If you are running Debian or prefer not to use my PPA, please use the .deb method. Otherwise, for any other Linux system, please install from source.

Note that my code is written in C++11, so you have to update GCC or change "--std=c++11" to --std=c++0x" in src/CMakeLists.txt before compiling if you are on an older system such as Debian Wheezy or Ubuntu Precise.

I do not support Windows nor do I have plans of doing so. You probably shouldn't be running Tor infrastructure on Windows anyway. I am willing to provide upstream support to anyone who wishes to provide and maintain a port to OS-X.

* **Install from PPA**

> 1. **sudo add-apt-repository ppa:jvictors/tor-dev**
> 2. **sudo apt-get update**
> 3. **sudo apt-get install tor-onions**

This software can then be updated like any other Linux package, since my PPA is now in your sources.

* **Install from .deb file**

    Please see the [Releases section](https://github.com/Jesse-V/OnioNS/releases) at the top of this Github repo. If you prefer, you may also download them from [my PPA](https://launchpad.net/~jvictors/+archive/tor-dev/+packages). They are equivalent.

* **Install from source**

> 1. **sudo apt-get install botan1.10-dev g++ cmake make libseccomp-dev libevent-dev**
> 2. **./buildClient.sh**
> 3. **./buildServer.sh**

    I have included build scripts for Clang if you, as I do, prefer that compiler. Please let me know if you encounter any issues, I can update these instructions so that it's easier for everyone going forward.

### Usage

> 1. Download the Tor Brower from torproject.org
> 2. Open it up, and right-click on the Tor Button in the upper-left corner.
> 3. Select Preferences.
> 4. Click "Use Custom Proxy Settings"
> 5. Change port to 9050 and hit OK.
> 6. Close the Tor Browser.
> 7. In the Terminal, run the generated binary with **./tor-client**
> 8. Open the Tor Browser and just ignore the Tor warning.
> 9. Type "example.tor" into the URL field.
> 10. In a moment, you should arrive at a hidden service.
