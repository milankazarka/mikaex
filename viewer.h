#ifndef _VIEWER_H_
#define _VIEWER_H_

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
  #include "mikaEventReceiver.h"
  #include "global.h"
}

#include "mikaWidget.h"
#include "mikaSignageWidget.h"
#include "mikaGalleryWidget.h"
#include "mikaVideoWidget.h"
#include "mikaRotateWidget.h"
#include "mikaWindow.h"
#include "mikaClockWidget.h"
#include "mikaList.h"

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "listener.h"

#define _APP_W 940
#define _APP_H 690
#define _INIT_ROTATION 0

class viewer {
public:
    mikaWindow *root;
    mikaWindow *window;    
    
    viewer( );
    ~viewer( );
    
    void setImage( int nid );
};

#endif
