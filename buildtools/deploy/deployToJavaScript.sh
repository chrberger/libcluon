#!/bin/bash

# Copyright (C) 2017  Christian Berger
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

PROJECT=libcluon
RELEASER_NAME="Christian Berger"
RELEASER_EMAIL="christian.berger@gu.se"
RELEASE_DATE=$(date -R)
OLDPWD=$(pwd)

if [ "$CHRBERGER_JFROG" == "" ]; then
    echo "No JFROG oAuth specified."
    exit 1
fi

echo "Cleaning repository: "
make clean

RELEASE_VERSION=$(head -n1 changelog|cut -f2 -d"("|cut -f1 -d")")
echo "Current version to be deployed: $RELEASE_VERSION"

################################################################################

# TODO: Add Docker builder for emscripten.

################################################################################

rm -fr tmp.javascript && \
    mkdir -p tmp.javascript && \
    tar cvfz tmp.javascript/libcluon-${RELEASE_VERSION}.tar.gz buildtools CODE_OF_CONDUCT.md libcluon Makefile README.md LICENSE && \
    cd tmp.javascript && \
    tar xvzf libcluon-${RELEASE_VERSION}.tar.gz && \
    source ~/GITHUB/emsdk/emsdk_env.sh && \
    mkdir build && cd build && \
    cmake -D CMAKE_C_COMPILER=emcc -D CMAKE_CXX_COMPILER=em++ -DWEB=1 ../libcluon/ && \
    make && \
    curl -T libcluon.js -uchrberger:$CHRBERGER_JFROG https://api.bintray.com/content/chrberger/libcluon/javascript/master/${RELEASE_VERSION}/ && \
    curl -X POST -uchrberger:${CHRBERGER_JFROG} https://api.bintray.com/content/chrberger/libcluon/javascript/master/publish

cd $OLDPWD && rm -fr tmp.javascript

