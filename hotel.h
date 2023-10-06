#ifndef _HOTEL_H_
#define _HOTEL_H_

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
#include "mikaPanel.h"
#include "mikaListbox.h"

class hotelPoster {
    public:
        hotelPoster( struct tr_Window *parentWindow );
        ~hotelPoster( );
};

class hotelMenu {
    public:
        struct tr_Attr		attr;
        struct tr_Window        *window;
        struct tr_Object        *canvas;
        
        mikaListbox                 *listbox;
        mikaPanel		    *panel;
        struct mikaListboxDelegate  delegate;
        
        struct tr_Object	*title;
        struct tr_Object	*titleBg;
        struct tr_Object	*poster;
        struct tr_Object	*posterBg;
        
        hotelMenu( struct tr_Window *parentWindow );
        ~hotelMenu( );
        
        int raise( );
};

class hotel {
    public:
        struct tr_Window	*root;
        struct tr_Window	*wmain;
        struct tr_Object	*cmain;
        struct tr_Attr		mainAttr;
        
        hotelMenu               *hMenu;
        
        hotel( );
        ~hotel( );
};

#endif
