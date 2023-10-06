#ifndef _MIKA_SKIN_EX_H_
#define _MIKA_SKIN_EX_H_

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

class mikaSkinEx {
  public:
    TiXmlDocument *doc;
    
    mikaSkinEx( );
    ~mikaSkinEx( );
    
    int setupWidget( tr_Object *w, unsigned char *name );
};

extern mikaSkinEx *globalSkin;
mikaSkinEx *getSkin( );

#endif
