EsgalDNS
========

OnioNS (alpha) - Tor-Powered Distributed DNS for Anonymous Servers

### Installation

* **Install from PPA**

> 1. **sudo add-apt-repository ppa:jvictors/tor-dev**
> 2. **sudo apt-get update**
> 3. **sudo apt-get install tor-onions**

* **Install from .deb file**

    Coming soon!

* **Install from source**

> 1. **sudo apt-get install botan1.10-dev g++ cmake build-essential**
> 2. **./buildClient.sh**
> 2. **./buildServer.sh**
> 2. **mv build/OnioNS* .**
> 2. **mv tor-client/tor* .**

    I have included build scripts for Clang if you, as I do, prefer that compiler.

### Usage

> 1. Run the Tor client and point the Tor Browser at it.
> 2. Query for "example.tor"
