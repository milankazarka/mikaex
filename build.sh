gcc -c system.c -DUNIX -DUSEX -L/usr/X11R7/lib -L/usr/pkg/lib -L/usr/lib \
	-I/usr/X11R7/include -I/usr/pkg/include -R/usr/lib
gcc -c ipcx.c -DUNIX -DUSEX -L/usr/X11R7/lib -L/usr/pkg/lib -L/usr/lib \
	-I/usr/X11R7/include -I/usr/pkg/include -R/usr/lib
gcc -c main.c -DUNIX -DUSEX -L/usr/X11R7/lib -L/usr/pkg/lib -L/usr/lib \
	-I/usr/X11R7/include -I/usr/pkg/include
gcc -o main main.o system.o ipcx.o -DUNIX -lpthread \
	-lX11 -lXext -L/usr/X11R7/lib -L/usr/pkg/lib -L/usr/lib \
	-I/usr/X11R7/include -I/usr/pkg/include \
	-lXrender -R/usr/X11R7/lib -DUSEX -lXpm
