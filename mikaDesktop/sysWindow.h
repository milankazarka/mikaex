#ifndef _SYS_WINDOW_H_
#define _SYS_WINDOW_H_

#include "common.h"
#include "sysCommon.h"
#include "mikaFrame.h"
#include "mikaPoint.h"

class sysWindow {
  public:
    Display	*dpy;
    int		screen;
    Window	id;
    unsigned long valuemask;
    XSetWindowAttributes attr;
    Pixmap pixmap; // offscreen composite
    void *ximageData;
    XImage *ximage;
    cairo_surface_t *masterSurface; // master
    cairo_surface_t *surface; // slave 'surface from xcomposite'
    
    struct mikaFrame myFrame;
  
    sysWindow( struct mikaFrame *frame, sysWindow *parent );
    ~sysWindow( );
    void show( );
    void resize( struct mikaFrame *frame );
    void onExpose( );
    void onButtonDown( struct mikaPoint *point );
    void eventLoop( );
};

#endif
