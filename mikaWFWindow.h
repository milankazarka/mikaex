#ifndef _MIKA_WF_WINDOW_H_
#define _MIKA_WF_WINDOW_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mikaFrame.h"

enum _MIKA_WF_WINDOW_TYPES {
  _MIKA_WF_WINDOW_TYPE_MAIN,
  _MIKA_WF_WINDOW_TYPE_SCENE
};

typedef struct lowMikaWFWindow {
  
} lowMikaWFWindow;

class mikaWFWindow {
  public:
    int type;
    struct mikaFrame frame;
    mikaWFWindow *parent;
  
    mikaWFWindow( mikaWFWindow *_parent, struct mikaFrame *_frame );
    ~mikaWFWindow( );
    void moveToTop( );
};

#endif
