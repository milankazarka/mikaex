#!/bin/bash
g++ -c mikaKit.cpp -O -g -I ../
g++ -c mikaWindow.cpp -O -g -I ../
g++ -c mikaScene.cpp -O -g -I ../
g++ -c mikaCanvas.cpp -O -g -I ../
g++ -c mikaWidget.cpp -O -g -I ../
g++ -c mikaImage.cpp -O -g -I ../

ar rvs libMikaKit.a \
	mikaKit.o \
	mikaWindow.o \
	mikaScene.o \
	mikaCanvas.o \
	mikaWidget.o \
	mikaImage.o

g++ -o mikaKitTest mikaKitTest.c \
	libMikaKit.a \
	../libMika.a \
	-latk-1.0 -lgio-2.0 -lpangoft2-1.0 \
	-lgdk_pixbuf-2.0 -lpangocairo-1.0 -lcairo -lpango-1.0 \
	-lfontconfig -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lglib-2.0 \
	-lpthread -lX11 -lXext \
	-lXrender -DGUI -DUSEX -DUNIX -DJPEG -lXpm \
	-DUSESSL -ltcl8.5 -ltk8.5 \
	-lz -lm -lcrypto -lcurl
	