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

if [ "$CHRBERGER_GH" == "" ]; then
    echo "No Github oAuth specified."
    exit 1
fi

echo "Cleaning repository: "
make clean

echo "Current version: `head -n1 changelog`"

echo "Next version: "
read RELEASE_VERSION

echo "One line description starting with * : "
read RELEASE_TEXT

rm -fr debian && mkdir debian

################################################################################

cat <<EOF >debian/rules
#!/usr/bin/make -f

BUILDDIR = build_dir

# secondly called by launchpad
build:
	mkdir \$(BUILDDIR);
	cd \$(BUILDDIR); cmake -DCMAKE_INSTALL_PREFIX=../debian/tmp/usr ../${PROJECT}
	make -C \$(BUILDDIR) && CTEST_OUTPUT_ON_FAILURE=1 make -C \$(BUILDDIR) test

# thirdly called by launchpad
binary: binary-indep binary-arch

binary-indep:
	# nothing to be done

binary-arch:
	cd \$(BUILDDIR); cmake -P cmake_install.cmake
	mkdir -p debian/tmp/DEBIAN
	dpkg-gencontrol -p${PROJECT}
	dpkg --build debian/tmp ..

# firstly called by launchpad
clean:
	rm -f build
	rm -rf \$(BUILDDIR)

.PHONY: binary binary-arch binary-indep clean
EOF
chmod 755 debian/rules

################################################################################

cat <<EOF >debian/copyright
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: libcluon
Source: https://github.com/chrberger/libcluon

Files: *
Copyright: 2017 Christian Berger <christian.berger@gu.se>
License: GPL-3.0+

Files: debian/*
Copyright: 2017 Christian Berger <christian.berger@gu.se>
License: GPL-3.0+

Files: buildtools/xUnit/*
Copyright: 2017 Two Blue Cubes Ltd. All rights reserved.
License: Boost-1.0

Files:
Files: libcluon/thirdparty/argh/*
Copyright: Copyright (c) 2016 Adi Shavit. All rights reserved.
License: BSD-3-Clause

Files: libcluon/thirdparty/cpp-peglib/*
Copyright: Copyright (c) 2015-17 Yuji Hirose. All rights reserved.
License: MIT

Files: libcluon/thirdparty/Mustache/*
Copyright: Copyright 2015-2017 Kevin Wojniak.
License: Boost-1.0


License: GPL-3.0+
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 .
 This package is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 .
 You should have received a copy of the GNU General Public License
 along with this program. If not, see <https://www.gnu.org/licenses/>.
 .
 On Debian systems, the complete text of the GNU General
 Public License version 3 can be found in "/usr/share/common-licenses/GPL-3".


License: Boost-1.0
 Boost Software License - Version 1.0 - August 17th, 2003
 .
 Permission is hereby granted, free of charge, to any person or organization
 obtaining a copy of the software and accompanying documentation covered by
 this license (the "Software") to use, reproduce, display, distribute,
 execute, and transmit the Software, and to prepare derivative works of the
 Software, and to permit third-parties to whom the Software is furnished to
 do so, all subject to the following:
 .
 The copyright notices in the Software and this entire statement, including
 the above license grant, this restriction and the following disclaimer,
 must be included in all copies of the Software, in whole or in part, and
 all derivative works of the Software, unless such copies or derivative
 works are solely in the form of machine-executable object code generated by
 a source language processor.
 .
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
 .
 You should have received a copy of the Boost Software License
 along with this program. If not, see <http://www.boost.org/LICENSE_1_0.txt>.


License: BSD-3-Clause
 Copyright (c) 2016, Adi Shavit 
 All rights reserved. 
 .
 Redistribution and use in source and binary forms, with or without 
 modification, are permitted provided that the following conditions are met: 
 .
 * Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer. 
 * Redistributions in binary form must reproduce the above copyright 
   notice, this list of conditions and the following disclaimer in the 
   documentation and/or other materials provided with the distribution. 
 * Neither the name of  nor the names of its contributors may be used to 
   endorse or promote products derived from this software without specific 
   prior written permission. 
 .
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 POSSIBILITY OF SUCH DAMAGE.


License: MIT
 The MIT License (MIT)
 .
Copyright (c) 2015 yhirose
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 .
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 .
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
EOF

################################################################################

cat <<EOF >debian/control
Source: libcluon
Section: devel
Priority: optional
Maintainer: Christian Berger <christian.berger@gu.se>
Build-Depends: cmake, build-essential
Homepage: https://github.com/chrberger/libcluon

Package: libcluon
Architecture: any
Depends: 
Description: libcluon is a small and efficient library written in modern C++
             to glue distributed software components together - in a clever
             way - simly: cluon.
EOF

cat <<EOF >debian/changelog
${PROJECT} (${RELEASE_VERSION}) xenial; urgency=low

  ${RELEASE_TEXT}

 -- ${RELEASER_NAME} <${RELEASER_EMAIL}>  ${RELEASE_DATE}

EOF

cat changelog >> debian/changelog

################################################################################

rm -fr tmp.launchpad && mkdir -p tmp.launchpad/libcluon && mv debian tmp.launchpad/libcluon && tar cvfz tmp.launchpad/libcluon-${RELEASE_VERSION}.orig.tar.gz buildtools CODE_OF_CONDUCT.md libcluon Makefile README.md LICENSE && tar xvzf tmp.launchpad/libcluon-${RELEASE_VERSION}.orig.tar.gz -C tmp.launchpad/libcluon && cd tmp.launchpad/libcluon && debuild -S -sd && cd ..

x=5
while [ $x -gt 0 ]; do
    echo "Still waiting (${x}s) before pushing to Launchpad - stop me now if not wanted..."
    sleep 1
    x=$(( $x - 1 ))
done

# Push Debian source package to Launchpad.
dput ppa:chrberger/libcluon libcluon_${RELEASE_VERSION}_source.changes

cat libcluon/debian/changelog > ../changelog

cd ..

echo $RELEASE_VERSION > VERSION

git commit -a -s -m "Releasing v${RELEASE_VERSION}" && git push origin master

# Push release to Github
RELEASE_DATA='{"tag_name": "v'${RELEASE_VERSION}'","target_commitish": "'$(git rev-parse HEAD)'","name": "v'${RELEASE_VERSION}'","body": "'${RELEASE_TEXT}'","draft": false,"prerelease": false}'
echo $RELEASE_DATA
curl --data "$RELEASE_DATA" https://api.github.com/repos/chrberger/libcluon/releases?access_token=$CHRBERGER_GH

rm -fr tmp.launchpad