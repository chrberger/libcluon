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

RELEASE_VERSION=$(head -n1 changelog | cut -f2 -d"(" | cut -f1 -d")")

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

