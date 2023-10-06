#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "object.h"
#include "mikaGl/mikaGlImage.h"

class milanImage : public object {
  public:
    mikaGlImage *glImage;
    int mx, my;
  
    milanImage( unsigned char *filename );
    ~milanImage( );
};

#endif
