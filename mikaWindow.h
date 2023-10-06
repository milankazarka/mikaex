#ifndef _MIKA_WINDOW_H_
#define _MIKA_WINDOW_H_

extern "C" {
  #include "interface.h"
  #include "common.h"
  #include "config.h"
  #include "os.h"
  #include "system.h"
  #include "systemip.h"
  #include "xml.h"
  #include "wm.h"
  #include "dom.h"
  #include "activityObject.h"
  #include "rootControllerSync.h"
  #include "basic.h"
  #include "mikaPosClient.h"
}
#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"
#include "mikaWidget.h"
#include "mikaCanvas.h"
#include "mikaObject.h"
#include "mikaEvent.h"

class mikaWindow : public mikaObject {
  public:
    mikaWindow 		*myParent;
    #ifdef _TR_SYSTEM
      struct tr_Window 	*window;
      struct tr_Object 	*canvas;
    #endif
    mikaCanvas *mcanvas; // top level canvas
  
    mikaWindow( mikaWindow *parent, struct tr_Attr *attr );
    ~mikaWindow( );

    int addWidget( mikaWidget *widget );
    // push this window to forefront and expose
    int push( );
    
    virtual void onEventNotify( struct mikaEvent *event );
};

#endif
