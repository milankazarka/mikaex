#ifndef _MIKA_ROOT_WINDOW_H_
#define _MIKA_ROOT_WINDOW_H_

/** this should in end effect replace the old system
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cairo/cairo.h>

typedef struct rootFrame
{
  int x;
  int y;
  int w;
  int h;
} rootFrame;

class mikaRootWindow {
  public:
    struct rootFrame frame;
  
    mikaRootWindow( );
    mikaRootWindow( struct rootFrame *_frame );
    ~mikaRootWindow( );
};

#endif
