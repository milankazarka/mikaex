#ifndef _LOW_DESKTOP_H_
#define _LOW_DESKTOP_H_

#include "sysCommon.h"
#include "common.h"

class sysDesktop {
  public:
    Display *dpy;
    int screen;
    Window root;
    XWindowAttributes attr;
  
    sysDesktop( );
    ~sysDesktop( );
};

#endif
