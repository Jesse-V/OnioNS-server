#OnioNS - the Onion Name System
### Tor-Powered Distributed DNS for Tor Hidden Services

OnioNS is a distributed, privacy-enhanced, metadata-free, and highly usable DNS for Tor hidden services. OnioNS allows hidden service operators to select a meaningful and globally-unique domain name for their service, which users can then reference from the Tor Browser. The system is powered by the Tor network, relies on a distributed database, and provides anonymity to both operators and users. This project aims to address the major usability issue that has been with Tor hidden services since their introduction in 2002.

### Repository Details [![Build Status](https://travis-ci.org/Jesse-V/OnioNS-server.svg)](https://travis-ci.org/Jesse-V/OnioNS-server)

This repository provides the networking infrastructure for OnioNS nodes.

This repository is a much older version of the software. The work continues on the json-rpc branch on all four repositories. The json-rpc branch contains a major but experimental network protocol that uses JSON-RPC-CPP as the underlying transport. That branch also contains the latest design specifications, as described in OnioNS-literature.

I will ask for volunteers when the software is ready to go. In the meantime, please feel to examine my latest commits for any security issues or other concerns.

Yes, I know the network is down. It's currently private because the code is moving around. Once the dust settles, the nodes will come back online.

### Supported Systems

**Debian 7 and 8, Ubuntu 14.04 - 15.10, Mint 17 - 17.2, Fedora 21 - 23**

Please see the [OnioNS-common README](https://github.com/Jesse-V/OnioNS-common#supported-systems) for more information.

### Installation

Please do not install the software from this branch as it is likely obsolete and will not be supported. See the json-rpc branch instead.
