#ifndef _MIKA_WIDGET_H_
#define _MIKA_WIDGET_H_

#include "mikaCommon.h"
#include "mikaFrame.h"
#include "dHandle.h"

class mikaWidget {
  public:
    dhandle lowWidget;
    dhandle parentCanvas;
    struct mikaFrame myFrame;
  
    mikaWidget( dhandle canvas, struct mikaFrame *frame );
    ~mikaWidget( );
    int setColor( unsigned char *colorCode );
    int addImage( dhandle image );
};

#endif
