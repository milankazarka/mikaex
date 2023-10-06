#!/bin/bash
g++ -c mikaHttp.c
g++ -c xipc.c
g++ -pthread -lpthread -lcurl -I/usr/local/include/gtk-2.0 \
	mikaHttp.o xipc.o \
	-I/usr/local/lib/gtk-2.0/include -I/usr/local/include/atk-1.0 -I/usr/local/include/cairo -I/usr/local/include/pango-1.0 \
	-I/usr/local/include/glib-2.0 -I/usr/local/lib/glib-2.0/include -I/usr/local/include/pixman-1 -I/usr/local/include/freetype2 \
	-I/usr/local/include -I/usr/local/include/webkit-1.0 -I/usr/include/libpng12  -pthread -L/usr/local/lib \
	-lwebkit-1.0 -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lgdk_pixbuf-2.0 -lpangocairo-1.0 \
	-lcairo -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lglib-2.0 \
	-o webkitwindow webkitwindow.c -I/usr/local/include/webkit-1.0 -I/usr/local/include/libsoup
