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
