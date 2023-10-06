#ifndef _MIKA_WM_H_
#define _MIKA_WM_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "sysCommon.h"

class mikaWMClient {
  public:
    Window parent;
    Window myWindow;
    mikaWMClient *next;
    
    /** initialize passing the Window of the application
     *  that needs to use the WM
     */
    mikaWMClient( Window window );
    ~mikaWMClient( );
};

class mikaWMRootWindow {
  public:
    XWindowAttributes attr;
    mikaWMRootWindow( );
    ~mikaWMRootWindow( );
};

class mikaWM {
  public:
    Display *dpy;
    int screen;
    Window root;
    mikaWMRootWindow *wmRoot;
    mikaWMClient *clients;
  
    mikaWM( );
    ~mikaWM( );
    
    int initEnvironment( );
    void eventLoop( );
    int addClient( Window window );
    
    mikaWMClient *clientOfWindowByParent( Window parent );
    mikaWMClient *clientOfWindow( Window window );
    
    int eventCreateNotify( XEvent *ev );
    int eventExpose( XEvent *ev );
    int eventConfigureRequest( XEvent *ev );
};

#endif
