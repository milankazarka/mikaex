#!/bin/bash
g++ -g -O2 -c mikaWM.c -I/usr/local/include/cairo
g++ -g -O2 -c mikaWMError.c -I/usr/local/include/cairo
g++ -o mikaWM mikaWM.o \
	mikaWMError.o \
	-lpthread -lX11 -lXext -lXrender -lXcomposite \
	-lcairo -lpangocairo-1.0 -lpango-1.0

	
