#ifndef _WIDGET_H_
#define _WIDGET_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "object.h"
#include "text.h"
#include "image.h"
#include "window.h"
#include "mikaGl/mikaGlWindow.h"

enum _WIDGET_TYPES {
  _WIDGET_TYPE_TEXT,
  _WIDGET_TYPE_COLOR,
  _WIDGET_TYPE_IMAGE
};

typedef struct milanWidgetObject {
  int type;
  object *data;
  struct milanWidgetObject *next;
};

typedef struct milanWidgetDelegate {
  void	*(*onButtonDown)( object *widget, struct eventButton *event );
  void	*(*onButtonUp)( object *widget, struct eventButton *event );
};

class milanWidget {
  public:
    int mx, my, mw, mh, round;
    struct milanWidgetObject *objects;
    unsigned char myColor[4];
    struct milanWidgetDelegate *delegate;
  
    milanWidget( int x, int y, int w, int h );
    ~milanWidget( );
    
    int redraw( );
    int onButtonDown( struct eventButton *event );
    int onButtonUp( struct eventButton *event );
    int addText( milanText *text );
    int addImage( milanImage *image );
    int setRound( int r );
    int setColor( unsigned char r, unsigned char g, unsigned char b, unsigned char a );
    int drawOnWindow( milanWindow *window );
};

#endif
