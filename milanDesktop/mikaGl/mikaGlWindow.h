#ifndef _MIKA_GL_WINDOW_H_
#define _MIKA_GL_WINDOW_H_

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/utsname.h>
#include <semaphore.h>

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/xpm.h>

#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>

#include "mikaGlSurface.h"
#include "mikaGl.h"

typedef struct lowWindow {
  Display		            *dpy;
  int			            screen;
  Window		            id;
    
  GC		                    *gc;
  XGCValues		            *gcv;

  Pixmap		            p;
  Pixmap		            m;
  XImage		            *surface; // dummy
};

struct lowWindow *lowWindowInit( int w, int h );

typedef struct eventButton {
  int 	x;
  int 	y;
  int 	id;
};

typedef struct mikaGlWindowDelegate {
  void		*(*onButtonDown)( void *windowData, struct eventButton *event );
  void		*(*onButtonRelease)( void *windowData, struct eventButton *event );
  void		*(*onEventExpose)( void *windowData );
};

class mikaGlWindow {
  public:
    int mw;
    int mh;
    mikaGlSurface *surface;
    struct lowWindow *window;
    mikaGl *gl;
    struct mikaGlWindowDelegate *delegate;
  
    mikaGlWindow( int w, int h );
    ~mikaGlWindow( );
    
    int wait( );
    int eventLoop( );
    int eventLoopThreaded( );
    
    int triggerExpose( );
    int eventExpose( );
    int eventButtonDown( int x, int y, int id );
    int eventButtonRelease( int x, int y, int id );
};

void *eventLoopThread(void*data);

#endif
