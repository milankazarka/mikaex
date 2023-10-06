#ifndef _MENU_SETUP_H_
#define _MENU_SETUP_H_

extern "C" {
  #include "interface.h"
  #include "config.h"
  #include "os.h"
  #include "system.h"
  #include "global.h"
  #include "xml.h"
  #include "wm.h"
  #include "dom.h"
  #include "activityObject.h"
  #include "rootControllerSync.h"
  #include "connectionWatchdog.h"
  #include "networkSetup.h"
}
#include "setupParser.h"

typedef struct menuSetup
{
  struct networkSetup	*setup;
  struct setupParser	*parser;
} menuSetup;

struct menuSetup *menuSetupInit( struct menuSetup *ms );

#endif
