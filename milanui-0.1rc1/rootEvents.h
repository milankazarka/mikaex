/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _ROOT_EVENTS_
#define _ROOT_EVENTS_

/** this is the entry point for hardware events. We basically
 *  generate an 'ePlatform' event for the rootScheduller & 
 *  rootController to process
 */

#include "common.h"
#include "os.h"
#include "rootController.h"

#define _ROOT_EVENTS_BUFFER 1

enum _ROOT_EVENT_TYPES {
  _ROOT_EVENT_NONE,
  _ROOT_EVENT_BUTTONDOWN,
  _ROOT_EVENT_BUTTONUP,
  _ROOT_EVENT_MOTION,
  _ROOT_EVENT_CURSOR,
  _ROOT_EVENT_EXPOSE
};

typedef struct rootEventPosition
{
  int x;
  int y;
} rootEventPosition;

typedef struct rootEventButton
{
  struct rootEventPosition position;
} rootEventButton;

typedef struct rootEventMotion
{
  struct rootEventPosition position;
} rootEventMotion;

/** the 'Cursor' is intended to be a merge of a button down
 *  and motion event - which is needed on some 'buggy' sensors
 *  where sometimes a buttonup is generated while doing motion
 *  in which case we ourselves decide if an event is 'down' 'up'
 *  or motion, based on time intervals
 *
 *  check this functionality by (rootEventsMerge)
 */
#define _ROOT_EVENTS_MERGE
typedef struct rootEventCursor
{
  struct rootEventPosition position;
} rootEventCursor;

typedef struct rootEventExpose
{
} rootEventExpose;

typedef struct rootEvent
{
  int active;
  int type;
  long timestamp;
  struct rootEventButton button;
  struct rootEventMotion motion;
  struct rootEventCursor cursor;
  struct rootEventExpose expose;
} rootEvent;

typedef struct rootEvents
{
  struct lowWindow *window;
  struct rootEvent current[_ROOT_EVENTS_BUFFER];
} rootEvents;

struct rootEvents *rootEventsInit( struct rootEvents *events );
struct rootEvent *rootEventsGetFreeEvent( struct rootEvents *events );
int rootEventsEventButtonDown( struct rootEvents *events, int x, int y );
int rootEventsEventButtonUp( struct rootEvents *events, int x, int y );
int rootEventsEventCursor( struct rootEvents *events, int x, int y );
int rootEventsEventMotion( struct rootEvents *events, int x, int y );
int rootEventsEventExpose( struct rootEvents *events );
int rootEventsMerge( struct rootEvents *events );

#endif
