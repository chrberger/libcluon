# Copyright (C) 2018  Christian Berger
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

all: run

cluonDataStructures.proto: ../libcluon/resources/cluonDataStructures.odvd
	cluon-msc --proto --out=cluonDataStructures.proto ../libcluon/resources/cluonDataStructures.odvd

cluonDataStructures_pb2.py: cluonDataStructures.proto
	protoc --python_out=. cluonDataStructures.proto

MyExampleMessageSpec.proto: MyExampleMessageSpec.odvd
	cluon-msc --proto --out=MyExampleMessageSpec.proto MyExampleMessageSpec.odvd

MyExampleMessageSpec_pb2.py: MyExampleMessageSpec.proto
	protoc --python_out=. MyExampleMessageSpec.proto

run: cluonDataStructures_pb2.py MyExampleMessageSpec_pb2.py
	python receiveEnvelopes.py &
	sleep 2 && python sendEnvelopes.py

clean:
	rm -f *_pb2.py *pyc *.proto 
