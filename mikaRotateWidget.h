#ifndef _MIKA_ROTATE_WIDGET_H_
#define _MIKA_ROTATE_WIDGET_H_

#include "mikaWidget.h"
#include "config.h"

#define _ROTATE_NONE 361

class mikaRotateWidget : public mikaWidget {
  public:
    int predefined;
    int current;
    int flip; // 0/1 flip upside down
  
    mikaRotateWidget( struct mikaWidgetData *data );
    mikaRotateWidget( struct mikaWidgetData *data, int angle );
    ~mikaRotateWidget( );
    
    void onEvent( struct mikaEvent *event );
    void onButtonDown( struct mikaEvent *event );
    void onButtonUp( struct mikaEvent *event );
    
    int rotateLeft( );
    int rotateRight( );
    int rotateUp( );
    int rotateDown( );
    // rotate to predefined value
    int rotateDefined( );
    
    int update( );
};

#endif
