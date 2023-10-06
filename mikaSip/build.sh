#!/bin/bash
g++ -c mikaApps.cpp
g++ -c mikaUID.c
g++ -c microDebug.c

g++ -c mikaSipCaller.c
g++ -c mikaSipCallerApp.c
g++ -o mikaSipCallerApp mikaSipCallerApp.o \
	mikaSipCaller.o \
	microDebug.c mikaApps.cpp mikaUID.c -luuid -lpthread
	
	