#include "debug.h"

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

#include "wrappers.h"
#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"

#define _APP_SIZE_W 480
#define _APP_SIZE_H 320

int main( )
{
    #ifdef _DEBUG
        printf("debug\n");
    #endif
    wrapperInitApp();
    return 0;
}
