#!/bin/bash
g++ -c mikaUID.c
g++ -c mikaApps.cpp
g++ -c mikaMpWindowClientMain.c
g++ -c mikaMpWindowClient.c
g++ -c mikaNetCurl.c
g++ -c mikaNetRequest.c
g++ -o mikaMpWindowClientMain \
	mikaUID.o \
	mikaApps.o \
	mikaMpWindowClientMain.o \
	mikaMpWindowClient.o \
	tinyxml.o tinystr.o tinyxmlparser.o tinyxmlerror.o \
	mikaNetRequest.o mikaNetCurl.o -lcurl -lpthread -luuid
	