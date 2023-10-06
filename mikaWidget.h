#ifndef _MIKA_WIDGET_H_
#define _MIKA_WIDGET_H_
#include "mikaEventReceiver.h"
#include "mikaWidgetDelegate.h"

#include "mikaSkinEx.h"
#include "mikaObject.h"

struct mikaWidgetData {
  struct tr_Attr attr; // position & size
  mikaObject *mwindow;
  #ifdef _TR_SYSTEM
    struct tr_Window *window;
    struct tr_Object *canvas;
    struct tr_Object *widget;
  #endif
};

class mikaWidget : public mikaObject, mikaEventReceiver {
  public:
    mikaWidget *myParent;
    struct mikaWidgetData myData;
    mikaWidget *children;
    mikaWidget *next;
    
    /** data will be copied to myData
     */
    mikaWidget( struct mikaWidgetData *data );
    mikaWidget( struct mikaWidgetData *data, mikaWidget *parent );
    ~mikaWidget( );
    
    int setup( unsigned char *name );
    
    void notifyWindow( struct mikaEvent *event ); // notify event of event using mikaWidgetDelegate
    void onEvent( struct mikaEvent *event );
    void onButtonDown( struct mikaEvent *event );
    void onButtonUp( struct mikaEvent *event );
    
    int addWidget( mikaWidget *widget );
    // place widget onto the canvas
    int placeWidget( );
    
    void *getLow( );
};

class mikaWidgetTest : public mikaWidget, mikaWidgetDelegate {
  public:
    mikaWidgetTest( struct mikaWidgetData *data );
    ~mikaWidgetTest( );
    
    /** general widget delegate
     */
    void onWidgetDelegate( struct delegateData *data );
};

/** create the data structure to be held by mikaWidget - created out of low level objects
 *
 */
#ifdef _TR_SYSTEM
  struct mikaWidgetData *mikaWidgetDataNew( struct tr_Window *window, struct tr_Object *canvas, struct tr_Object *widget );
#endif

struct mikaWidgetData *mikaWidgetDataNewEx( mikaObject *window, struct tr_Attr *attr );

#endif
