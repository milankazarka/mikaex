/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

/**
 * this is meant to schedulle events according to some speciffic
 * settings. This is usefull when using some public space touch
 * sensors or creating different usability patterns than expected
 * on a Desktop system
 *
 * NOTE: do not synchronize the root controller in the scheduller
 * only the activity access and the scheduller itself
 */
 
#include "rootScheduller.h"
#include "rootController.h"
#include "rootPointer.h"
#include "config.h"
#include "activityObject.h"
#include "mikaSerial.h"

/** get the current time
 */
unsigned long schedullerTime( ) 
{
  static int usetimer = 0;
  static unsigned long long inittime;
  struct tms cputime;
            
  if (usetimer == 0)
  {
    inittime  = (unsigned long long)times(&cputime);
    usetimer = 1;
  }
  
  return (unsigned long)((times(&cputime) - inittime)*1000UL/sysconf(_SC_CLK_TCK));
} 

int schedullerEventIsActive( struct rootSchedullerEvent *event )
{
  if (!event)
    return 0;
  return event->active;
}

void schedullerEventIncrementSerial( struct rootSchedullerEvent *event )
{
  if (!event)
    return;
  event->serial++;
}

/** activate a scheduller event
 */
void schedullerEventActivate( struct rootSchedullerEvent *event )
{
  if (!event)
    return;
  event->active = 1;
}

/** deactivate a scheduller event
 */
void schedullerEventDeactivate( struct rootSchedullerEvent *event )
{
  if (!event)
    return;
  event->active = 0;
  schedullerEventIncrementSerial(event);
}

void *_schedullerEventExpose( )
{
  #ifdef _DEBUG
    printf("rootScheduller _schedullerEventExpose EVENTFLOW\n");
  #endif
  if (rdGlobal)
  {
  //  windowEventExpose(rdGlobal);
    _trigger_expose();
  }
}

/** send UI events to activities
 *  - send a button down event to an activity
 */
void *_schedullerEventButtondown( struct rootScheduller *scheduller )
{
  #ifdef _DEBUG
    printf("rootScheduller _schedullerEventButtondown serial(%d) EVENTFLOW\n",
      scheduller->queue.buttondown.serial);
  #endif
  struct rootObject *window = scheduller->queue.buttondown.custom;
  binarySemaphoreWait(window->act->sem->id);
  activityAddEventButtonDown(window->act,
    scheduller->queue.buttondown.x,scheduller->queue.buttondown.y);
  binarySemaphorePost(window->act->sem->id);
}

/**  - send a button up event to an activity
 */
void *_schedullerEventButtonup( struct rootScheduller *scheduller )
{
  if (!scheduller)
    return;
  #ifdef _DEBUG
    printf("rootScheduller _schedullerEventButtonup serial(%d) EVENTFLOW\n",
      scheduller->queue.buttondown.serial);
  #endif
  struct rootObject *window = scheduller->queue.buttondown.custom;
  binarySemaphoreWait(window->act->sem->id);
  activityAddEventButtonUp(window->act,
    scheduller->queue.buttondown.x,scheduller->queue.buttondown.y);
  binarySemaphorePost(window->act->sem->id);
}

/**  - send a motion event to an activity
 */
void *_schedullerEventMotion( struct rootScheduller *scheduller )
{
  if (!scheduller)
    return;
  #ifdef _DEBUG
    printf("rootScheduller _schedullerEventMotion serial(%d)\n",
      scheduller->queue.motion.serial);
  #endif
  struct rootObject *window = scheduller->queue.motion.custom;
}

void *rootSchedullerWatchdog(void*data)
{
  struct rootScheduller *scheduller = (struct rootScheduller*)data;
  long long count = 0;
  while(1)
  {
    rootSchedullerLock(scheduller);
    rootSchedullerUnlock(scheduller);
    #ifdef _DEBUG
      printf("rootSchedullerWatchdog count(%d)\n",count);
    #endif
    usleep(500000);
    count++;
  }
}

#define _SERIALIZE_EVENTS_NO
void *_schedullerThreadLoop( void *data )
{  
  if (!data)
    return;
  
  int expose 		= 0;
  int buttondown 	= 0;
  int buttonup 		= 0;
  struct rootScheduller *scheduller = (struct rootScheduller*)data;
  struct activityObject oExpose;
  static int lastEvent 	= 0;
  int clocktik = _SCHEDULLER_TIMER;
  static unsigned long timestamp;
  static struct mikaEvent;
  struct mikaSerial 		*serial;
  struct mikaSerialEvent 	serialEvent;
  
  pthread_t schedullerWatchdog;
  pthread_create(&schedullerWatchdog,NULL,rootSchedullerWatchdog,scheduller);
  serial = mikaSerialClone();
  
  if (!actObjects)
    actObjects = activityObjectsClone();
  
  while(1)
  {
    #ifdef _DEBUG
    //  printf("rootScheduller fetch event\n");
    #endif
    if (mikaSerialNextEvent(serial,&serialEvent,-1)==0)
    {
      #ifdef _DEBUG
        printf("rootSchedullerExpose serial event\n");
      #endif
      rootSchedullerExpose(scheduller);
    }
  
    expose 	= 0;
    buttondown 	= 0;
    buttonup 	= 0;
    rootSchedullerLock(scheduller);
    timestamp = schedullerTime();
    if (schedullerEventIsActive(&scheduller->queue.expose)==1)
    {  
      #ifdef _SCHEDULLER_SLOPPY
      #else
        if (
          scheduller->queue.expose.timestamp+(_SCHEDULLER_EXPOSE/1000)<=
            timestamp
        )
        {
          #ifdef _DEBUG
            printf("_schedullerThreadLoop scheduller->eventExpose UEVENT\n");
            printf("_schedullerThreadLoop reference(%d) current(%d) to(%d) UEVENT\n",
              scheduller->queue.expose.timestamp,timestamp,
              scheduller->queue.expose.timestamp+(_SCHEDULLER_EXPOSE/1000));
          #endif
          
          schedullerEventDeactivate(&scheduller->queue.expose);
          rootSchedullerUnlock(scheduller);
          scheduller->eventExpose();
          rootSchedullerLock(scheduller);
        }
      #endif
    }
    #ifdef _SCHEDULLER_SLOPPY
    #else
      if (schedullerEventIsActive(&scheduller->queue.buttondown)==1)
      {
        clocktik = _SCHEDULLER_TIMER;
        lastEvent = timestamp;
        #ifdef _DEBUG
          printf("_schedullerThreadLoop button reference(%d) current(%d) to(%d) UEVENT\n",
            scheduller->queue.buttondown.timestamp,timestamp,
            scheduller->queue.buttondown.timestamp+(_SCHEDULLER_BUTTON/1000));
        #endif
        if (
          scheduller->queue.buttondown.timestamp+(_SCHEDULLER_BUTTON/1000)<=
            timestamp
          )
        {
          #ifdef _DEBUG
            printf("_schedullerThreadLoop button trigger eventButtonup UEVENT\n");
          #endif
          scheduller->queue.buttondown.timestampEnded = timestamp;
          
          schedullerEventDeactivate(&scheduller->queue.buttondown);
          rootSchedullerUnlock(scheduller);
          scheduller->eventButtonup(scheduller);
          rootSchedullerLock(scheduller);
        }
      }
    #endif
    if (schedullerEventIsActive(&scheduller->queue.motion)==1)
    {
      clocktik = _SCHEDULLER_TIMER;
      lastEvent = timestamp;
      #ifdef _DEBUG
        printf("_schedullerThreadLoop motion x(%d) y(%d)\n",
          scheduller->queue.motion.x,
          scheduller->queue.motion.y);
      #endif
    }
    rootSchedullerUnlock(scheduller);
    
    // we slow down the scheduller if there has been no user activity
    // for some time
    if (lastEvent==0)
      lastEvent = timestamp;
      
    if (lastEvent+(_SCHEDULLER_IDLE/1000)<timestamp & clocktik != _SCHEDULLER_SLOW)
    {
      #ifdef _DEBUG
        printf("rootScheduller slowing down\n");
      #endif
      clocktik = _SCHEDULLER_SLOW;
    }
    usleep(clocktik);
  }
}

/** create a new root scheduller object
 */
struct rootScheduller *rootSchedullerNew( )
{
  struct rootScheduller *scheduller;
  scheduller = (struct rootScheduller*)malloc(sizeof(struct rootScheduller));
  scheduller->eventExpose 	= _schedullerEventExpose;
  scheduller->eventButtondown	= _schedullerEventButtondown;
  scheduller->eventButtonup	= _schedullerEventButtonup;
  scheduller->eventMotion	= _schedullerEventMotion;
  scheduller->threadLoop 	= _schedullerThreadLoop;
  pthread_mutex_init(&scheduller->mutex,NULL);
  scheduller->thread = -1;
  
  scheduller->queue.expose.active 	= 0;
  scheduller->queue.buttondown.active 	= 0;
  scheduller->queue.motion.active	= 0;
  scheduller->queue.cursor.active	= 0;
  
  scheduller->queue.expose.serial	= 0;
  scheduller->queue.buttondown.serial	= 0;
  scheduller->queue.buttondown.timestampEnded = 0;
  scheduller->queue.motion.serial	= 0;
  scheduller->queue.cursor.serial	= 0;
  
  return scheduller;
}

void rootSchedullerRelease( struct rootScheduller *scheduller )
{
  if (!scheduller)
    return;
  
  free(scheduller);
}

/** start the root scheduller thread
 */
void rootSchedullerRun( struct rootScheduller *scheduller )
{
  if (!scheduller)
    return;
  pthread_create(&scheduller->thread,NULL,_schedullerThreadLoop,scheduller);
}

void rootSchedullerLock( struct rootScheduller *scheduller )
{
  static long long count = 0;
  #ifdef _DEBUG
  //  printf("rootSchedullerLock count(%d)\n",count);
  #endif
  #ifndef _SCHEDULLER_LOCK
    return;
  #endif

  if (!scheduller)		
    return;
  
  pthread_mutex_lock(&scheduller->mutex);
  count++;
}

void rootSchedullerUnlock( struct rootScheduller *scheduller )
{
  #ifndef _SCHEDULLER_LOCK
    return;
  #endif

  static long long count = 0;
  #ifdef _DEBUG
  //  printf("rootSchedullerUnlock count(%d)\n",count);
  #endif
  
  if (!scheduller)		
    return;
  
  pthread_mutex_unlock(&scheduller->mutex);
  count++;
}

/** trigger an expose event on the root scheduller
 */
void rootSchedullerExpose( struct rootScheduller *scheduller )
{
  static int run = 0;
  #ifdef _DEBUG
    printf("rootSchedullerExpose %d\n",run);
  #endif
  if (!scheduller)
    return;
  /** directly sending this event could cause X server syncing problems
   *  and (code 14) error - crash of the server
   */
  #ifndef _SCHEDULLER_SLOPPY
    rootSchedullerLock(scheduller);
    if (scheduller->queue.expose.active==0)
    {
      scheduller->queue.expose.active = 1;
      scheduller->queue.expose.timestamp = schedullerTime();
    }
    rootSchedullerUnlock(scheduller);
  #else
    scheduller->eventExpose();
  #endif
  run++;
}

/** send a Button Down event to an activity and register it, so
 *  that it can be polled in the scheduller (when it triggers Button Up)
 */
void rootSchedullerButtondown( struct rootScheduller *scheduller, void *window, int x, int y )
{
  #ifdef _DEBUG
    printf("rootSchedullerButtondown EVENTFLOW\n");
  #endif
  if (!scheduller)
    return;
  
  #ifndef _SCHEDULLER_SLOPPY
    rootSchedullerLock(scheduller);
    // ensures a time delay between button events
    //
    if (
      scheduller->queue.buttondown.timestampEnded+(_SCHEDULLER_BUTTONDOWN_DELAY/1000)>
      schedullerTime()
      )
    {
      #ifdef _DEBUG
        printf("rootSchedullerButtondown skipping (still in time delay of last event) ended(%d)+%d > current(%d)\n",
          scheduller->queue.buttondown.timestampEnded,_SCHEDULLER_BUTTONDOWN_DELAY/1000,
          schedullerTime());
      #endif
      rootSchedullerUnlock(scheduller);
      return;
    }
    
    if (scheduller->queue.buttondown.active==0)
    {
      #ifdef _DEBUG
        printf("rootSchedullerButtondown !active\n");
      #endif
      scheduller->queue.buttondown.active = 1;
      scheduller->queue.buttondown.timestamp = schedullerTime();
      scheduller->queue.buttondown.x = x;
      scheduller->queue.buttondown.y = y;
      scheduller->queue.buttondown.custom = window;
      
      scheduller->queue.motion.fromx = x;
      scheduller->queue.motion.fromy = y;
    }
    else
    {
      #ifdef _DEBUG
        printf("rootSchedullerButtondown active\n");
      #endif
    }
  #else
    scheduller->queue.buttondown.active = 1;
    scheduller->queue.buttondown.x = x;
    scheduller->queue.buttondown.y = y;
    scheduller->queue.buttondown.custom = window;
  #endif
  rootSchedullerUnlock(scheduller);
  scheduller->eventButtondown(scheduller);
}

#define _SIMPLE_BUTTONUP_NO
void rootSchedullerButtonup( struct rootScheduller *scheduller )
{
  unsigned long start = schedullerTime();
  #ifdef _DEBUG
    printf("rootSchedullerButtonup start(%d) EVENTFLOW\n",start);
  #endif
  if (!scheduller)
  {
    #ifdef _DEBUG
      printf("rootScheduller no scheduller!\n");
    #endif
    return;
  }
  
  rootSchedullerLock(scheduller);
   
  // deactivate the execution of the buttondown event if we receive
  // a buttonup event
  //
  if (scheduller->queue.buttondown.active==1)
    schedullerEventDeactivate(&scheduller->queue.buttondown);
  
  #ifdef _DEBUG
    printf("rootSchedullerButtonup finish(%d)\n",schedullerTime()-start);
  #endif
  #ifdef _SIMPLE_BUTTONUP
    rootSchedullerUnlock(scheduller);
    return;
  #endif
    
  #ifndef _SCHEDULLER_SLOPPY // prefered
   
    if (scheduller->queue.buttondown.active==1)
    {
      #ifdef _DEBUG
        printf("rootSchedullerButtonup active\n");
      #endif
      rootSchedullerUnlock(scheduller);
      scheduller->eventButtonup(scheduller);
      rootSchedullerLock(scheduller);
      schedullerEventDeactivate(&scheduller->queue.buttondown);
    }
    else
    {
      #ifdef _DEBUG
        printf("rootSchedullerButtonup !active\n");
      #endif
    }
  #else
    if (scheduller->queue.buttondown.active==1)
    {
      scheduller->queue.buttondown.active = 0;
      rootSchedullerUnlock(scheduller);
      scheduller->eventButtonup(scheduller);
      rootSchedullerLock(scheduller);
    }
  #endif
  rootSchedullerUnlock(scheduller);
  #ifdef _DEBUG
    printf("rootSchedullerButtonup finish(%d)\n",schedullerTime()-start);
  #endif
}
 
void rootSchedullerMotion( struct rootScheduller *scheduller, void *window )
{
  #ifdef _DEBUG
    printf("rootSchedullerMotion\n");
  #endif
  if (!scheduller || !window)
    return;
  
  if (scheduller->queue.buttondown.active==1)
  {
    rootSchedullerLock(scheduller);
    // we don't need to sync outside, since the user events are synchronious
    //
    schedullerEventDeactivate(&scheduller->queue.buttondown);
    rootSchedullerUnlock(scheduller);
  }
}

/** groupes button down, button up and motion events. We receive only one
 *  type of event and then by timing and other attributes decide which type
 *  of event to trigger. This is good for buggy / too sensitive / unsensitive
 *  touch sensors
 */
void rootSchedullerCursor( struct rootScheduller *scheduller, int x, int y )
{
  #ifdef _DEBUG
    printf("rootSchedullerCursor xy(%d,%d)\n",x,y);
  #endif
  // todo
}

/** remove a button down event
 */
void rootSchedullerButtondownReset( struct rootScheduller *scheduller )
{
  #ifdef _DEBUG
    printf("rootSchedullerButtondownReset\n");
  #endif
  if (!scheduller)
    return;
  rootSchedullerLock(scheduller);
  #ifndef _SCHEDULLER_SLOPPY
    if (scheduller->queue.buttondown.active==1)
      scheduller->queue.buttondown.active = 0;
  #else
  #endif
  rootSchedullerUnlock(scheduller);
}
