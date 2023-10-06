#!/bin/bash
g++ -c mikaNetServer.c -g -O -I mikaXml
g++ -c mikaNetServerTest.c -g -O -I mikaXml

g++ -o mikaNetServerTest \
	mikaNetServerTest.o \
	mikaNetServer.o \
	-lcurl -lpthread
	
	