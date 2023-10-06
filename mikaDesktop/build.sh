#!/bin/bash
g++ -g -O2 -c sysCommon.c -I/usr/local/include/cairo
g++ -g -O2 -c sysWindow.c -I/usr/local/include/cairo
g++ -g -O2 -c sysDesktop.c -I/usr/local/include/cairo
g++ -g -O2 -c mikaWindow.c -I/usr/local/include/cairo
g++ -g -O2 -c mikaDesktop.c -I/usr/local/include/cairo
g++ -g -O2 -c main.c -I/usr/local/include/cairo
g++ -o mikaDesktop main.o mikaDesktop.o \
	mikaWindow.o sysWindow.o sysCommon.o \
	sysDesktop.o \
	-lpthread -lX11 -lXext -lXrender -lXcomposite \
	-lcairo -lpangocairo-1.0 -lpango-1.0
	
