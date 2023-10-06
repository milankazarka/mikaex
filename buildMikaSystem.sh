#!/bin/bash

g++ -c -g mikaMessage.c
g++ -c -g mikaNetRequest.c
g++ -c -g mikaCeObject.c
g++ -c -g mikaCe.c
g++ -c -g mikaCeParcel.c
g++ -c -g mikaCeNetwork.c
g++ -c -g mikaCeClient.c
g++ -c -g base64.c

g++ -c mikaXml/tinystr.cpp -O -g -I mikaXml
g++ -c mikaXml/tinyxml.cpp -O -g -I mikaXml
g++ -c mikaXml/tinyxmlerror.cpp -O -g -I mikaXml
g++ -c mikaXml/tinyxmlparser.cpp -O -g -I mikaXml

g++ -o mikaCe mikaCeMain.c mikaCe.o mikaCeObject.o mikaCeParcel.o \
	mikaCeNetwork.o mikaCeClient.o mikaNetRequest.o base64.o \
	mikaMessage.o \
	tinyxml.o tinystr.o tinyxmlerror.o tinyxmlparser.o \
	-lpthread -lcurl -ImikaXml

g++ -g -O2 -c mikaWF.c
g++ -g -O2 -c mikaWFWidget.c
g++ -g -O2 -c mikaWFWindow.c
g++ -g -O2 -c mikaPBO.c \
	-I/usr/local/include/cairo \
	-I/usr/local/include/pango-1.0 \
	-I/opt/mika/include -I/usr/local/include

g++ -o mikaMpDesktop mikaMpDesktop.c \
	mikaPBO.o \
	mikaWF.o \
	mikaWFWindow.o \
	mikaWFWidget.o \
	mikaCe.o \
	mikaCeObject.o \
	mikaCeParcel.o \
	mikaCeNetwork.o \
	mikaCeClient.o \
	mikaMessage.o \
	mikaNetRequest.o \
	tinyxml.o tinystr.o tinyxmlerror.o tinyxmlparser.o \
	-I/usr/local/include/cairo \
	-I/usr/local/include/pango-1.0 \
	-I/opt/mika/include -I/usr/local/include \
	-ImikaXml \
	`sdl-config --libs --cflags` \
	-lpthread -lcairo -lpangocairo-1.0 -lpango-1.0 \
	-lz -lm -lGL -lGLU -lpthread -lcurl
