/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _ROOT_SCHEDULLER_
#define _ROOT_SCHEDULLER_

#include "common.h"
  #define _SCHEDULLER_TIMER 		20
  #define _SCHEDULLER_SLOW		50
  #define _SCHEDULLER_IDLE_MINUTES	1
  #define _SCHEDULLER_IDLE		1000000*60*_SCHEDULLER_IDLE_MINUTES
  #define _SCHEDULLER_EXPOSE		50	
  #define _SCHEDULLER_BUTTON		50
  #define _SCHEDULLER_BUTTONDOWN_DELAY	200
  #define _SCHEDULLER_MOTION		100  
  #define _SCHEDULLER_MOTION_ACCEPT	100	
#ifdef _STRESS
#else
  #define _SCHEDULLER_TIMER 		2000
  #define _SCHEDULLER_SLOW		6000  // scheduller in idle mode
// if there is no user interaction, the schedullers cycle get's longer (idle mode)
  #define _SCHEDULLER_IDLE_MINUTES	1
  #define _SCHEDULLER_IDLE		1000000*60*_SCHEDULLER_IDLE_MINUTES
  #define _SCHEDULLER_EXPOSE		50000	
  #define _SCHEDULLER_BUTTON		100000  // a time delay after which the button event is triggered
  #define _SCHEDULLER_BUTTONDOWN_DELAY	150000  // needed time delay between 2 button down events
  #define _SCHEDULLER_MOTION		200000  // a time delay before other events are accepted
  #define _SCHEDULLER_MOTION_ACCEPT	170000	// after a button press, this delay is needed before motion is accepted (prevents shaking the view)
#endif
#define _SCHEDULLER_SLOPPY_NO		// execute events as they come
                                        // otherwise we would schedulle
                                        // them according to setup
#define _SCHEDULLER_SLOPPY_SYNC_NO
#define _SCHEDULLER_LOCK

typedef struct rootSchedullerEvent // todo, create more event types, remanufacture
{
  void			*custom;
  int			fromx;
  int			fromy;
  int			x;
  int			y;
  int			active;
  unsigned long		timestamp;
  unsigned long		timestampEnded;
  long			serial;
} rootSchedullerEvent;

typedef struct rootSchedullerQueue
{
  struct rootSchedullerEvent expose;
  struct rootSchedullerEvent buttondown;
  struct rootSchedullerEvent motion;
  /** the 'cursor' event merges the buttondown and motion event, so that
   *  so that we in end effect decide which one of those two the event is
   *
   *  this is a good strategy for buggy or less/too sensitive touch sensors
   *  where we could get button up events which doing a motion
   */
  struct rootSchedullerEvent cursor;
} rootSchedullerQueue;

typedef struct rootScheduller
{
  pthread_t 			thread;
  pthread_mutex_t		mutex;
  void				*(*threadLoop)();
  void				*(*eventExpose)();
  void				*(*eventButtondown)(struct rootScheduller *scheduller);
  void				*(*eventButtonup)(struct rootScheduller *scheduller);
  void				*(*eventMotion)(struct rootScheduller *scheduller);
  struct rootSchedullerQueue	queue;
} rootScheduller;

struct rootScheduller *rootSchedullerNew( );
void rootSchedullerRelease( struct rootScheduller *scheduller );
void rootSchedullerRun( struct rootScheduller *scheduller );
void rootSchedullerLock( struct rootScheduller *scheduller );
void rootSchedullerUnlock( struct rootScheduller *scheduller );
void rootSchedullerExpose( struct rootScheduller *scheduller );
void rootSchedullerButtondown( struct rootScheduller *scheduller, void *window, int x, int y );
void rootSchedullerButtonup( struct rootScheduller *scheduller );
void rootSchedullerMotion( struct rootScheduller *scheduller, void *window );
void rootSchedullerCursor( struct rootScheduller *scheduller, int x, int y );
void rootSchedullerButtondownReset( struct rootScheduller *scheduller );

unsigned long schedullerTime( );
int schedullerEventIsActive( struct rootSchedullerEvent *event );
void schedullerEventIncrementSerial( struct rootSchedullerEvent *event );
void schedullerEventActivte( struct rootSchedullerEvent *event );
void schedullerEventDeactivate( struct rootSchedullerEvent *event );

#endif
