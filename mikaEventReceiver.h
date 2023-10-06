#ifndef _MIKA_EVENT_RECEIVER_H_
#define _MIKA_EVENT_RECEIVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mikaEvent.h"

#include "config.h"
#ifdef _TR_SYSTEM
extern "C" {
  #include "interface.h"
  #include "common.h"
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
};

#endif

class mikaEventReceiver {
  public:
    /** a pointer to an object we can identify with this receiver
     */
    void *low; // low level object
    void *idObject;
    mikaEventReceiver *next;
  
    virtual void onButtonDown( struct mikaEvent *event );
    virtual void onButtonUp( struct mikaEvent *event );
    virtual void onEvent( struct mikaEvent *event );
    
    mikaEventReceiver( );
    ~mikaEventReceiver( );
    
    void *getLow( );
};

extern mikaEventReceiver *receivers;
int mikaEventReceiverRegister( mikaEventReceiver *receiver );
int mikaEventReceiverUnregister( mikaEventReceiver *receiver );

class mikaEventReceiverTest : public mikaEventReceiver {
  public:
    mikaEventReceiverTest( );
    ~mikaEventReceiverTest( );
    
    void onButtonDown( struct mikaEvent *event );
    void onButtonUp( struct mikaEvent *event );
    void onEvent( struct mikaEvent *event );
};

/** is called with the event data
 */
void *mikaEventTransmit( void *data );
void *lowMikaEventTransmit( void *data );
/** return the Receiver that should get the event
 *
 */
mikaEventReceiver *mikaEventAssociateEventWithReceiver( struct mikaEvent *event );

//class mikaEventReceiver 

#endif
