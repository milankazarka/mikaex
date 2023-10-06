#ifndef _MIKA_GL_IMAGE_H_
#define _MIKA_GL_IMAGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libJpeg/jpeglib.h"
#include "libJpeg/jpegint.h"
#include "libJpeg/jerror.h"
#include "libJpeg/jconfig.h"

class mikaGlImage {
  public:
    int mw;
    int mh;
    unsigned char *pixels;
  
    mikaGlImage( unsigned char *filename );
    ~mikaGlImage( );
};

#endif
