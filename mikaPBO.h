#ifndef _MIKA_PBO_H_
#define _MIKA_PBO_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <cairo/cairo.h>
#include "mikaFrame.h"

enum _MIKA_PBO_EVENTS
{
  _MIKA_PBO_EVENT_NONE,
  _MIKA_PBO_EVENT_EXPOSE
};

typedef struct mikaPBOEvent
{
  unsigned long timestamp;
  int type;
} mikaPBOEvent;

typedef struct mikaPBODelegate
{
  void *(*onUpdate)(void*pbo);
} mikaPBODelegate;

class mikaPBO {
  public:
    struct mikaPBOEvent event;
    struct mikaPBODelegate delegate;
    void *pixels;
    cairo_surface_t *cairo_surface;
    struct mikaFrame frame;
    unsigned char clearChar;
    
    mikaPBO( );
    mikaPBO( struct mikaFrame *_frame );
    mikaPBO( struct mikaFrame *_frame, void *_pixels );
    ~mikaPBO( );
    void update( void *_pixels );
    void clear( );
};

typedef struct mikaPBOHolder
{
  void *pbo;
} mikaPBOHolder;

#endif
