#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "window.h"
#include "object.h"
#include "mikaGl/mikaGlWindow.h"

class milanWindow : public object {
  public:
    int mw;
    int mh;
    mikaGlWindow *glWindow;
    struct mikaGlWindowDelegate glWindowDelegate;
    milanWindow *next;
    object *canvasses;
    object *currentCanvas;
  
    milanWindow( int w, int h );
    ~milanWindow( );
    
    int triggerExpose( );
    int onButtonDown( struct eventButton *event );
    int onButtonUp( struct eventButton *event );
    int eventLoop( );
    
    int addCanvas( object *nc );
    int pushCanvas( object *c );
};

void *onButtonDownShared( void *windowData, struct eventButton *event );
void *onButtonUpShared( void *windowData, struct eventButton *event );

extern milanWindow *windows;

#endif
