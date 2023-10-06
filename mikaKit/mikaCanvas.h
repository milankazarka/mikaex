#ifndef _MIKA_CANVAS_H_
#define _MIKA_CANVAS_H_

#include "mikaCommon.h"
#include "dHandle.h"

enum _CANVAS_MOVEMENT_TYPES {
  _CANVAS_MOVEMENT_TYPE_NONE,
  _CANVAS_MOVEMENT_TYPE_SCROLL
};

class mikaCanvas {
  public:
    dhandle lowCanvas;
    int myMovementType;
    
    mikaCanvas( dhandle scene );
    ~mikaCanvas( );
    int setMovement( int movementType );
    int setColor( unsigned char *colorCode );
    
    int addWidget( dhandle widget );
};

#endif
