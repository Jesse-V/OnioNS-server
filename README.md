OnioNS
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
> 3. **./buildServer.sh**

    I have included build scripts for Clang if you, as I do, prefer that compiler.

### Usage

> 1. Download the Tor Brower from torproject.org
> 2. Open it up, and right-click on the Tor Button in the upper-left corner.
> 3. Select Preferences.
> 4. Click "Use Custom Proxy Settings"
> 5. Change port to 9050 and hit OK.
> 6. Close the Tor Browser.
> 7. In the Terminal, run the generated binary with **./tor**
> 8. Open the Tor Browser and just ignore the Tor warning.
> 9. Type "example.tor" into the URL field.
> 10. In a moment, you should arrive at a hidden service.
