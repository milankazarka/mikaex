#!/bin/bash
g++ -c mikaApps.cpp -g -O
g++ -c mikaAppsMain.c -g -O
g++ -c mikaUID.c -g -O

g++ -o mikaAppsMain \
	mikaApps.o \
	mikaUID.c \
	mikaAppsMain.o -lpthread -luuid
