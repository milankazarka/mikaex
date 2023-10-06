#!/bin/bash
#g++ -c `sdl-config --cflags` mikaMpDesktop.c
echo "building XML library"
g++ -c mikaXml/tinystr.cpp -O2 -g -I mikaXml
g++ -c mikaXml/tinyxml.cpp -O2 -g -I mikaXml
g++ -c mikaXml/tinyxmlerror.cpp -O2 -g -I mikaXml
g++ -c mikaXml/tinyxmlparser.cpp -O2 -g -I mikaXml

g++ -c mikaNetCurl.c

echo "building Mika Net Server"
g++ -c mikaNetServer.c -g -O2 -I mikaXml
g++ -c mikaNetServerTest.c -g -O2 -I mikaXml
g++ -c mikaNetRequest.c -g -O2 -I mikaXml
g++ -o mikaNetServerTest \
	mikaNetServerTest.o \
	mikaNetCurl.o \
	mikaNetServer.o \
	mikaNetRequest.o \
	-lcurl -lpthread

echo "building Compositing Editor"
g++ -c mikaCeObject.c -g -O2
g++ -c mikaCe.c -g -O2
g++ -c mikaCeParcel.c -g -O2
g++ -c mikaCeNetwork.c -g -O2
g++ -c base64.c

g++ -g -O2 -c mikaMessage.c -ImikaXml
g++ -g -O2 -c mikaWF.c
g++ -g -O2 -c mikaWFWidget.c
g++ -g -O2 -c mikaWFWindow.c
g++ -g -O2 -c mikaMessage.c -I mikaXml
g++ -g -O2 -c mikaPBO.c \
	-I/usr/local/include/cairo \
	-I/usr/local/include/pango-1.0 \
	-I/opt/mika/include -I/usr/local/include
echo "building Mp Desktop"
g++ -g -O2 -c mikaMpController.c
g++ -g -O2 -c mikaMpSetup.c
g++ -g -O2 -c mikaMpSched.c
g++ -g -O2 -c mikaMpWindowClient.c
g++ -g -O2 -c mikaMpWindowClientMain.c
g++ -g -O2 -c mikaMpDesktopServer.c

g++ -g -O2 -c mikaUID.c
g++ -g -O2 -c mikaApps.cpp

g++ -o mikaMpDesktop mikaMpDesktop.c \
	mikaUID.o \
	mikaApps.o \
	mikaMpController.o \
	mikaMpSetup.o \
	mikaMpSched.o \
	mikaNetCurl.o \
	mikaNetRequest.o \
	mikaMpWindowClient.o \
	mikaMpDesktopServer.o \
	mikaPBO.o \
	mikaWF.o \
	mikaWFWindow.o \
	mikaWFWidget.o \
	tinystr.o tinyxml.o tinyxmlerror.o tinyxmlparser.o \
	mikaMessage.o \
	mikaNetServer.o \
	mikaCe.o mikaCeObject.o mikaCeParcel.o mikaCeNetwork.o base64.o \
	-ImikaXml \
	-I/usr/local/include/cairo \
	-I/usr/local/include/pango-1.0 \
	-I/opt/mika/include -I/usr/local/include \
	`sdl-config --libs --cflags` \
	-lpthread -lcairo -lpangocairo-1.0 -lpango-1.0 \
	-lz -lm -lGL -lGLU -lcurl -luuid
	