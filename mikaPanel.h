#ifndef _MIKA_PANEL_H_
#define _MIKA_PANEL_H

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

class mikaPanelItem {
    public:
      mikaPanelItem 	*next;
      struct tr_Object 	*widget;
      void 		*(*onbutton)(void*data);
      
      mikaPanelItem( struct tr_Object *canvas, struct tr_Attr *attr );
      ~mikaPanelItem( );
      
      int setName( unsigned char *name );
};

class mikaPanel {
    public:
        struct tr_Object *widget;
        struct mikaPanelItem *items;
        
        mikaPanel( struct tr_Object *canvas, struct tr_Attr *attr );
        ~mikaPanel( );
        
        int addItem( mikaPanelItem *item );
};

#endif
