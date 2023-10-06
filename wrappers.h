#ifndef _WRAPPERS_H_
#define _WRAPPERS_H_

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

int wrapperInitApp( );

#endif
