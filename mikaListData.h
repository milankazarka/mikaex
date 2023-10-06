#ifndef _MIKA_LIST_DATA_H_
#define _MIKA_LIST_DATA_H_

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

struct mikaListDataDelegate {
    void *holder; // identifier of holder
    void *(*onNodeElement)( struct mikaListDataDelegate *delegate, TiXmlElement *element, int position );
    /** we have clicked on an element without children
     */
    void *(*onLowestElement)( struct mikaListDataDelegate *delegate, TiXmlElement *element, int position );
};

class mikaListData {
    public:
        TiXmlDocument *doc;
        TiXmlElement *currentNode;
        struct mikaListDataDelegate *delegate;
        
        mikaListData( unsigned char *dataFile );
        ~mikaListData( );
        
        int back( );
        /** retrieve the element at a position <node>
         */
        TiXmlElement *elementAtPosition( int pos );
        /** retrieve the element's <nodes> of element at position
         */
        TiXmlElement *elementAtPositionNodes( int pos );
        int moveToElementAtPosition( int pos );
        int listCurrent( );
        int delegateNode( );
        /** number of items in current node
         */
        int numberOfItems( );
};

#endif
