#!/bin/bash
g++ -c -g mikaCeObject.c
g++ -c -g mikaCe.c
g++ -c -g mikaCeParcel.c
g++ -c -g mikaCeNetwork.c
g++ -c -g base64.c

g++ -c mikaXml/tinystr.cpp -O -g -I mikaXml
g++ -c mikaXml/tinyxml.cpp -O -g -I mikaXml
g++ -c mikaXml/tinyxmlerror.cpp -O -g -I mikaXml
g++ -c mikaXml/tinyxmlparser.cpp -O -g -I mikaXml

g++ -o mikaCe mikaCe.o mikaCeObject.o mikaCeParcel.o \
	mikaCeNetwork.o base64.o \
	tinyxml.o tinystr.o tinyxmlerror.o tinyxmlparser.o \
	-lpthread -lcurl -I mikaXml
