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
 */
 
#include "rootScheduller.h"
#include "rootController.h"
#include "config.h"

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
  if (rdGlobal)
    windowEventExpose(rdGlobal); // todo, make this independent
}

/** send UI events to activities
 *  - send a button down event to an activity
 */
void *_schedullerEventButtondown( struct rootScheduller *scheduller )
{
  #ifdef _DEBUG
    printf("_schedullerEventButtondown serial(%d)\n",
      scheduller->queue.buttondown.serial);
  #endif
  struct rootObject *window = scheduller->queue.buttondown.custom;
  binarySemaphoreWait(rdGlobal->sem->id);
  activityAddEventButtonDown(window->act,
    scheduller->queue.buttondown.x,scheduller->queue.buttondown.y);
  binarySemaphorePost(rdGlobal->sem->id);
}

/**  - send a button up event to an activity
 */
void *_schedullerEventButtonup( struct rootScheduller *scheduller )
{
  if (!scheduller)
    return;
  #ifdef _DEBUG
    printf("_schedullerEventButtonup serial(%d)\n",
      scheduller->queue.buttondown.serial);
  #endif
  struct rootObject *window = scheduller->queue.buttondown.custom;
  binarySemaphoreWait(rdGlobal->sem->id);
  activityAddEventButtonUp(window->act,
    scheduller->queue.buttondown.x,scheduller->queue.buttondown.y);
  binarySemaphorePost(rdGlobal->sem->id);
}

/**  - send a motion event to an activity
 */
void *_schedullerEventMotion( struct rootScheduller *scheduller )
{
  if (!scheduller)
    return;
  #ifdef _DEBUG
    printf("_schedullerEventMotion serial(%d)\n",
      scheduller->queue.motion.serial);
  #endif
  struct rootObject *window = scheduller->queue.motion.custom;
  binarySemaphoreWait(rdGlobal->sem->id);
  
  binarySemaphorePost(rdGlobal->sem->id);
}

#define _SERIALIZE_EVENTS_NO
#define _ASYNC_INTERFACE_EVENTS_NO
void *_schedullerThreadLoop( void *data )
{  
  if (!data)
    return;
  
  int expose 		= 0;
  int buttondown 	= 0;
  int buttonup 		= 0;
  struct rootScheduller *scheduller = (struct rootScheduller*)data;
  
  while(1)
  {
    expose 	= 0;
    buttondown 	= 0;
    buttonup 	= 0;
    rootSchedullerLock(scheduller);
    unsigned long timestamp = schedullerTime();
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
            printf("_schedullerThreadLoop reference(%d) current(%d) to(%d)\n",
              scheduller->queue.expose.timestamp,timestamp,
              scheduller->queue.expose.timestamp+(_SCHEDULLER_EXPOSE/1000));
          #endif
          
          #ifndef _ASYNC_INTERFACE_EVENTS
            scheduller->eventExpose();
          #else
            expose = 1;
          #endif
          schedullerEventDeactivate(&scheduller->queue.expose);
        }
      #endif
    }
    #ifdef _SCHEDULLER_SLOPPY
    #else
      if (schedullerEventIsActive(&scheduller->queue.buttondown)==1)
      {
        #ifdef _DEBUG
          printf("_schedullerThreadLoop button reference(%d) current(%d) to(%d)\n",
            scheduller->queue.buttondown.timestamp,timestamp,
            scheduller->queue.buttondown.timestamp+(_SCHEDULLER_EXPOSE/1000));
        #endif
        if (
          scheduller->queue.buttondown.timestamp+(_SCHEDULLER_BUTTON/1000)<=
          timestamp
          )
        {
          #ifdef _ASYNC_INTERFACE_EVENTS
            scheduller->eventButtonup(scheduller);
          #else  
            buttonup = 1;
          #endif
          
          #ifdef _SERIALIZE_EVENTS
            scheduller->queue.buttondown.timestamp = timestamp;
          #else
            schedullerEventDeactivate(&scheduller->queue.buttondown);
          #endif
        }
      }
    #endif
    if (schedullerEventIsActive(&scheduller->queue.motion)==1)
    {
      #ifdef _DEBUG
        printf("_schedullerThreadLoop motion x(%d) y(%d)\n",
          scheduller->queue.motion.x,
          scheduller->queue.motion.y);
      #endif
    }
    rootSchedullerUnlock(scheduller);
    
    #ifndef _ASYNC_INTERFACE_EVENTS
      if (expose==1)	scheduller->eventExpose();
      if (buttonup==1)	scheduller->eventButtonup(scheduller);
    #endif
    
    usleep(_SCHEDULLER_TIMER);
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
  if (!scheduller)		
    return;
  
  pthread_mutex_lock(&scheduller->mutex);
}

void rootSchedullerUnlock( struct rootScheduller *scheduller )
{
  if (!scheduller)		
    return;
  
  pthread_mutex_unlock(&scheduller->mutex);
}

/** trigger an expose event on the root scheduller
 */
void rootSchedullerExpose( struct rootScheduller *scheduller )
{
  #ifdef _DEBUG
    printf("rootSchedullerExpose\n");
  #endif
  if (!scheduller)
    return;
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
}

/** send a Button Down event to an activity and register it, so
 *  that it can be polled in the scheduller (when it triggers Button Up)
 */
void rootSchedullerButtondown( struct rootScheduller *scheduller, void *window, int x, int y )
{
  #ifdef _DEBUG
    printf("rootSchedullerButtondown\n");
  #endif
  if (!scheduller)
    return;
   
  rootSchedullerLock(scheduller);
  #ifndef _SCHEDULLER_SLOPPY
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

void rootSchedullerButtonup( struct rootScheduller *scheduller )
{
  #ifdef _DEBUG
    printf("rootSchedullerButtonup\n");
  #endif
  if (!scheduller)
  {
    #ifdef _DEBUG
      printf("rootScheduller no scheduller!\n");
    #endif
    return;
  }
  
  rootSchedullerLock(scheduller);
  if (scheduller->queue.buttondown.active==1)
  {
    schedullerEventDeactivate(&scheduller->queue.buttondown);
  }
  rootSchedullerUnlock(scheduller);  
  return;
    
  rootSchedullerLock(scheduller);
  #ifndef _SCHEDULLER_SLOPPY
    if (schedullerEventIsActive(&scheduller->queue.motion)==1)
    {
    }
  
    if (scheduller->queue.buttondown.active==1)
    {
      #ifdef _DEBUG
        printf("rootSchedullerButtonup active\n");
      #endif
      scheduller->eventButtonup(scheduller);
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
      scheduller->eventButtonup(scheduller);
    }
  #endif
  rootSchedullerUnlock(scheduller);
}

void rootSchedullerMotion( struct rootScheduller *scheduller, void *window )
{
  #ifdef _DEBUG
    printf("rootSchedullerMotion\n");
  #endif
  if (!scheduller || !window)
    return;
  
  rootSchedullerLock(scheduller);
  if (scheduller->queue.buttondown.active==1)
  {
    schedullerEventDeactivate(&scheduller->queue.buttondown);
  }
  rootSchedullerUnlock(scheduller);
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
