#ifndef _MIKA_GL_SURFACE_H_
#define _MIKA_GL_SURFACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class mikaGlSurface {
  public:
    unsigned char *pixels;
    int mw;
    int mh;
    
    mikaGlSurface( int w, int h );
    ~mikaGlSurface( );
};

#endif
