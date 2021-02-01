#!/bin/bash

# Copyright (C) 2018  Christian Berger
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

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

RELEASE_VERSION=$(head -n1 changelog | cut -f2 -d"(" | cut -f1 -d"-")
echo "Current version to be deployed: $RELEASE_VERSION"

################################################################################

rm -f libcluon.js && rm -fr tmp.javascript && mkdir -p tmp.javascript

cat <<EOF > tmp.javascript/javascript-builder.sh
#!/bin/bash
# Copyright (C) 2018  Christian Berger
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

source /opt/emsdk/emsdk_env.sh

cd /opt && \
    mkdir build && cd build && \
    cmake -D CMAKE_C_COMPILER=emcc -D CMAKE_CXX_COMPILER=em++ -DWEB=1 ../source/libcluon && \
    make -j2 && \
    cp libcluon.js ../source && chown 1000:1000 ../source/libcluon.js
EOF
chmod 755 tmp.javascript/javascript-builder.sh

################################################################################

cat <<EOF > tmp.javascript/Dockerfile.javascript
# Copyright (C) 2018  Christian Berger
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

FROM ubuntu:18.04
MAINTAINER Christian Berger "christian.berger@gu.se"

# Set the env variable DEBIAN_FRONTEND to noninteractive
ENV DEBIAN_FRONTEND noninteractive

# The following list of libraries was determined from the binaries up to the
# top most layer.
RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get dist-upgrade -y && \
    apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    docker.io \
    python2.7 \
    nodejs \
    default-jre && \
    apt-get autoremove && \
    apt-get autoclean && \
    apt-get clean

RUN ln -sf /usr/bin/python2.7 /usr/bin/python && \ 
    cd /opt && \
    git clone https://github.com/juj/emsdk.git && \
    cd emsdk && \
    git pull --all && \
    git pull origin master && \
    ./emsdk install latest && \
    ./emsdk activate latest

ADD ./javascript-builder.sh /opt/javascript-builder.sh
EOF

################################################################################
# Build Docker image for building Javascript.
if [[ "$(docker images -q chrberger/javascript-libcluon-builder:latest 2> /dev/null)" == "" ]]; then
    docker build -t chrberger/javascript-libcluon-builder:latest -f tmp.javascript/Dockerfile.javascript tmp.javascript
fi

################################################################################
# Run JavaScript builder.
docker run --rm -i -v $PWD:/opt/source -v /var/run/docker.sock:/var/run/docker.sock chrberger/javascript-libcluon-builder:latest /opt/javascript-builder.sh

################################################################################
# Deploy.
curl -T libcluon.js -uchrberger:$CHRBERGER_JFROG https://api.bintray.com/content/chrberger/libcluon/javascript/master2/${RELEASE_VERSION}/ && \
    curl -X POST -uchrberger:${CHRBERGER_JFROG} https://api.bintray.com/content/chrberger/libcluon/javascript/master2/publish && \
    rm -fr tmp.javascript && rm -f libcluon.js

