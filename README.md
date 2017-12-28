# libcluon

| Linux & OSX Build (TravisCI) | Win64 Build (AppVeyor) | Test Coverage | Coverity Analysis | CII Best Practices |
| :--------------------------: | :--------------------: | :-----------: | :---------------: | :----------------: |
| [![Build Status](https://travis-ci.org/chrberger/libcluon.svg?branch=master)](https://travis-ci.org/chrberger/libcluon) | [![Build status](https://ci.appveyor.com/api/projects/status/n33il43mb6ot5422/branch/master?svg=true)](https://ci.appveyor.com/project/chrberger/libcluon/branch/master) | [![codecov](https://codecov.io/gh/chrberger/libcluon/branch/master/graph/badge.svg)](https://codecov.io/gh/chrberger/libcluon) | [![Coverity Scan](https://scan.coverity.com/projects/14014/badge.svg)]() | [![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/1479/badge)](https://bestpractices.coreinfrastructure.org/projects/1479) |

[![License](https://img.shields.io/badge/license-GPL--3-blue.svg)](https://raw.githubusercontent.com/chrberger/libcluon/master/LICENSE) [![API documentation](https://img.shields.io/badge/documentation-latest-blue.svg)](https://chrberger.github.io/libcluon/) [![Win (x86_64)](https://img.shields.io/badge/Win-x86__64%20(installer)-blue.svg
)](https://dl.bintray.com/chrberger/libcluon/) [![Ubuntu (x86_64)](https://img.shields.io/badge/deb-x86__64-blue.svg
)](https://launchpad.net/~chrberger/+archive/ubuntu/libcluon/+packages) [![Ubuntu (armhf)](https://img.shields.io/badge/deb-armhf-blue.svg
)](https://launchpad.net/~chrberger/+archive/ubuntu/libcluon/+packages) [![Alpine (x86_64)](https://img.shields.io/badge/Alpine-x86__64-blue.svg
)](https://github.com/chrberger/libcluon/blob/gh-pages/alpine/v3.7/x86_64/Dockerfile#L19) [![Alpine (armhf)](https://img.shields.io/badge/Alpine-armhf-blue.svg
)](https://github.com/chrberger/libcluon/blob/gh-pages/alpine/v3.7/armhf/Dockerfile#L25)

libcluon is a small and efficient library written in modern C++ library to _glue_ distributed software components together - in a _clever_ way - simply: cluon. Its name is inspired by gluon, an [elementary particle acting as exchange particle](https://en.wikipedia.org/wiki/Gluon).

## Table of Contents
* [Features](#features)
* [Dependencies](#dependencies)
* [Installation on Ubuntu 16.04 LTS](#installation-on-ubuntu-1604-lts)
* [Installation on Alpine 3.7](#installation-on-alpine-37)
* [Installation on Windows](#installation-on-windows)
* [Build from sources on the example of Ubuntu 16.04 LTS](#build-from-sources-on-the-example-of-ubuntu-1604-lts)
* [Tutorials](#tutorials--api-documentation)
* [Contributing](#contributing)
* [License](#license)


## Features
* Written in highly portable and high quality C++14
* Message compiler produces fully self-contained messages that do only depend on C++14 - external libraries are not needed allowing easy embedding into existing projects
* Native implementation of [Protobuf](https://developers.google.com/protocol-buffers/) for data serialization & deserialization: [Examples](https://github.com/chrberger/libcluon/blob/master/libcluon/testsuites/TestMyTestMessagesToProto.cpp)
* Native implementation of [LCM](http://lcm-proj.github.io/type_specification.html) for data serialization & deserialization: [Examples](https://github.com/chrberger/libcluon/blob/master/libcluon/testsuites/TestMyTestMessagesToLCM.cpp)
* Native implementation of JSON export for messages: [Examples](https://github.com/chrberger/libcluon/blob/master/libcluon/testsuites/TestJSONVisitor.cpp#L28)
* libcluon natively available for JavaScript via [Emscripten](https://github.com/kripken/emscripten): [libcluon.js](https://bintray.com/chrberger/libcluon/javascript#files)
* Portable implementation of publish/subscribe communication (Linux, MacOSX, Windows): [Example](https://github.com/chrberger/libcluon/blob/master/libcluon/testsuites/TestUDPReceiver.cpp#L111)
* Intermediate Data Representation (IDR) enables flexible message transformations at runtime; for example: Protobuf to JSON at runtime without generating any data structures beforehand: [Examples](https://github.com/chrberger/libcluon/blob/master/libcluon/testsuites/TestMyTestMessage1.cpp#L348)
* Message self-reflection to extract portable message specifications at runtime: [Examples](https://github.com/chrberger/libcluon/blob/master/libcluon/testsuites/TestODVDVisitor.cpp#L32)
* Message transformatiom into platform-independent CSV format: [Examples](https://github.com/chrberger/libcluon/blob/master/libcluon/testsuites/TestCSVVisitor.cpp#L28)


## Dependencies
All you need is a C++14-compliant compiler (we are testing with Clang 5.0, GCC 5.4, GCC 6.0, GCC 7.2, Xcode 9.1, and Visual Studio 14 (MSVC 19.0)) as the project ships the following dependencies as part of the source distribution:

* [Unit Test Framework Catch2](https://github.com/catchorg/Catch2/releases/tag/v2.0.1) - [![License: Boost Software License v1.0](https://img.shields.io/badge/License-Boost%20v1-blue.svg)](http://www.boost.org/LICENSE_1_0.txt) - [Source](https://github.com/chrberger/libcluon/tree/master/buildtools/xUnit)
* [kainjow/Mustache v3.1](https://github.com/kainjow/Mustache/releases/tag/v3.1) - [![License: Boost Software License v1.0](https://img.shields.io/badge/License-Boost%20v1-blue.svg)](http://www.boost.org/LICENSE_1_0.txt) - [Source](https://github.com/chrberger/libcluon/tree/master/libcluon/thirdparty/Mustache)
* [thelink2012/any](https://github.com/thelink2012/any) - [![License: Boost Software License v1.0](https://img.shields.io/badge/License-Boost%20v1-blue.svg)](http://www.boost.org/LICENSE_1_0.txt) - [Source](https://github.com/chrberger/libcluon/tree/master/libcluon/thirdparty/cluon/any)
* [adishavit/argh v1.2.0](https://github.com/adishavit/argh/releases/tag/v1.2.0) - [![License: BSD 3-Clause](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause) - [Source](https://github.com/chrberger/libcluon/tree/master/libcluon/thirdparty/argh)
* [yhirose/cpp-peglib](https://github.com/yhirose/cpp-peglib) - [![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT) - [Source](https://github.com/chrberger/libcluon/tree/master/libcluon/thirdparty/cpp-peglib)

## Installation
### Installation on Ubuntu 16.04 LTS
We are providing pre-compiled binaries for Ubuntu 16.04 LTS (Xenial Xerus) via Ubuntu's Launchpad for `amd64`, `i386`, `armfh`, and `arm64`; simply add the following PPA to your sources list:

```
sudo add-apt-repository ppa:chrberger/libcluon
```

Afterwards, update your package database and install `libcluon`:

```
sudo apt-get update
sudo apt-get install libcluon
```

### Installation on Alpine 3.7
We are providing pre-compiled binaries for Alpine 3.7 for `x86_64` and `armfh`; simply install the pre-compile `.apk` package as follows:

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
* [How to send data via a UDP socket](docs/cluon-UDPSender.md)
* [How to receive data via a UDP socket](docs/cluon-UDPReceiver.md)


## Contributing
We are happy to receive your PRs to accelerate libcluon's development; before contributing, please take a look at the [Contribution Documents](CONTRIBUTING.md).


## License

* This project is released under the terms of the GNU GPLv3 License - [![License: GPLv3](https://img.shields.io/badge/license-GPL--3-blue.svg
)](https://www.gnu.org/licenses/gpl-3.0.txt)
* The auto-generated code for libcluon.js is released under the terms of the MIT License - [![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
