#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "object.h"
#include "mikaGl/mikaGlWindow.h"

class milanCanvasObject : public object {
  public:
    object *myWidget;
    milanCanvasObject *next;
    
    milanCanvasObject( object *widget );
    ~milanCanvasObject( );
};

class milanCanvas : public object {
  public:
    object *myParent;
    object *canvasWidget; // widget setup
    milanCanvas *next;
    unsigned char name[256];
    milanCanvasObject *objects;
  
    milanCanvas( object *parent ); // window as parent
    ~milanCanvas( );
    
    int raise( );
    int triggerExpose( );
    int onButtonDown( struct eventButton *button );
    int onButtonUp( struct eventButton *button );
    int addWidget( object *widget );
    int isEventWidget( struct milanCanvasObject *co, struct eventButton *button );
};

#endif
