# Copyright (C) 2017  Christian Berger
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

FROM ubuntu:18.04
MAINTAINER Christian Berger "christian.berger@gu.se"

# Set the env variable DEBIAN_FRONTEND to noninteractive
ENV DEBIAN_FRONTEND noninteractive

ADD ./01_nodoc /etc/dpkg/dpkg.cfg.d/01_nodoc
ADD ./02_nolocales /etc/dpkg/dpkg.cfg.d/02_nolocales

# The following list of libraries was determined from the binaries up to the
# top most layer.
RUN mkdir -p /usr/share/man/man1 && \
    apt-get update -y && \
    apt-get upgrade -y && \
    apt-get dist-upgrade -y && \
    apt-get install -y --no-install-recommends \
    build-essential \
    ccache \
    clang \
    clang-format \
    clang-tidy \
    cmake \
    cppcheck \
    flawfinder \
    iwyu \
    llvm \
    ninja-build \
    pmccabe \
    upx \
    unzip \
    valgrind \
    vera++ \
    wget && \
    apt-get autoremove && \
    apt-get autoclean && \
    apt-get clean

RUN cd /tmp && \
    wget --no-check-certificate https://github.com/L2Program/FlintPlusPlus/archive/master.zip && \
    unzip master.zip && \
    cd /tmp/FlintPlusPlus-master/flint && \
    make -j2 && cp flint++ /usr/bin && \
    cd /tmp && rm -fr FlintPlusPlus-master master.zip && \
    cd /tmp && wget --no-check-certificate https://github.com/oclint/oclint/releases/download/v0.13.1/oclint-0.13.1-x86_64-linux-4.4.0-112-generic.tar.gz && \
    tar xvzf /tmp/oclint-0.13.1-x86_64-linux-4.4.0-112-generic.tar.gz -C /usr/local && rm -f /tmp/oclint-0.13.1-x86_64-linux-4.4.0-112-generic.tar.gz
