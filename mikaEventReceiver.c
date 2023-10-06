#include "mikaEventReceiver.h"
#include "mikaWidget.h"
#include "mikaWindow.h"
#include "config.h"
#define _STATIC_NO

mikaEventReceiver *receivers = NULL;

mikaEventReceiver::mikaEventReceiver( )
{
  idObject = NULL;
  next = NULL;
  low = NULL;
  #ifdef _DEBUG
    printf("mikaEventReceiver::mikaEventReceiver\n");
  #endif
  mikaEventReceiverRegister(this);
}

void *mikaEventReceiver::getLow( )
{
  return low;
}

mikaEventReceiver::~mikaEventReceiver( )
{
  mikaEventReceiverUnregister(this);
}

void mikaEventReceiver::onButtonUp( struct mikaEvent *event )
{
}

void mikaEventReceiver::onButtonDown( struct mikaEvent *event )
{
}

void mikaEventReceiver::onEvent( struct mikaEvent *event )
{
}

int mikaEventReceiverRegister( mikaEventReceiver *receiver )
{
  #ifdef _DEBUG
    printf("mikaEventReceiverRegister\n");
  #endif
  if (!receiver)
    return 1;
  
  receiver->next = receivers;
  receivers = receiver;
  
  return 0;
}

int mikaEventReceiverUnregister( mikaEventReceiver *receiver )
{
  if (!receiver)
    return 1;
    
  mikaEventReceiver *current = receivers;
  mikaEventReceiver *next = NULL;
  mikaEventReceiver *prev = NULL;
  while(current)
  {
    next = current->next;
    if (current==receiver)
    {
    }
    current = next;
  }  
    
  return 0;
}

mikaEventReceiverTest::mikaEventReceiverTest( )
{
  #ifdef _DEBUG
    printf("mikaEventReceiverTest::mikaEventReceiverTest\n");
  #endif
}

mikaEventReceiverTest::~mikaEventReceiverTest( )
{
}

void mikaEventReceiverTest::onButtonDown( struct mikaEvent *event )
{
  #ifdef _DEBUG
    printf("mikaEventReceiverTest::onButtonDown\n");
  #endif
}

void mikaEventReceiverTest::onButtonUp( struct mikaEvent *event )
{
  #ifdef _DEBUG
    printf("mikaEventReceiverTest::onButtonUp\n");
  #endif
}

void mikaEventReceiverTest::onEvent( struct mikaEvent *event )
{
  if (!event)
    return;
  #ifdef _DEBUG
    printf("mikaEventReceiverTest::onEvent\n");
  #endif
}

/** todo - enable multiple receivers / event handlers
 *
 */
mikaEventReceiver *mikaEventAssociateEventWithReceiver( struct mikaEvent *event )
{
  #ifdef _DEBUG
    printf("mikaEventAssociateEventWithReceiver\n");
  #endif
  mikaEventReceiver *receiver = NULL;
  
  mikaEventReceiver *current = receivers;
  while(current)
  {
    struct tr_Object *to = (struct tr_Object*)current->low;
    if (event->idObject==to->widget.sub) 	// set to low level or make configurable?
                                        // is set by parent object
    {
      #ifdef _DEBUG
        printf("mikaEventAssociateEventWithReceiver hit\n");
      #endif
      return current;
    }
    current = current->next;
  }
  
  return receiver;
}

/** todo - enable multiple receivers / event handlers
 *
 */
void *mikaEventTransmit( void *data )
{
  #ifdef _DEBUG
    printf("mikaEventTransmit\n");
  #endif
  if (!data)
    return NULL;
  struct mikaEvent *event = (struct mikaEvent*)data;
  mikaEventReceiver *receiver = mikaEventAssociateEventWithReceiver(event);
  if (receiver)
  {
    // remake, so that we are transparent to widgets
    event->mobject = (mikaObject*)receiver;
    mikaWidget *widget = (mikaWidget*)receiver;
    if (widget->myData.mwindow)
    {
      mikaWindow *win = (mikaWindow*)widget->myData.mwindow;
      win->onEventNotify(event);
    }
    else
    {
      #ifdef _DEBUG
        printf("mikaEventTransmit error, widget has no valid window\n");
      #endif
    }
    
    receiver->onEvent(event);
  }
  return NULL;
}

/** bound to the low level structures sending user events
 */
void *lowMikaEventTransmit( void *data )
{
  #ifdef _DEBUG
    printf("lowMikaEventTransmit\n");
  #endif
  if (!data)
  {
    #ifdef _DEBUG
      printf("lowMikaEventTransmit error, invalid data\n");
    #endif
    return NULL;
  }
  
  struct mikaEvent *me = (struct mikaEvent*)malloc(sizeof(struct mikaEvent));
  
  me->idObject = NULL;
  #ifdef _TR_SYSTEM
    struct gui_Event *event = (struct gui_Event*)data;
    struct gui_Widget *widget = (struct gui_Widget*)event->widget;
    me->idObject = widget;
  #endif 
  
  mikaEventTransmit((void*)me);
  free(me);
  
  return NULL;
}

#ifdef _STATIC
  int main( )
  {
    void *obj = malloc(1);
  
    struct mikaEvent event;
    event.idObject = obj;
    event.x = 0;
    event.y = 0;
    
    mikaEventReceiverTest *test = new mikaEventReceiverTest();
    test->low = &test; // just test purposes
    event.idObject = test->low;
    mikaEventReceiver *receiver = test;
    receiver->onButtonDown(&event);
    mikaEventTransmit(&event);
    return 0;
  }
#endif

