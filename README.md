#OnioNS - the Onion Name System
### A New Privacy-Enhanced DNS for Tor Hidden/Onion Services

OnioNS is a privacy-enhanced, metadata-free, and highly-usable DNS for Tor onion services. Administrators can use the Onion Name System to anonymously register a meaningful and globally-unique domain name for their site. Users can then load the site just by typing "example.tor" into the Tor Browser. OnioNS is backwards-compatible with traditional .onion addresses, does not require any modifications to the Tor binary or network, and there are no central authorities in charge of the domain names. This project was specifically engineered to solve the usability problem with onion services.

This is the software implementation of the system described in "The Onion Name System: Tor-Powered Decentralized DNS for Tor Onion Services", which will appear in the 2017.1 issue of the Proceedings on Privacy Enhancing Technologies (PoPETS). It will also be demoed in July 2017 at the 17th PETS Symposium in Minneapolis.

### Repository Details [![Build Status](https://travis-ci.org/Jesse-V/OnioNS-server.svg)](https://travis-ci.org/Jesse-V/OnioNS-server)

This software is intended for Tor relay operators who would like to contribute to the OnioNS network. Each OnioNS node, or mirror, maintains an up-to-date version of name records and other miscellaneous data. Qualified mirrors can be randomly selected to be in the Quorum, a group of mirrors that is "in charge" for a period of time. This repository implements the server-side domain resolution, name registration protocols, and server-server communication.

### Supported Systems

I am currently supporting Debian, Ubuntu, Mint, and Fedora. I provide builds for i386, amd64, and armhf. Please see the [OnioNS-common README](https://github.com/Jesse-V/OnioNS-common#supported-systems) for more information.

### Installation

* **Ubuntu/Mint? Install using Personal Package Archive (PPA)**

> 1. **sudo add-apt-repository ppa:jvictors/tor-dev**
> 2. **sudo apt-get update**
> 3. **sudo apt-get install tor-onions-server**

This is the recommended method as it will keep you up-to-date with my releases.

* **Debian? Install from .deb file**

You can find Debian packages in [Releases section](https://github.com/Jesse-V/OnioNS-server/releases). For other architectures, see [my PPA](https://launchpad.net/~jvictors/+archive/tor-dev/+packages).

* **Otherwise, install from source**

> 1. Install tor-onions-common by following [these instructions](https://github.com/Jesse-V/OnioNS-common#installation).
> 2. Download and extract the latest release from the [Releases page](https://github.com/Jesse-V/OnioNS-server/releases).
> 3. **(mkdir build; cd build; cmake ../src; make; sudo make install)**

You can cleanup your build with **rm -rf build**.

If you use Clang, you can encounter unusual linking errors depending on your GCC and Clang version. If so, try using the same compiler for both onions-common and onions-client to work around https://llvm.org/bugs/show_bug.cgi?id=23529.

You can cleanup your build with **rm -rf build**

### Setup

It is expected that you are maintaining an active Tor relay or exit. This software *will not* work if you are running a relay or bridge. If you are maintaining a relay, there are some necessary setup procedures to ensure proper inter-process communication between onion-server and the Tor binary.

1. First, enable communication to your server by adding the following to your Tor's configuration file (torrc), typically found at /etc/tor/torrc:

> * HiddenServiceDir OnioNS_Mirror
> * HiddenServicePort 10053 127.9.0.53:10053
> * ControlPort 9151
> * CookieAuthentication 1

2. Reload or restart Tor via **sudo systemctl reload tor** or equivalent command.

3. Grant onions-server access to the Tor authentication cookie and HS hostname via these commands:

> * **sudo cp -u /var/run/tor/control.authcookie ~/.OnioNS/control.authcookie**
> * **sudo cp -u /var/lib/tor/OnioNS_Mirror/hostname ~/.OnioNS/hostname**
> * **w=$(sudo echo `whoami`) && sudo chown $w:$w ~/.OnioNS/control.authcookie ~/.OnioNS/hostname**

You may have to run these commands again whenever you restart Tor.

4. Launch the onions-server executable. It can run under a normal user as root is not necessary. No additional incoming firewall rules are needed; if Tor can work correctly, then you're set.

> **onions-server --output server_info.log**

The --output flag is optional. Your server should now be online. No user interaction should be necessary, but you can keep an eye on the log in case anything goes wrong. A manpage is available for your convenience. You can also type **onions-server --help** for a list of flags and usage examples. Contact me on IRC or by email (see below) if you need further assistance.

### How to Contribute

I need more testers to verify that the software is stable and reliable. If you find an issue, please report it on Github. If you do not have a Github account, please contact kernelcorn on #tor-dev on OFTC IRC, or email kernelcorn at torproject dot org. Please follow the same process for filing enhancement requests.

Developers should use Clang 3.8 as it will compile faster and provide cleaner error messages. Feel check out the devBuild.sh and scanBuild.sh scripts as they can helpful to you. If you would like to contribute code, please fork this repo, sign your commits, and file a pull request. I develop using Clang 3.8 on Debian Testing amd64.

### Security Vulnerabilities

Usually, security vulnerabilities may be reported through the same communication channels as bug reports. However, if the impact is significant and you wish to report it privately, please contact me on IRC and I'll open a private conversation with you, or you can send me an email. I use PGP key 0xAD97364FC20BEC80. Please allow me time to respond, patch, and push out an update before reporting it publicly, which shouldn't take long.
