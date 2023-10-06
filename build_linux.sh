#gcc -c db.c -O3 \
#	-I/root/zaloha/ipc/freetype/include \
#	-I/usr/local/include/freetype/freetype2 \
#	-I/root/zaloha/ipc/freetype/ft2_app/graph \
#	-I/usr/local/include/freetype2/freetype

#gcc -c xml.c \
#	-I/root/zaloha/ipc/freetype/include \
#	-I/usr/local/include/freetype/freetype2 \
#	-I/root/zaloha/ipc/freetype/ft2_app/graph \
#	-I/usr/local/include/freetype2/freetype
	
gcc -O3 -c system.c -DUNIX -DGUI -DUSEX -DJPEG -L/usr/X11R6/lib -L/usr/pkg/lib -L/usr/lib \
	-I/usr/X11R7/include -I/usr/pkg/include \
	-DUSESSL -Issl/include/openssl -O3 \
	-L/usr/local/lib -I/usr/local/include \
	-I/root/zaloha/ipc/freetype/include \
	-I/usr/local/include/freetype2/freetype \
	-I/root/zaloha/ipc/freetype/ft2_app/graph \
	-fomit-frame-pointer \
	-march=native -mtune=native
	

#gcc -c freetype/fttrop.c -DUNIX -DGUI -DUSEX -DJPEG \
#	-I/usr/X11R7/include -I/usr/pkg/include \
#	-DUSESSL -Issl/include/openssl -Lssl/lib -O3 \
#	-I/root/zaloha/ipc/freetype/include \
#	-I/usr/local/include/freetype2/freetype \
#	-I/root/zaloha/ipc/freetype/ft2_app/graph

gcc -O3 -c canvas.c -DUNIX -DGUI -DUSEX -DJPEG -L/usr/X11R6/lib -L/usr/pkg/lib -L/usr/lib \
	-I/usr/X11R7/include -I/usr/pkg/include \
	-DUSESSL -Issl/include/openssl -Lssl/lib \
	-I/usr/local/include -L/usr/local/lib \
	-I/root/zaloha/ipc/freetype/include \
	-I/root/zaloha/ipc/freetype/ft2_app/graph \
	-I/usr/local/include/freetype2/freetype -O3
	
gcc -O3 -c text.c -DUNIX -DGUI -DUSEX -DJPEG -L/usr/X11R6/lib -L/usr/pkg/lib -L/usr/lib \
	-I/usr/X11R7/include -I/usr/pkg/include \
	-DUSESSL -Issl/include/openssl -Lssl/lib \
	-I/usr/local/include -L/usr/local/lib \
	-I/root/zaloha/ipc/freetype/include \
	-I/root/zaloha/ipc/freetype/ft2_app/graph \
	-I/usr/local/include/freetype2/freetype -O3

gcc -O3 -c guirelay.c -DUNIX -DGUI -DUSEX -DJPEG -L/usr/X11R6/lib -L/usr/pkg/lib -L/usr/lib \
	-I/usr/X11R7/include -I/usr/pkg/include \
	-DUSESSL -Issl/include/openssl -Lssl/lib \
	-I/usr/local/include -L/usr/local/lib \
	-I/root/zaloha/ipc/freetype/include \
	-I/root/zaloha/ipc/freetype/ft2_app/graph \
	-I/usr/local/include/freetype2/freetype -O3
	
gcc -O3 -c guimsg.c -DUNIX -DGUI -DUSEX -DJPEG -L/usr/X11R6/lib -L/usr/pkg/lib -L/usr/lib \
	-I/usr/X11R7/include -I/usr/pkg/include \
	-DUSESSL -Issl/include/openssl -Lssl/lib \
	-I/usr/local/include -L/usr/local/lib \
	-I/root/zaloha/ipc/freetype/include \
	-I/root/zaloha/ipc/freetype/ft2_app/graph \
	-I/usr/local/include/freetype2/freetype -O3

gcc -O3 -c editable.c -DUNIX -DGUI -DUSEX -DJPEG -L/usr/X11R6/lib -L/usr/pkg/lib -L/usr/lib \
	-I/usr/X11R7/include -I/usr/pkg/include \
	-DUSESSL -Issl/include/openssl -Lssl/lib \
	-I/usr/local/include -L/usr/local/lib \
	-I/root/zaloha/ipc/freetype/include \
	-I/root/zaloha/ipc/freetype/ft2_app/graph \
	-I/usr/local/include/freetype2/freetype -O3

gcc -O3 -c keyboard.c -DUNIX -DGUI -DUSEX -DJPEG -L/usr/X11R6/lib -L/usr/pkg/lib -L/usr/lib \
	-I/usr/X11R7/include -I/usr/pkg/include \
	-DUSESSL -Issl/include/openssl -Lssl/lib \
	-I/usr/local/include -L/usr/local/lib \
	-I/root/zaloha/ipc/freetype/include \
	-I/root/zaloha/ipc/freetype/ft2_app/graph \
	-I/usr/local/include/freetype2/freetype -O3
	
gcc -O3 -c xmlsetup.c -DUNIX -DGUI -DUSEX -DJPEG -L/usr/X11R6/lib -L/usr/pkg/lib -L/usr/lib \
	-I/usr/X11R7/include -I/usr/pkg/include \
	-DUSESSL -Issl/include/openssl -Lssl/lib \
	-I/usr/local/include -L/usr/local/lib \
	-I/root/zaloha/ipc/freetype/include \
	-I/root/zaloha/ipc/freetype/ft2_app/graph \
	-I/usr/local/include/freetype2/freetype -O3

gcc -O3 -c panel.c -DUNIX -DGUI -DUSEX -DJPEG -L/usr/X11R6/lib -L/usr/pkg/lib -L/usr/lib \
	-I/usr/X11R7/include -I/usr/pkg/include \
	-DUSESSL -Issl/include/openssl -Lssl/lib \
	-I/usr/local/include -L/usr/local/lib \
	-I/root/zaloha/ipc/freetype/include \
	-I/root/zaloha/ipc/freetype/ft2_app/graph \
	-I/usr/local/include/freetype2/freetype -O3

gcc -O3 -c window.c -DUNIX -DGUI -DUSEX -DJPEG -L/usr/X11R6/lib -L/usr/pkg/lib -L/usr/lib \
	-I/usr/X11R7/include -I/usr/pkg/include \
	-DUSESSL -Issl/include/openssl -Lssl/lib \
	-I/usr/local/include -L/usr/local/lib \
	-I/root/zaloha/ipc/freetype/include \
	-I/root/zaloha/ipc/freetype/ft2_app/graph \
	-I/usr/local/include/freetype2/freetype -O3

gcc -O3 -c modules.c -DUNIX

gcc -O3 -c wm.c -DUNIX -DGUI -DUSEX -DJPEG -L/usr/X11R6/lib -L/usr/pkg/lib -L/usr/lib \
	-I/usr/X11R7/include -I/usr/pkg/include \
	-DUSESSL -Issl/include/openssl -Lssl/lib \
	-I/usr/local/include -L/usr/local/lib \
	-I/root/zaloha/ipc/freetype/include \
	-I/root/zaloha/ipc/freetype/ft2_app/graph \
	-I/usr/local/include/freetype2/freetype -O3

#gcc -O3 -c wm2.c -DUNIX -DGUI -DUSEX -DJPEG -L/usr/X11R6/lib -L/usr/pkg/lib -L/usr/lib \
#	-I/usr/X11R7/include -I/usr/pkg/include \
#	-DUSESSL -Issl/include/openssl -Lssl/lib \
#	-I/usr/local/include -L/usr/local/lib \
#	-I/root/zaloha/ipc/freetype/include \
#	-I/root/zaloha/ipc/freetype/ft2_app/graph \
#	-I/usr/local/include/freetype2/freetype -O3

#gcc -O3 -o wm2 wm2.o system.o modules.o canvas.o keyboard.o text.o editable.o \
#	xmlsetup.o xml.o ipcx.o db.o fttrop.o guirelay.o guimsg.o wm.o \
#	libjpeg.a \
#	freetype/freetype2/objs/.libs/libfreetype.so \
#	ft2_obj/common.o \
#	ft2_obj/ftcommon.o \
#	ft2_obj/gblblit.o \
#	ft2_obj/gblender.o \
#	ft2_obj/grblit.o \
#	ft2_obj/grdevice.o ft2_obj/grfill.o ft2_obj/grfont.o \
#	ft2_obj/grinit.o ft2_obj/grobjs.o ft2_obj/grswizzle.o ft2_obj/grx11.o \
#	-DUNIX -lpthread \
#	-lX11 -lXext -L/usr/X11R6/lib -L/usr/lib \
#	-lXrender -DGUI -DUSEX -DJPEG -lXpm -lm \
#	-DUSESSL -lssl -Issl/include/openssl -Lssl/lib -O3 -ltcl8.5 -ltk8.5 \
#	-lfreetype -lz -lm \
#	-fomit-frame-pointer \
#	-march=native -mtune=native

gcc -O3 -c wm2a.c -DUNIX -DGUI -DUSEX -DJPEG -L/usr/X11R6/lib -L/usr/pkg/lib -L/usr/lib \
	-I/usr/X11R7/include -I/usr/pkg/include \
	-DUSESSL -Issl/include/openssl -Lssl/lib \
	-I/usr/local/include -L/usr/local/lib \
	-I/root/zaloha/ipc/freetype/include \
	-I/root/zaloha/ipc/freetype/ft2_app/graph \
	-I/usr/local/include/freetype2/freetype -O3
	
gcc -O3 -o wm2a wm2a.o system.o modules.o canvas.o keyboard.o text.o editable.o \
	xmlsetup.o xml.o ipcx.o db.o fttrop.o guirelay.o guimsg.o window.o wm.o \
	panel.o \
	libjpeg.a \
	freetype/freetype2/objs/.libs/libfreetype.so \
	ft2_obj/common.o \
	ft2_obj/ftcommon.o \
	ft2_obj/gblblit.o \
	ft2_obj/gblender.o \
	ft2_obj/grblit.o \
	ft2_obj/grdevice.o ft2_obj/grfill.o ft2_obj/grfont.o \
	ft2_obj/grinit.o ft2_obj/grobjs.o ft2_obj/grswizzle.o ft2_obj/grx11.o \
	-DUNIX -lpthread \
	-lX11 -lXext -L/usr/X11R6/lib -L/usr/lib \
	-lXrender -DGUI -DUSEX -DJPEG -lXpm -lm \
	-DUSESSL -lssl -Issl/include/openssl -Lssl/lib -O3 -ltcl8.5 -ltk8.5 \
	-lfreetype -lz -lm \
	-fomit-frame-pointer \
	-march=native -mtune=native

#gcc -c uds.c -DUNIX -DGUI -DUSEX -DJPEG -L/usr/X11R6/lib -L/usr/pkg/lib -L/usr/lib \
#	-I/usr/X11R7/include -I/usr/pkg/include \
#	-DUSESSL -Issl/include/openssl -Lssl/lib \
#	-I/usr/local/include -L/usr/local/lib \
#	-I/root/zaloha/ipc/freetype/include \
#	-I/root/zaloha/ipc/freetype/ft2_app/graph \
#	-I/usr/local/include/freetype2/freetype -O3

#gcc -o uds uds.o system.o xml.o ipcx.o db.o fttrop.o \
#	freetype/freetype2/objs/.libs/libfreetype.so \
#	libjpeg.so \
#	ft2_obj/common.o \
#	ft2_obj/ftcommon.o \
#	ft2_obj/gblblit.o \
#	ft2_obj/gblender.o \
#	ft2_obj/grblit.o \
#	ft2_obj/grdevice.o ft2_obj/grfill.o ft2_obj/grfont.o \
#	ft2_obj/grinit.o ft2_obj/grobjs.o ft2_obj/grswizzle.o ft2_obj/grx11.o \
#	-DUNIX -lpthread \
#	-lX11 -lXext -L/usr/X11R6/lib -L/usr/lib \
#	-lXrender -DGUI -DUSEX -DJPEG -lXpm -lm \
#	-DUSESSL -lssl -Issl/include/openssl -Lssl/lib -O3 -ltcl8.5 -ltk8.5 \
#	-lfreetype -lz -lm -O3	
	
#gcc -c ipcx.c -DUNIX -DGUI -DUSEX -L/usr/X11R6/lib -L/usr/pkg/lib -L/usr/lib \
#	-I/usr/X11R7/include -I/usr/pkg/include \
#	-DUSESSL -Issl/include/openssl -Lssl/lib

#gcc -c main.c -DUNIX -DGUI -DUSEX -L/usr/X11R6/lib -L/usr/pkg/lib -L/usr/lib \
#	-I/usr/X11R7/include -I/usr/pkg/include \
#	-DUSESSL -Issl/include/openssl -Lssl/lib
#gcc -o main main.o system.o ipcx.o db.o -DUNIX -lpthread \
#	-lX11 -lXext -L/usr/X11R6/lib -L/usr/lib \
#	-I/usr/X11R6/include -I/usr/pkg/include \
#	-lXrender -DGUI -DUSEX -lXpm -lm \
#	-DUSESSL -lssl -Issl/include/openssl -Lssl/lib -O3

#gcc -c app00.c -O3 -DUNIX -DGUI -DUSEX -DJPEG -L/usr/X11R6/lib -L/usr/pkg/lib -L/usr/lib \
#	-I/usr/X11R7/include -I/usr/pkg/include \
#	-DUSESSL -Issl/include/openssl -Lssl/lib \
#	-I/usr/local/include -L/usr/local/lib \
#	-I/root/zaloha/ipc/freetype/include \
#	-I/root/zaloha/ipc/freetype/ft2_app/graph \
#	-I/usr/local/include/freetype2/freetype
	
#gcc -o app00 app00.o system.o xml.o ipcx.o db.o fttrop.o libjpeg.so \
#	freetype/freetype2/objs/.libs/libfreetype.so \
#	ft2_obj/common.o \
#	ft2_obj/ftcommon.o \
#	ft2_obj/gblblit.o \
#	ft2_obj/gblender.o \
#	ft2_obj/grblit.o \
#	ft2_obj/grdevice.o ft2_obj/grfill.o ft2_obj/grfont.o \
#	ft2_obj/grinit.o ft2_obj/grobjs.o ft2_obj/grswizzle.o ft2_obj/grx11.o \
#	-DUNIX -lpthread \
#	-lX11 -lXext -L/usr/X11R6/lib -L/usr/lib \
#	-lXrender -DGUI -DUSEX -DJPEG -lXpm -lm \
#	-DUSESSL -lssl -Issl/include/openssl -Lssl/lib -O3 -ltcl8.5 -ltk8.5 \
#	-lfreetype -lz -lm


#gcc -o mem mem.c system.o ipcx.o db.o -DUNIX -lpthread \
#	-lX11 -lXext -L/usr/X11R6/lib -L/usr/lib \
#	-I/usr/X11R6/include -I/usr/pkg/include \
#	-lXrender -DGUI -DUSEX -lXpm -lm \
#	-DUSESSL -lssl -Issl/include/openssl -Lssl/lib -O3

#gcc -c csa.c -DUNIX \
#	-L/usr/X11R6/lib -L/usr/lib \
#	-I/usr/X11R6/include -I/usr/pkg/include \
#	-DGUI -DUSEX \
#	-DUSESSL -Lssl/lib -O3	
	
#gcc -o csa csa.o system.o ipcx.o db.o -DUNIX -lpthread \
#	-lX11 -lXext -L/usr/X11R6/lib -L/usr/lib \
#	-I/usr/X11R6/include -I/usr/pkg/include \
#	-lXrender -DGUI -DUSEX -lXpm -lm \
#	-DUSESSL -lssl -Issl/include/openssl -Lssl/lib -O3

#gcc -c xml.c

#gcc -o cluster cluster.c csa.o xml.o system.o ipcx.o db.o -DUNIX -lpthread \
#	-lX11 -lXext -L/usr/X11R6/lib -L/usr/lib \
#	-I/usr/X11R6/include -I/usr/pkg/include \
#	-lXrender -DGUI -DUSEX -lXpm -lm \
#	-DUSESSL -lssl -Issl/include/openssl -Lssl/lib -O3

#gcc -o naming naming.c csa.o xml.o system.o ipcx.o db.o -DUNIX -lpthread \
#	-lX11 -lXext -L/usr/X11R6/lib -L/usr/lib \
#	-I/usr/X11R6/include -I/usr/pkg/include \
#	-lXrender -DGUI -DUSEX -lXpm -lm \
#	-DUSESSL -lssl -Issl/include/openssl -Lssl/lib -O3

#gcc -o udp udp.c csa.o xml.o system.o ipcx.o db.o -DUNIX -lpthread \
#	-lX11 -lXext -L/usr/X11R6/lib -L/usr/lib \
#	-I/usr/X11R6/include -I/usr/pkg/include \
#	-lXrender -DGUI -DUSEX -lXpm -lm \
#	-DUSESSL -lssl -Issl/include/openssl -Lssl/lib -O3
	
#gcc -o hash hash.c system.o ipcx.o db.o -DUNIX -lpthread \
#	-lX11 -lXext -L/usr/X11R6/lib -L/usr/lib \
#	-I/usr/X11R6/include -I/usr/pkg/include \
#	-lXrender -DGUI -DUSEX -lXpm -lm \
#	-DUSESSL -lssl -Issl/include/openssl -Lssl/lib -O3


