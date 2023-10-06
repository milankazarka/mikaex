/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "rootEvents.h"
#include "rootScheduller.h"
#include "rootControllerSync.h"

struct rootScheduller *eventsScheduller = NULL;

void rootEventsLowEventLoop( struct rootEvents *events )
{
  #ifdef _DEBUG
    printf("rootEventsLowEventLoop\n");
  #endif
  if (!events) return;
  if (!events->window) return;
  XEvent event;
  while(1)
  {
    XNextEvent(events->window->dpy,&event);
    switch(event.type)
    {
      case ButtonPress:
        rootEventsEventButtonDown(events,event.xbutton.x,event.xbutton.y);
      break;
      case ButtonRelease:
        rootEventsEventButtonUp(events,event.xbutton.x,event.xbutton.y);
      break;
      case MotionNotify:
        rootEventsEventButtonMotion(events,event.xbutton.x,event.xbutton.y);
      break;
      case Expose:
        rootEventsEventExpose(events);
      break;
    }
  }
}

struct rootEvents *rootEventsInit( struct rootEvents *events )
{
  if (!events)
    events = (struct rootEvents*)malloc(sizeof(struct rootEvents));
  
  eventsScheduller = rootSchedullerNew();
  
  events->window = NULL;
  
  int n;
  for(n = 0; n < _ROOT_EVENTS_BUFFER; n++)
  {
    events->current[n].active = 0;
    events->current[n].timestamp = 0;
    events->current[n].type = _ROOT_EVENT_NONE;
  }
  
  return events;
}

int rootEventsMerge( struct rootEvents *events )
{
  #ifdef _ROOT_EVENTS_MERGE
    return 1;
  #else
    return 0;
  #endif
}

struct rootEvent *rootEventsGetFreeEvent( struct rootEvents *events )
{
  static struct rootEvent *event = NULL;
  event = &events->current[0];
  return event;
}

/** this is a merge of button down, button up & motion event. In case
 *  of this event, the rootScheduller decides the type of event we are
 *  handling. This is usefull when using buggy / less / more sensitive
 *  touch sensors when we could get button up events while doing a motion.
 *
 */
int rootEventsEventCursor( struct rootEvents *events, int x, int y )
{
  #ifdef _DEBUG
    printf("rootEventsEventCursor\n");
  #endif
  static struct rootEvent *event = NULL;
  if (!events) return 1;
  
  rootControllerLock();
  event = rootEventsGetFreeEvent(events);
  event->active = 1;
  event->timestamp = schedullerTime();
  event->cursor.position.x = x;
  event->cursor.position.y = y;
  event->type = _ROOT_EVENT_CURSOR;
  rootControllerUnlock();
  
  rootSchedullerCursor(eventsScheduller,x,y);
  
  return 0;
}

int rootEventsEventButtonDown( struct rootEvents *events, int x, int y )
{
  #ifdef _DEBUG
    printf("rootEventsEventButtonDown\n");
  #endif
  static struct rootEvent *event = NULL;
  if (!events) return 1;
  if (rootEventsMerge(events)==1)
    rootEventsEventCursor(events,x,y);
  else
  {
    rootControllerLock();
    event = rootEventsGetFreeEvent(events);
    event->active = 1;
    event->timestamp = schedullerTime();
    event->button.position.x = x;
    event->button.position.y = y;
    event->type = _ROOT_EVENT_BUTTONDOWN;
    rootControllerUnlock();
  }
  return 0;
}

int rootEventsEventButtonUp( struct rootEvents *events, int x, int y )
{
  #ifdef _DEBUG
    printf("rootEventsEventButtonUp\n");
  #endif
  static struct rootEvent *event = NULL;
  if (!events) return 1;
  if (rootEventsMerge(events)==1)
    rootEventsEventCursor(events,x,y);
  else
  {
    rootControllerLock();
    event = rootEventsGetFreeEvent(events);
    event->active = 1;
    event->timestamp = schedullerTime();
    event->button.position.x = x;
    event->button.position.y = y;
    event->type = _ROOT_EVENT_BUTTONUP;
    rootControllerUnlock();
  }
  return 0;
}

int rootEventsEventButtonMotion( struct rootEvents *events, int x, int y ) // rename to motion
{
  #ifdef _DEBUG
    printf("rootEventsEventButtonMotion\n");
  #endif
  static struct rootEvent *event = NULL;
  if (!events) return 1;
  if (rootEventsMerge(events)==1)
    rootEventsEventCursor(events,x,y);
  else
  {
    rootControllerLock();
    event = rootEventsGetFreeEvent(events);
    event->active = 1;
    event->timestamp = schedullerTime();
    event->motion.position.x = x;
    event->motion.position.y = y;
    event->type = _ROOT_EVENT_MOTION;
    rootControllerUnlock();
  }
  return 0;
}

int rootEventsEventExpose( struct rootEvents *events )
{
  #ifdef _DEBUG
    printf("rootEventsEventExpose\n");
  #endif
  if (!events) return 1;
  return 0;
}
