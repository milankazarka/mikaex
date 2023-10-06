#ifndef _MIKA_WINDOW_H_
#define _MIKA_WINDOW_H_

#include "common.h"
#include "mikaFrame.h"
#include "sysWindow.h"

class mikaWindow {
  public:
    struct mikaFrame myFrame;
    mikaWindow *myParent;
    mikaWindow *next;
    sysWindow *sys;
  
    mikaWindow( struct mikaFrame *frame, mikaWindow *parent );
    ~mikaWindow( );
    void show( );
    void eventLoop( );
};

#endif
