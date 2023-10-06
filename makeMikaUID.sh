#!/bin/bash
g++ -c mikaUID.c
g++ -c mikaUIDMain.c

g++ -o mikaUID mikaUID.o mikaUIDMain.o -luuid

