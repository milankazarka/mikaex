#ifndef _MIKA_LISTBOX_H_
#define _MIKA_LISTBOX_H_

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

#define _MIKA_LISTBOX_ITEMS       8
#define _MIKA_LISTBOX_NODE_STR    128

typedef struct mikaListboxNode
{
    unsigned char name[_MIKA_LISTBOX_NODE_STR];
    unsigned char descr[_MIKA_LISTBOX_NODE_STR];
    unsigned char image[_MIKA_LISTBOX_NODE_STR];
    struct mikaListboxNode *parent;
    struct mikaListboxNode *children;
    struct mikaListboxNode *next;
} mikaListboxNode;

class mikaListboxItem {
  public:
        mikaListboxItem *next;
        mikaListboxItem *last;
        struct tr_Object *button;
    
        mikaListboxItem( );
        mikaListboxItem( struct tr_Object *canvas, struct tr_Attr *attr );
        ~mikaListboxItem( );
};

typedef struct mikaListboxDelegate
{
  // arrived to the lowest level element in tree
  void		*(*onOrder)(struct mikaListboxNode*node);
  // selected an item
  void		*(*onSelection)(struct mikaListboxNode*node);
} mikaListboxDelegate;

class mikaListbox {
    public:
        struct tr_Object *myCanvas;
        struct tr_Attr myAttr;
        
        int start; // position in list at node
        struct tr_Window *window;
        struct mikaListboxNode *mainNode;
        struct mikaListboxNode *currentNode;
        struct tr_Object *widget;
        struct tr_Object *buttons[_MIKA_LISTBOX_ITEMS];
        
        int nitems;
        mikaListboxItem *items;
        
        struct mikaListboxDelegate *delegate;
        
        mikaListbox( struct tr_Object *canvas, struct tr_Attr *attr );
        mikaListbox( struct tr_Object *canvas, struct tr_Attr *attr, int members );
        ~mikaListbox( );
        
        // setup the static parameters
        int setupStatic( );
        
        int loadNodeFile( unsigned char *filename );
        void listNodes( struct mikaListboxNode *node );
        void loadNode( struct mikaListboxNode *node );
        int loadNavigatorImages( unsigned char *buttonUp, unsigned char *buttonDown );
        
        // retrieve an item for a position in list
        tr_Object *itemAt( int pos );
        
        void update( );
        int onButton( int nbutton );
        int onListDown( );
        int onListUp( );
        int onListBack( );
};

#endif
