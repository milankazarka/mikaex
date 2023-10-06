#!/bin/bash

g++ -c mikaProject.c -O -g

g++ -c mikaXml/tinystr.cpp -O -g -I mikaXml
g++ -c mikaXml/tinyxml.cpp -O -g -I mikaXml
g++ -c mikaXml/tinyxmlerror.cpp -O -g -I mikaXml
g++ -c mikaXml/tinyxmlparser.cpp -O -g -I mikaXml

g++ -o mikaProject mikaProject.c \
	tinyxml.o tinystr.o tinyxmlerror.o tinyxmlparser.o \
	-ImikaXml
