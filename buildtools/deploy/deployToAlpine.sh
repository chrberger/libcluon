#!/bin/bash

# Copyright (C) 2017  Christian Berger
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

PROJECT=libcluon
RELEASER_NAME="Christian Berger"
RELEASER_EMAIL="christian.berger@gu.se"
RELEASE_DATE=$(date -R)
OLDPWD=$(pwd)

if [ "$CHRBERGER_DOCKER_CLUON" == "" ]; then
    echo "No Docker oAuth specified."
    exit 1
fi

echo "Cleaning repository: "
make clean

RELEASE_VERSION=$(head -n1 changelog | cut -f2 -d"(" | cut -f1 -d"-")
echo "Current version to be deployed: $RELEASE_VERSION"

rm -fr alpine && mkdir -p alpine/abuild

################################################################################

cat <<EOF >alpine/APKBUILD
# Contributor: $RELEASER_NAME <$RELEASER_EMAIL>
# Maintainer: $RELEASER_NAME <$RELEASER_EMAIL>
pkgname=$PROJECT
pkgver=$RELEASE_VERSION
pkgrel=0
pkgdesc="$PROJECT"
url="https://github.com/chrberger/libcluon"
arch="all"
license="GPL3"
depends=""
makedepends=""
install=""
subpackages=""
source="\${pkgname}-\${pkgver}.tar.gz"
builddir="\$srcdir/"

build() {
	cd "\$builddir" && mkdir build && cd build && cmake -D CMAKE_INSTALL_PREFIX="\$pkgdir" ../libcluon
	make
}

check() {
	cd "\$builddir" && cd build
	make test
}

package() {
	cd "\$builddir" && cd build
	make install
}
EOF

################################################################################

cat <<EOF >alpine/Dockerfile
# Copyright (C) 2017  Christian Berger
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

FROM alpine:3.7
MAINTAINER $RELEASER_NAME "RELEASER_EMAIL"

RUN apk add --update \
    cmake \
    g++ \
    gcc \
    make \
    upx \
    alpine-sdk

RUN adduser -D berger && \
    echo "berger    ALL=(ALL) ALL" >> /etc/sudoers && \
    addgroup berger abuild && \
    chgrp abuild /var/cache/distfiles && \
    chmod g+w /var/cache/distfiles
EOF

################################################################################

cat <<EOF >alpine/abuild/abuild.conf
PACKAGER_PRIVKEY="/home/berger/.abuild/key.rsa"
EOF
chmod 644 alpine/abuild/builder.sh

################################################################################

cat <<EOF >alpine/builder.sh
#!/bin/sh
# Copyright (C) 2017  Christian Berger
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

cp -r /home/berger/tmp-alpine /home/berger/alpine
ln -sf /home/berger/alpine/abuild /home/berger/.abuild

cd /home/berger/alpine
abuild checksum
abuild
cp -r /home/berger/packages/berger/x86_64 /home/berger/tmp-alpine
EOF
chmod 755 alpine/builder.sh

################################################################################

rm -fr tmp.alpine && \
    mkdir -p tmp.alpine && \
    mv alpine/Dockerfile tmp.alpine && \
    mv alpine tmp.alpine && \
    tar cvfz tmp.alpine/alpine/libcluon-${RELEASE_VERSION}.tar.gz buildtools CODE_OF_CONDUCT.md libcluon Makefile README.md LICENSE && \
    cd tmp.alpine && \
    docker build -t chrberger/alpine-libcluon-builder . && \
    cat ~/.ssh/christian.berger@gu.se.priv > alpine/abuild/key.rsa && \
    openssl rsa -in ~/.ssh/christian.berger@gu.se.priv -pubout > alpine/abuild/key.rsa.pub && \
    docker run --rm -ti -w /home/berger --user=berger -h alpine-builder -v $PWD/alpine:/home/berger/tmp-alpine chrberger/alpine-libcluon-builder /home/berger/tmp-alpine/builder.sh && \
    git clone --branch gh-pages --depth 1 git@github.com:chrberger/libcluon.git && \
    cp -r alpine/x86_64/* libcluon/alpine/v3.7/x86_64 && \
    cd libcluon && git add -f alpine/v3.7/x86_64/* && git commit -s -m "Updated apk (x86_64)" && git push origin gh-pages && \
    curl -H "Content-Type: application/json" --data '{"build": true}' -X POST https://registry.hub.docker.com/u/chrberger/cluon/trigger/${CHRBERGER_DOCKER_CLUON}/

cd $OLDPWD && rm -fr tmp.alpine

