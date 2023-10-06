#!/bin/bash

g++ -c mikaDb.c -I mikaXml
g++ -c mikaXml/tinystr.cpp -O -g -I mikaXml
g++ -c mikaXml/tinyxml.cpp -O -g -I mikaXml
g++ -c mikaXml/tinyxmlerror.cpp -O -g -I mikaXml
g++ -c mikaXml/tinyxmlparser.cpp -O -g -I mikaXml
g++ -c mikaServerModDelegate.c
g++ -c mikaServerMod.c
g++ -c mikaConfigServerMod.c
g++ -c mikaPosMod.c
g++ -c mikaServerPosDelegate.c
g++ -c mikaServer.c
g++ -c mikaPosClientEx.c
g++ -c mikaNetRequest.c
g++ -c mikaPosServer.c
g++ -c mikaPosServerObjects.c
g++ -c mikaNetCurl.c

g++ -o mikaServer \
	mikaServer.o \
	mikaServerModDelegate.o \
	mikaServerMod.o \
	mikaServerPosDelegate.o \
	mikaNetCurl.o \
	mikaPosServer.o \
	mikaPosServerObjects.o \
	mikaPosMod.o \
	mikaConfigServerMod.o \
	mikaDb.c \
	mikaDom.c \
	tinystr.o \
	tinyxml.o \
	tinyxmlerror.o \
	tinyxmlparser.o \
	-lpthread -ImikaXml -ldb -lcurl

g++ -o mikaPosClientEx \
	mikaPosClientEx.o \
	mikaNetRequest.o \
	mikaNetCurl.o \
	tinystr.o \
	tinyxml.o \
	tinyxmlerror.o \
	tinyxmlparser.o \
	-lpthread -ImikaXml -ldb -lcurl
	