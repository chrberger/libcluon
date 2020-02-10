# libcluon

| Linux & OSX Build (TravisCI) | Win64 Build (AppVeyor) | Test Coverage | Coverity Analysis | CII Best Practices |
| :--------------------------: | :--------------------: | :-----------: | :---------------: | :----------------: |
| [![Build Status](https://travis-ci.org/chrberger/libcluon.svg?branch=master)](https://travis-ci.org/chrberger/libcluon) | [![Build status](https://ci.appveyor.com/api/projects/status/n33il43mb6ot5422/branch/master?svg=true)](https://ci.appveyor.com/project/chrberger/libcluon/branch/master) | [![codecov](https://codecov.io/gh/chrberger/libcluon/branch/master/graph/badge.svg)](https://codecov.io/gh/chrberger/libcluon) | [![Coverity Scan](https://scan.coverity.com/projects/14014/badge.svg)]() | [![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/1479/badge)](https://bestpractices.coreinfrastructure.org/projects/1479) |

[![License](https://img.shields.io/badge/license-MPL--2-blue.svg)](https://raw.githubusercontent.com/chrberger/libcluon/master/LICENSE) [![API documentation](https://img.shields.io/badge/documentation-latest-blue.svg)](https://chrberger.github.io/libcluon/) [![Win (x86_64)](https://img.shields.io/badge/Win-x86__64%20(installer)-blue.svg
)](https://dl.bintray.com/chrberger/libcluon/) [![Ubuntu (x86_64)](https://img.shields.io/badge/deb-x86__64-blue.svg
)](https://launchpad.net/~chrberger/+archive/ubuntu/libcluon/+packages) [![Ubuntu (armhf)](https://img.shields.io/badge/deb-armhf-blue.svg
)](https://launchpad.net/~chrberger/+archive/ubuntu/libcluon/+packages) [![Ubuntu (aarch64)](https://img.shields.io/badge/deb-aarch64-blue.svg
)](https://launchpad.net/~chrberger/+archive/ubuntu/libcluon/+packages) [![Ubuntu (s390x)](https://img.shields.io/badge/deb-s390x-blue.svg
)](https://launchpad.net/~chrberger/+archive/ubuntu/libcluon/+packages) [![Ubuntu (powerpc)](https://img.shields.io/badge/deb-powerpc-blue.svg
)](https://launchpad.net/~chrberger/+archive/ubuntu/libcluon/+packages) [![Alpine (x86_64)](https://img.shields.io/badge/Alpine-x86__64-blue.svg
)](https://github.com/chrberger/libcluon/blob/gh-pages/alpine/v3.7/x86_64/Dockerfile#L19) [![Alpine (armhf)](https://img.shields.io/badge/Alpine-armhf-blue.svg
)](https://github.com/chrberger/libcluon/blob/gh-pages/alpine/v3.7/armhf/Dockerfile#L25) [![Alpine (aarch64)](https://img.shields.io/badge/Alpine-aarch64-blue.svg
)](https://github.com/chrberger/libcluon/blob/gh-pages/alpine/v3.7/aarch64/Dockerfile#L25)

libcluon is a small single-file, header-only library written in modern C++ library to _glue_ microservices - in a _clever_ way - simply: cluon. Its name is inspired by gluon, an [elementary particle acting as exchange particle](https://en.wikipedia.org/wiki/Gluon).

libcluon is distributed as single-file, [header-only](https://github.com/chrberger/libcluon/tree/gh-pages/headeronly) library - just drop [cluon-complete.hpp](https://chrberger.github.io/libcluon/headeronly/cluon-complete.hpp) into your project, `#include "cluon-complete.hpp"`, and compile your project with a modern C++ compiler (C++14 or newer)

_Say you want to quickly realize a distributed software system where individual software components exchange messages and you want to keep your project as *simple* and *clean* as possible - that's a typical use-case for libcluon._ [Getting Started Tutorial using an online C++ compiler](https://wandbox.org/permlink/3S1bSOaLakXfdWWZ).


## Table of Contents
* [Features](#features)
* [Dependencies](#dependencies)
* [Installation on Ubuntu 18.04 LTS](#installation-on-ubuntu-1804-lts)
* [Installation on Debian](#installation-on-debian)
* [Installation on Alpine 3.7](#installation-on-alpine-37)
* [Installation on Windows](#installation-on-windows)
* [Build from sources on the example of Ubuntu 16.04 LTS](#build-from-sources-on-the-example-of-ubuntu-1604-lts)
* [Tutorials](#tutorials--api-documentation)
* [Contributing](#contributing)
* [License](#license)


## Features
* Written in highly portable and high quality C++14
* **Available as [header-only](https://github.com/chrberger/libcluon/tree/gh-pages/headeronly), single-file distribution - just drop [cluon-complete.hpp](https://chrberger.github.io/libcluon/headeronly/cluon-complete.hpp) into your project, `#include "cluon-complete.hpp"`, and compile your project with a modern C++ compiler (C++14 or newer)**
* Message compiler produces fully self-contained messages that do only depend on C++14 - external libraries are not needed allowing easy embedding into existing projects
* Native implementation of [Protobuf](https://developers.google.com/protocol-buffers/) for data serialization & deserialization: [Example](https://wandbox.org/permlink/rXayIZxXyVDt5Jgn)
* Native implementation of [LCM](http://lcm-proj.github.io/type_specification.html)/[ZCM](http://zerocm.github.io/zcm/) for data serialization & deserialization: [Example](https://wandbox.org/permlink/ItJnZ3VVnZsqL28y)
* Native implementation of [JSON](https://www.json.org/) for data serialization & deserialization: [Example](https://wandbox.org/permlink/qyvkWIzGZFiBnTvm)
* Native implementation of [MsgPack](https://msgpack.org/) for data serialization & deserialization: [Example](https://wandbox.org/permlink/qFNxlA8zwZj2if0P)
* libcluon natively available for JavaScript via [Emscripten](https://github.com/kripken/emscripten): [libcluon.js](https://bintray.com/chrberger/libcluon/javascript#files)
* Portable implementation of publish/subscribe communication (Linux, MacOSX, Windows): [Example](https://github.com/chrberger/libcluon/blob/master/libcluon/testsuites/TestUDPReceiver.cpp#L111)
* Intermediate Data Representation (IDR) enables flexible message transformations at runtime; for example: Convert [Protobuf to JSON](https://github.com/chrberger/libcluon/blob/master/libcluon/testsuites/TestMyTestMessage1.cpp#L348) or convert [OD4 to JSON](https://github.com/chrberger/libcluon/blob/master/libcluon/tools/cluon-OD4toJSON.cpp) or convert [LCM to JSON](https://github.com/chrberger/libcluon/blob/master/libcluon/tools/cluon-LCMtoJSON.cpp) at runtime without generating any data structures beforehand
* Message self-reflection to extract portable message specifications at runtime: [Examples](https://github.com/chrberger/libcluon/blob/master/libcluon/testsuites/TestToODVDVisitor.cpp#L32)
* Message transformatiom into platform-independent CSV format: [Examples](https://github.com/chrberger/libcluon/blob/master/libcluon/testsuites/TestToCSVVisitor.cpp#L28)


## Dependencies
No dependencies! All you need is a C++14-compliant compiler as the project ships the following dependencies as part of the source distribution:

* [Unit Test Framework Catch2 v2.11](https://github.com/catchorg/Catch2/releases/tag/v2.11) - [![License: Boost Software License v1.0](https://img.shields.io/badge/License-Boost%20v1-blue.svg)](http://www.boost.org/LICENSE_1_0.txt) - [Source](https://github.com/chrberger/libcluon/tree/master/buildtools/xUnit)
* [adishavit/argh v1.3.1](https://github.com/adishavit/argh/releases/tag/v1.3.1) - [![License: BSD 3-Clause](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause) - [Source](https://github.com/chrberger/libcluon/tree/master/libcluon/thirdparty/argh)
* [chrberger/stringtoolbox](https://github.com/chrberger/stringtoolbox) - [![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT) - [Source](https://github.com/chrberger/libcluon/tree/master/libcluon/thirdparty/cluon/stringtoolbox.hpp)
* [kainjow/Mustache v4.0](https://github.com/kainjow/Mustache/releases/tag/v4.0) - [![License: Boost Software License v1.0](https://img.shields.io/badge/License-Boost%20v1-blue.svg)](http://www.boost.org/LICENSE_1_0.txt) - [Source](https://github.com/chrberger/libcluon/tree/master/libcluon/thirdparty/Mustache)
* [thelink2012/any](https://github.com/thelink2012/any) - [![License: Boost Software License v1.0](https://img.shields.io/badge/License-Boost%20v1-blue.svg)](http://www.boost.org/LICENSE_1_0.txt) - [Source](https://github.com/chrberger/libcluon/tree/master/libcluon/thirdparty/cluon/any)
* [yhirose/cpp-peglib v0.1.2](https://github.com/yhirose/cpp-peglib/releases/tag/v0.1.2) - [![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT) - [Source](https://github.com/chrberger/libcluon/tree/master/libcluon/thirdparty/cpp-peglib)

As part of our CI strategy thanks to TravisCI and AppVeyor, we are continuously building with:

* Darwin 16.7.0 (x86_64)/AppleClang 9.0.0.9000038

* FreeBSD 11.1 (x86_64)/GCC 6.4.0
* NetBSD 8.0 (x86_64)/GCC 5.5.0
* OpenBSD 6.3 (x86_64)/clang 5.0.1

* Windows (x86_64)/MSVC 19.13.26129.0

* Ubuntu 18.04 LTS (x86_64)/GCC 7.4.0
* Ubuntu 18.04 LTS (x86_64)/clang 7.0.0

* Ubuntu 16.04 LTS (x86_64)/GCC 5.4.0
* Ubuntu 16.04 LTS (x86_64)/clang 7.0.0

* Ubuntu 14.04 LTS (armhf)/clang 3.8.1

* Ubuntu 14.04 LTS (x86_64)/GCC 8.0.1
* Ubuntu 14.04 LTS (x86_64)/GCC 7.3.0
* Ubuntu 14.04 LTS (x86_64)/GCC 6.4.0
* Ubuntu 14.04 LTS (x86_64)/GCC 5.5.0

* Ubuntu 14.04 LTS (x86_64)/clang 6.0.1
* Ubuntu 14.04 LTS (x86_64)/clang 5.0.2
* Ubuntu 14.04 LTS (x86_64)/clang 5.0.0
* Ubuntu 14.04 LTS (x86_64)/clang 4.0.1
* Ubuntu 14.04 LTS (x86_64)/clang 3.9.1
* Ubuntu 14.04 LTS (x86_64)/clang 3.8.0


## Installation
### Installation as single-file, header-only library
`libcluon` is provided as [header-only](https://github.com/chrberger/libcluon/tree/gh-pages/headeronly), single-file library as well - just drop [cluon-complete.hpp](https://chrberger.github.io/libcluon/headeronly/cluon-complete.hpp) into your project, `#include "cluon-complete.hpp"` where you want to use libcluon, and compile your project with a modern C++ compiler (C++14 or newer).

### Installation on Ubuntu 18.04 LTS
We are providing pre-compiled binaries for Ubuntu 18.04 LTS (Bionic Beaver) via Ubuntu's Launchpad for `amd64`, `i386`, `armfh`, and `arm64`; simply add the following PPA to your sources list:

```
sudo add-apt-repository ppa:chrberger/libcluon
```

Afterwards, update your package database and install `libcluon`:

```
sudo apt-get update
sudo apt-get install libcluon
```

### Installation on Debian
To use `libcluon` on Debian, you need to add the repository key first:

```
sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 8EA63C9470BA0E595B75BBA2A92E492AC0B8C7EC
```

Afterwards, install `add-apt-repository` and its dependencies:

```
sudo apt install dirmngr software-properties-common
```

Then, you can add the `libcluon` repository:
```
sudo add-apt-repository ppa:chrberger/libcluon
```

Now, you can finally install `libcluon`:
```
sudo apt update && sudo apt install libcluon
```

### Installation on Alpine 3.7
We are providing pre-compiled binaries for Alpine 3.7 for `x86_64`, `armfh`, and `aarch64`; simply install the pre-compile `.apk` package as follows:

```
apk add libcluon --no-cache --repository https://chrberger.github.io/libcluon/alpine/v3.7 --allow-untrusted
```

### Installation on Windows
We are providing pre-compiled binaries including debug symbols for Windows 64 via BinTray here: https://bintray.com/chrberger/libcluon/libcluon-win64-debug#files/


## Build from sources on the example of Ubuntu 16.04 LTS
To compile `libcluon` from sources on an Ubuntu 16.04 LTS (Xenial Xerus) system, you need to have `build-essential`, `cmake`, and `git` installed:

```
sudo apt-get install build-essential git cmake
```

Afterwards, simply clone our Git repository:

```
git clone https://github.com/chrberger/libcluon.git
```

As an alternative, you can download our latest source release from here: https://github.com/chrberger/libcluon/releases/latest

Change to your working copy and create a build folder:

```
cd libcluon
mkdir build
cd build
```

Next, run `cmake` to create the necessary build files:

```
cmake ../libcluon
```

Finally, compile and install the software:

```
make
make test
make install
```

## Tutorials & API Documentation
* [API Documentation](https://chrberger.github.io/libcluon/)
* [Tutorial: "Getting Started" explaining how to exchange messages between distributed applications (using an online C++ compiler)](https://wandbox.org/permlink/3S1bSOaLakXfdWWZ)
* [Tutorial: Sending & receiving data via UDP (using an online C++ compiler)](https://wandbox.org/permlink/3bWexUDo6OqxDCBE)
* [Tutorial: Creating a TCP server to send & receive data (using an online C++ compiler)](https://wandbox.org/permlink/ZumnWxgXN9QUNHmD)
* [Tutorial: Exchanging data via shared memory with wait & notify (using an online C++ compiler)](https://wandbox.org/permlink/m7bmtpOS0dynGJlP)
* [Tutorial: Timestamping (using an online C++ compiler)](https://wandbox.org/permlink/aigGxE7xjU09X3qW)
* [Tutorial: Encoding & decoding using libcluon's native Protobuf (using an online C++ compiler)](https://wandbox.org/permlink/rXayIZxXyVDt5Jgn)
* [Tutorial: Encoding & decoding using libcluon's native LCM (using an online C++ compiler)](https://wandbox.org/permlink/ItJnZ3VVnZsqL28y)
* [Tutorial: Encoding & decoding using libcluon's native MsgPack (using an online C++ compiler)](https://wandbox.org/permlink/qFNxlA8zwZj2if0P)
* [Tutorial: Encoding using libcluon's native JSON (using an online C++ compiler)](https://wandbox.org/permlink/qyvkWIzGZFiBnTvm)
* [Tutorial: Encoding using libcluon's native CSV (using an online C++ compiler)](https://wandbox.org/permlink/APD3G7An9Ebh4eio)
* [Tutorial: Encoding & decoding with dynamic message specifications using libcluon's native Protobuf (using an online C++ compiler)](https://wandbox.org/permlink/uR2NZwnYZkTwmDac)
* [Tutorial: Turn a JSON string into Protobuf with dynamic message specifications (using an online C++ compiler)](https://wandbox.org/permlink/2SKYkHnVrNwoSJfK)
* [Tutorial: Using libcluon to realize an application communicating between JavaScript and C++](https://github.com/chrberger/cluon-javascript)


## Contributing
We are happy to receive your PRs to accelerate libcluon's development; before contributing, please take a look at the [Contribution Documents](CONTRIBUTING.md).


## License
* This project is released under the terms of the Mozilla Public License 2.0 - [![License: MPL-2.0](https://img.shields.io/badge/license-MPL--2-blue.svg)](http://mozilla.org/MPL/2.0/) - [FAQ](https://www.mozilla.org/en-US/MPL/2.0/FAQ/)
* Commercial support is available at [libcluon@christianberger.net](mailto:libcluon@christianberger.net)
