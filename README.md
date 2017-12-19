# libcluon

| Linux & OSX Build (TravisCI) | Win64 Build (AppVeyor) | Test Coverage | Coverity Analysis | CII Best Practices |
| :--------------------------: | :--------------------: | :-----------: | :---------------: | :----------------: |
| [![Build Status](https://travis-ci.org/chrberger/libcluon.svg?branch=master)](https://travis-ci.org/chrberger/libcluon) | [![Build status](https://ci.appveyor.com/api/projects/status/n33il43mb6ot5422/branch/master?svg=true)](https://ci.appveyor.com/project/chrberger/libcluon/branch/master) | [![codecov](https://codecov.io/gh/chrberger/libcluon/branch/master/graph/badge.svg)](https://codecov.io/gh/chrberger/libcluon) | [![Coverity Scan](https://scan.coverity.com/projects/14014/badge.svg)]() | [![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/1479/badge)](https://bestpractices.coreinfrastructure.org/projects/1479) |

libcluon is a small and efficient library written in modern C++ library to _glue_ distributed software components together - in a _clever_ way - simply: cluon. Its name is inspired by gluon, an [elementary particle acting as exchange particle](https://en.wikipedia.org/wiki/Gluon).

## Installation on Ubuntu 16.04 LTS

We are providing pre-compiled binaries for Ubuntu 16.04 LTS (Xenial Xerus) via Ubuntu's Launchpad for amd64, i386, armfh, and arm64; simply add the following PPA to your sources list:

```
sudo add-apt-repository ppa:chrberger/libcluon
```

Afterwards, update your package database and install `libcluon`:

```
sudo apt-get update
sudo apt-get install libcluon
```

## Installation on Windows

We are providing pre-compiled binaries including debug symbols for Windows 64 via BinTray here: https://bintray.com/chrberger/libcluon/libcluon-win64-debug#files/

## Installation from sources on the example of Ubuntu 16.04 LTS

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