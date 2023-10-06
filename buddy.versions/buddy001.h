#ifndef _BUDDY_H_
#define _BUDDY_H_

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

#define _APP_W 640
#define _APP_H 640

class buddyPos {
    public:
        mikaWindow *window;
        
        buddyPos( mikaWindow *root );
        ~buddyPos( );
};

class buddyGallery {
    public:
        mikaWindow *window;
        
        buddyGallery( mikaWindow *root );
        ~buddyGallery( );
};

class buddy {
    public:
        mikaWindow *root;
        mikaWindow *window;
        mikaSignageWidget *signage;
        mikaGalleryWidget *gallery;
        mikaVideoWidget *video;
        mikaRotateWidget *rotate;
        mikaClockWidget *clock;
    
        buddyPos *pos;
    
        buddy( );
        ~buddy( );
};

class buddyWidget : public mikaWidget {
    public:
    
        buddyWidget( struct mikaWidgetData *data );
        ~buddyWidget( );
        
        void onButtonDown( struct mikaEvent *event );
        void onButtonUp( struct mikaEvent *event );
        void onEvent( struct mikaEvent *event );
};

extern buddy *gbuddy;

#endif
