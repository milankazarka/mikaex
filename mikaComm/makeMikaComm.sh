#!/bin/bash
g++ -c mikaXml/tinystr.cpp -O -g -I mikaXml -I../
g++ -c mikaXml/tinyxml.cpp -O -g -I mikaXml -I../
g++ -c mikaXml/tinyxmlerror.cpp -O -g -I mikaXml -I../
g++ -c mikaXml/tinyxmlparser.cpp -O -g -I mikaXml -I../

g++ -c mikaCurl.c -O -g -I mikaXml -I../
g++ -c mikaCommCMS.c -O -g -I mikaXml -I../
g++ -c mikaCommHttp.c -O -g -I mikaXml -I../
g++ -c mikaCommCMSTest.c -O -g -I mikaXml -I../
g++ -c ../mikaNetRequest.c -I../ -O -g -I mikaXml -I../
g++ -c mikaCommAgnis.c -O -g -I mikaXml
g++ -c mikaCommAgnisTest.c -O -g -I mikaXml
g++ -c mikaCommAgnisServer.c -O -g -I mikaXml -I ../

ar rvs libMikaComm.a \
	tinystr.o \
	tinyxml.o \
	tinyxmlerror.o \
	tinyxmlparser.o \
	mikaCurl.o \
	mikaCommCMS.o \
	mikaCommHttp.o \
	mikaCommAgnis.o \
	mikaNetRequest.o

g++ -o mikaCommCMSTest \
	mikaCommCMSTest.o \
	libMikaComm.a \
	-lcurl
	
g++ -o mikaCommCMSServer mikaCommCMSServer.c \
	libMikaComm.a \
	-lcurl

g++ -o mikaCommHttpTest mikaCommHttpTest.c \
	libMikaComm.a \
	-lcurl

g++ -o mikaCommAgnisTest mikaCommAgnisTest.c \
	libMikaComm.a \
	-lcurl

g++ -o mikaCommAgnisServer mikaCommAgnisServer.c \
	../mikaNetServer.o \
	libMikaComm.a \
	-lcurl
	