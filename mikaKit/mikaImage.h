#ifndef _MIKA_IMAGE_H_
#define _MIKA_IMAGE_H_

#include "mikaCommon.h"
#include "dHandle.h"
#include "mikaFrame.h"

class mikaImage {
  public:
    dhandle lowImage;
    struct mikaFrame myFrame;
  
    mikaImage( unsigned char *filename, struct mikaFrame *frame );
    ~mikaImage( );
};

#endif
