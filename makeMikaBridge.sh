#!/bin/bash

g++ -c os.c
g++ -o bridge bridge.c os.o -lpthread -lX11
