#ifndef _MIKA_WINDOW_H_
#define _MIKA_WINDOW_H_

#include "mikaCommon.h"
#include "mikaFrame.h"
#include "mikaScene.h"
#include "dHandle.h"

class mikaWindow {
  public:
    dhandle lowWindow;
    struct mikaFrame myFrame;
    mikaScene *scenes;
    
    mikaWindow( struct mikaFrame *frame );
    ~mikaWindow( );
    int eventLoop( );
    int addScene( dhandle scene );
};

#endif
