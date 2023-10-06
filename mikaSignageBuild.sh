#!/bin/bash
g++ -c mikaParser.c
g++ -c mikaSignage.c
g++ -c mikaIpc.c -lpthread
g++ -c os.c
g++ -o mikaSignage mikaSignage.o mikaParser.o os.o \
	\
	-lpthread -lcurl -lX11
