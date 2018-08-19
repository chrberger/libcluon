#!/bin/bash

# Copyright (C) 2017  Christian Berger
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

RELEASE_VERSION=$(head -n1 changelog | cut -f2 -d"(" | cut -f1 -d"-")

OLDPWD=$PWD

if [ -d api ]; then
    rm -fr api
fi

cat buildtools/doxygen.cfg | sed -e s/%VERSIONNUMBER%/$RELEASE_VERSION/g > $OLDPWD/doxygen.cfg
doxygen $OLDPWD/doxygen.cfg && rm -f $OLDPWD/doxygen.cfg

if [ -d api ]; then
    cd api && \
    git clone --branch gh-pages --depth 1 git@github.com:chrberger/libcluon.git && \
    cd libcluon && \
    git checkout gh-pages && \
    git rm -rf * && git reset -- alpine && git checkout -- alpine && git reset -- headeronly && git checkout -- headeronly && git commit -m "Cleaning" -a -s && git push origin gh-pages && cp ../html/* . && touch .nojekyll && git add * && git add .nojekyll && git commit -m "Documentation for v$RELEASE_VERSION" -a -s && git push origin gh-pages && \
    cd ../.. && \
    rm -fr api
fi

