#!/bin/bash
g++ -c mikaMpSched.c -g -O2
g++ -c mikaMpSchedTest.c -g -O2

g++ -o mikaMpSchedTest \
	mikaMpSchedTest.o \
	mikaMpSched.o -lpthread
