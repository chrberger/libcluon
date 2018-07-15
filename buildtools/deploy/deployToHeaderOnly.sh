#!/bin/bash

# Copyright (C) 2018  Christian Berger
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
# along with this program.  If not, see <http:#www.gnu.org/licenses/>.

PROJECT=libcluon
RELEASER_NAME="Christian Berger"
RELEASER_EMAIL="christian.berger@gu.se"
RELEASE_DATE=$(date -R)
OLDPWD=$(pwd)

RELEASE_VERSION=$(head -n1 changelog | cut -f2 -d"(" | cut -f1 -d"-")
echo "Current version to be deployed: $RELEASE_VERSION"

rm -fr tmp.headeronly && mkdir -p tmp.headeronly

cat <<EOF >> tmp.headeronly/cluon-complete.hpp
// This is an auto-generated header-only single-file distribution of libcluon.
// Date: ${RELEASE_DATE}
// Version: ${RELEASE_VERSION}
//
EOF

# thirdparty/cluon/any/any.hpp --> remove #pragma once
# thirdparty/cpp-peglib/peglib.h
# thirdparty/argh/argh.h --> remove #pragma once + header guards

cat libcluon/thirdparty/cluon/any/any.hpp >> tmp.headeronly/cluon-complete.hpp
cat tmp.headeronly/cluon-complete.hpp | sed -e 's/^#pragma/\/\/#pragma/g' > tmp.headeronly/cluon-complete.hpp.tmp && mv tmp.headeronly/cluon-complete.hpp.tmp tmp.headeronly/cluon-complete.hpp

cat libcluon/thirdparty/cpp-peglib/peglib.h >> tmp.headeronly/cluon-complete.hpp

cat <<EOF >> tmp.headeronly/cluon-complete.hpp
#ifndef ARGH
#define ARGH
EOF
cat libcluon/thirdparty/argh/argh.h >> tmp.headeronly/cluon-complete.hpp
cat tmp.headeronly/cluon-complete.hpp | sed -e 's/^#pragma/\/\/#pragma/g' > tmp.headeronly/cluon-complete.hpp.tmp && mv tmp.headeronly/cluon-complete.hpp.tmp tmp.headeronly/cluon-complete.hpp
cat <<EOF >> tmp.headeronly/cluon-complete.hpp
#endif
EOF

docker pull chrberger/cluon
docker run --rm -v $PWD/libcluon/resources/cluonDataStructures.odvd:/opt/cluonDataStructures.odvd chrberger/cluon cluon-msc --cpp /opt/cluonDataStructures.odvd >> tmp.headeronly/cluon-complete.hpp

cat libcluon/thirdparty/cluon/stringtoolbox.hpp >> tmp.headeronly/cluon-complete.hpp

for i in \
    cluon/Time.hpp \
    cluon/PortableEndian.hpp \
    cluon/cluon.hpp \
    cluon/MetaMessage.hpp \
    cluon/MessageParser.hpp \
    cluon/TerminateHandler.hpp \
    cluon/NotifyingPipeline.hpp \
    cluon/UDPPacketSizeConstraints.hpp \
    cluon/UDPSender.hpp \
    cluon/UDPReceiver.hpp \
    cluon/TCPConnection.hpp \
    cluon/TCPServer.hpp \
    cluon/ProtoConstants.hpp \
    cluon/ToProtoVisitor.hpp \
    cluon/FromProtoVisitor.hpp \
    cluon/FromLCMVisitor.hpp \
    cluon/MsgPackConstants.hpp \
    cluon/FromMsgPackVisitor.hpp \
    cluon/JSONConstants.hpp \
    cluon/FromJSONVisitor.hpp \
    cluon/ToJSONVisitor.hpp \
    cluon/ToCSVVisitor.hpp \
    cluon/ToLCMVisitor.hpp \
    cluon/ToODVDVisitor.hpp \
    cluon/ToMsgPackVisitor.hpp \
    cluon/Envelope.hpp \
    cluon/EnvelopeConverter.hpp \
    cluon/GenericMessage.hpp \
    cluon/LCMToGenericMessage.hpp \
    cluon/OD4Session.hpp \
    cluon/Player.hpp \
    cluon/SharedMemory.hpp; do
cat libcluon/include/$i >> tmp.headeronly/cluon-complete.hpp
done


cat <<EOF >> tmp.headeronly/cluon-complete.cpp
#ifndef BEGIN_HEADER_ONLY_IMPLEMENTATION
#define BEGIN_HEADER_ONLY_IMPLEMENTATION
EOF
for i in \
    cluon.cpp \
    MetaMessage.cpp \
    MessageParser.cpp \
    TerminateHandler.cpp \
    UDPSender.cpp \
    UDPReceiver.cpp \
    TCPConnection.cpp \
    TCPServer.cpp \
    ToProtoVisitor.cpp \
    FromProtoVisitor.cpp \
    FromLCMVisitor.cpp \
    FromMsgPackVisitor.cpp \
    FromJSONVisitor.cpp \
    GenericMessage.cpp \
    ToJSONVisitor.cpp \
    ToCSVVisitor.cpp \
    ToLCMVisitor.cpp \
    LCMToGenericMessage.cpp \
    ToMsgPackVisitor.cpp \
    OD4Session.cpp \
    ToODVDVisitor.cpp \
    EnvelopeConverter.cpp \
    Player.cpp \
    SharedMemory.cpp; do
cat libcluon/src/$i >> tmp.headeronly/cluon-complete.cpp
done
cat <<EOF >> tmp.headeronly/cluon-complete.cpp
#endif
EOF

# Making method definitions inline.
cat tmp.headeronly/cluon-complete.cpp | sed -e 's/\(^[^\ \}\#\/\\].*::.*\)/inline\ \1/g' >> tmp.headeronly/cluon-complete.hpp

cat <<EOF >> tmp.headeronly/cluon-complete.hpp
#ifdef HAVE_CLUON_MSC
EOF
cat libcluon/thirdparty/Mustache/mustache.hpp >> tmp.headeronly/cluon-complete.hpp

for i in \
    cluon/MetaMessageToCPPTransformator.hpp \
    cluon/MetaMessageToProtoTransformator.hpp; do
cat libcluon/include/$i >> tmp.headeronly/cluon-complete.hpp
done

for i in \
    MetaMessageToCPPTransformator.cpp \
    MetaMessageToProtoTransformator.cpp; do
cat libcluon/src/$i >> tmp.headeronly/cluon-complete.hpp
done

cat libcluon/tools/cluon-msc.hpp >> tmp.headeronly/cluon-complete.hpp
cat libcluon/tools/cluon-msc.cpp >> tmp.headeronly/cluon-complete.hpp

cat <<EOF >> tmp.headeronly/cluon-complete.hpp
#endif
EOF

cat <<EOF >> tmp.headeronly/cluon-complete.hpp
#ifdef HAVE_CLUON_REPLAY
EOF
cat libcluon/tools/cluon-replay.hpp >> tmp.headeronly/cluon-complete.hpp
cat libcluon/tools/cluon-replay.cpp >> tmp.headeronly/cluon-complete.hpp
cat <<EOF >> tmp.headeronly/cluon-complete.hpp
#endif
EOF

cat <<EOF >> tmp.headeronly/cluon-complete.hpp
#ifdef HAVE_CLUON_LIVEFEED
EOF
cat libcluon/tools/cluon-livefeed.hpp >> tmp.headeronly/cluon-complete.hpp
cat libcluon/tools/cluon-livefeed.cpp >> tmp.headeronly/cluon-complete.hpp
cat <<EOF >> tmp.headeronly/cluon-complete.hpp
#endif
EOF

cat <<EOF >> tmp.headeronly/cluon-complete.hpp
#ifdef HAVE_CLUON_REC2CSV
EOF
cat libcluon/tools/cluon-rec2csv.hpp >> tmp.headeronly/cluon-complete.hpp
cat libcluon/tools/cluon-rec2csv.cpp >> tmp.headeronly/cluon-complete.hpp
cat <<EOF >> tmp.headeronly/cluon-complete.hpp
#endif
EOF

cat tmp.headeronly/cluon-complete.hpp | sed -e 's/^#include\ \"cluon\//\/\/#include\ \"cluon\//g' > tmp.headeronly/cluon-complete.hpp.tmp && mv tmp.headeronly/cluon-complete.hpp.tmp tmp.headeronly/cluon-complete.hpp
cat tmp.headeronly/cluon-complete.hpp | sed -e 's/^#include\ \"cpp-peglib\//\/\/#include\ \"cpp-peglib\//g' > tmp.headeronly/cluon-complete.hpp.tmp && mv tmp.headeronly/cluon-complete.hpp.tmp tmp.headeronly/cluon-complete.hpp
cat tmp.headeronly/cluon-complete.hpp | sed -e 's/^#include\ \"argh\//\/\/#include\ \"argh\//g' > tmp.headeronly/cluon-complete.hpp.tmp && mv tmp.headeronly/cluon-complete.hpp.tmp tmp.headeronly/cluon-complete.hpp
cat tmp.headeronly/cluon-complete.hpp | sed -e 's/^#include\ \"Mustache\//\/\/#include\ \"Mustache\//g' > tmp.headeronly/cluon-complete.hpp.tmp && mv tmp.headeronly/cluon-complete.hpp.tmp tmp.headeronly/cluon-complete.hpp

################################################################################

x=5
while [ $x -gt 0 ]; do
    echo "Still waiting (${x}s) before pushing headeronly - stop me now if not wanted..."
    sleep 1
    x=$(( $x - 1 ))
done

cd tmp.headeronly && \
    git clone --branch gh-pages --depth 1 git@github.com:chrberger/libcluon.git && \
    mkdir -p libcluon/headeronly && \
    cp -r cluon-complete.hpp libcluon/headeronly/cluon-complete-v${RELEASE_VERSION}.hpp && \
    cp -r cluon-complete.hpp libcluon/headeronly/cluon-complete.hpp && \
    cd libcluon && git add -f headeronly/cluon-complete-v${RELEASE_VERSION}.hpp headeronly/cluon-complete.hpp && git commit -s -m "Updated header-only" && git push origin gh-pages

cd $OLDPWD && rm -fr tmp.headeronly

