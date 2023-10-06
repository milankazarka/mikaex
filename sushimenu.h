#ifndef _SUSHIMENU_H_
#define _SUSHIMENU_H_

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
  #include "global.h"
}
#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"
//#include "mikaPanel.h"
#include "mikaList.h"

#define _APP_SIZE_W 480
#define _APP_SIZE_H 480

class sushiMenuBill {
  public:
    struct tr_Window	*window;
    struct tr_Object	*canvas;
    struct tr_Attr	attr;
    
    struct tr_Object	*info;
    
    sushiMenuBill( struct tr_Window *parentWindow );
    ~sushiMenuBill( );
    
    int raise( );
};

class sushiMenuOrder {
    public:
      struct tr_Window	        *window;
      struct tr_Object	        *canvas;
      struct tr_Attr		attr;
      
      struct tr_Object          *infoText;
      struct tr_Object          *infoButton;
      struct tr_Object          *order;
      struct tr_Object          *cancel;
      
      TiXmlElement              *currentElement;
      
      sushiMenuOrder( struct tr_Window *parentWindow );
      ~sushiMenuOrder( );
      
      int raise( );
};

class sushimenu {
    public:
        struct tr_Window	*root;
        struct tr_Window	*wmain;
        struct tr_Object	*cmain;
        struct tr_Attr		mainAttr;
        
        mikaList                *list;
        struct mikaListDelegate listdelegate;
        
        sushiMenuOrder          *menuOrder;
        sushiMenuBill		*menuBill;
        
        sushimenu( );
        ~sushimenu( );
        
        int onListNode( TiXmlElement *element, int position );
        int raise( );
};

#endif
