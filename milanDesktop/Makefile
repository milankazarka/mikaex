CC = gcc
CCO = g++

CFLAGS = -c -DUNIX -DGUI -DUSEX -DJPEG \
	-g -O2 \
	-DUSESSL -I/ssl/include/openssl -Lssl/lib \
	-Ifreetype/include \
	-I/usr/include \
	-Ifreetype/ft2_app/graph \
	-I/usr/local/lib/gtk-2.0/include -I/usr/local/include/atk-1.0 \
	-I/usr/local/include/cairo \
	-I/usr/local/include/pango-1.0 \
	-I/usr/local/include/glib-2.0 -I/usr/local/lib/glib-2.0/include \
	-I/usr/local/include/pixman-1 -I/usr/local/include/freetype2 \
	-I/usr/local/include -I/usr/local/include/webkit-1.0 \
	-I/usr/include/libpng12 \
	-I/opt/mika/include -L/opt/mika/lib \
	-I/opt/mika/gtk-2.0
	
LDFLAGS = -pthread \
	-lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 \
	-lgdk_pixbuf-2.0 -lpangocairo-1.0 -lcairo -lpango-1.0 \
	-lfontconfig -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lglib-2.0 \
	-lpthread -lX11 -lXext \
	-lXrender -DGUI -DUSEX -DUNIX -DJPEG -lXpm \
	-DUSESSL -ltcl8.5 -ltk8.5 \
	-lz -lm -lcrypto -lcurl -lnxml -lcairo

desktop :
	$(CCO) $(LDFLAGS) \
	desktop.o widget.o canvas.o object.o window.o \
	image.o text.o \
	mikaGl/mikaGl.a -o desktop

desktop.o : desktop.c
	$(CCO) $(CFLAGS) -c desktop.c

canvas.o : canvas.c
	$(CCO) $(CFLAGS) -c canvas.c

widget.o : widget.c
	$(CCO) $(CFLAGS) -c widget.c

text.o : text.c
	$(CCO) $(CFLAGS) -c text.c

image.o : image.c
	$(CCO) $(CFLAGS) -c image.c

object.o : object.c
	$(CCO) $(CFLAGS) -c object.c

window.o : window.c
	$(CCO) $(CFLAGS) -c window.c

bridge.o : bridge.c
	$(CCO) $(CFLAGS) -c bridge.c
	

	