#ifndef _MIKA_INPUT_FIELD_H_
#define _MIKA_INPUT_FIELD_H_

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

class mikaInputField {
    public:
        mikaInputField( );
        ~mikaInputField( );
};

#endif
