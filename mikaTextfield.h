#ifndef _MIKA_TEXTFIELD_H_
#define _MIKA_TEXTFIELD_H_

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
}
#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"

class mikaLine {
  public:
    mikaLine *next;
    struct tr_Object *widget;
    
    mikaLine( struct tr_Object *canvas, struct tr_Object *parent, struct tr_Attr *attr );
    ~mikaLine( );
};

class mikaTextfield {
  public:
    struct tr_Object *widget;
    struct tr_Object *myParent;
    int myLineheight;
    mikaLine *lines;
    int mynlines;
  
    mikaTextfield( struct tr_Object *canvas, struct tr_Object *parent, int lineheight );
    ~mikaTextfield( );
    
    int clear( );
    int addLine( mikaLine *line );
    int fillLine( int nline, unsigned char *text );
    int setText( unsigned char *text );
};

#endif
