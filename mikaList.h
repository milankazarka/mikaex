#ifndef _MIKA_LIST_H_
#define _MIKA_LIST_H_

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
#include "mikaListItem.h"
#include "mikaListData.h"

class mikaListItem {
    public:
        struct tr_Object *button;
        mikaListItem *next;
        
        mikaListItem( struct tr_Object *canvas, struct tr_Attr *attr );
        ~mikaListItem( );
};

/** receiving events from mikaList and delegating them to wanted methods
 *
 */
struct mikaListDelegate {
    void *(*onNodeElement)( TiXmlElement *element, int position );
    void *(*onSelection)( int position );
    void *(*onLowest)( void *sender, TiXmlElement *element, int position );
};

class mikaList {
    public:
        int nitems;
        mikaListItem *items;
        mikaListData *data;
        struct tr_Attr myAttr;
        struct tr_Object *myCanvas;
        struct tr_Window *myWindow;
        struct tr_Object *widget;
        // the mikaListData sends us messages:
        struct mikaListDataDelegate dataDelegate;
        struct mikaListDelegate *delegate;
        int position;
        
        mikaList( struct tr_Object *canvas, struct tr_Attr *attr, int members );
        ~mikaList( );
        
        int loadFile( unsigned char *filename );
        int generateButtons( );
        int onButton( struct gui_Event *event );
        
        int moveUp( );
        int moveDown( );
        int moveBack( );
        
        int update( );
};

#endif
