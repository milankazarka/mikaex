#ifndef _MIKA_DESKTOP_H_
#define _MIKA_DESKTOP_H_

#include "common.h"
#include "mikaFrame.h"
#include "mikaWindow.h"
#include "sysDesktop.h"

class mikaDesktop {
  public:
    struct mikaFrame myFrame;
    mikaWindow *windows;
    sysDesktop *sys;
  
    mikaDesktop( struct mikaFrame *frame );
    ~mikaDesktop( );
    
    int addWindow( mikaWindow *window );
};

extern mikaDesktop *desktop;

#endif
