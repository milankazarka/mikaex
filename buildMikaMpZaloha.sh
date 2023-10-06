#!/bin/bash
#g++ -c `sdl-config --cflags` mikaMpDesktop.c
g++ -g mikaXml/tinystr.cpp -O -g -I mikaXml
g++ -g mikaXml/tinyxml.cpp -O -g -I mikaXml
g++ -g mikaXml/tinyxmlerror.cpp -O -g -I mikaXml
g++ -g mikaXml/tinyxmlparser.cpp -O -g -I mikaXml

g++ -g -O2 -c mikaWF.c
g++ -g -O2 -c mikaWFWidget.c
g++ -g -O2 -c mikaWFWindow.c
g++ -g -O2 -c mikaMessage.c -I mikaXml
g++ -g -O2 -c mikaPBO.c \
	-I/usr/local/include/cairo \
	-I/usr/local/include/pango-1.0 \
	-I/opt/mika/include -I/usr/local/include
g++ -o mikaMpDesktop mikaMpDesktop.c \
	mikaPBO.o \
	mikaWF.o \
	mikaWFWindow.o \
	mikaWFWidget.o \
	mikaMessage.o \
	tinyxml.o tinystr.o tinyxmlerror.o tinyxmlparser.o \
	-I/usr/local/include/cairo \
	-I/usr/local/include/pango-1.0 \
	-I/opt/mika/include -I/usr/local/include \
	`sdl-config --libs --cflags` \
	-lpthread -lcairo -lpangocairo-1.0 -lpango-1.0 \
	-lz -lm -lGL -lGLU
	