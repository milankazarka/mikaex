/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _ACTIVITY_H_
#define _ACTIVITY_H_

#include "os.h"

enum activityTypes {
  _ACTIVITY_TYPE_CTRL,
  _ACTIVITY_TYPE_APP,
  _ACTIVITY_TYPE_EVENT,
  _ACTIVITY_TYPE_CUSTOM
};

enum activityEvents {
  _ACTIVITY_EVENT_NONE,
  _ACTIVITY_EVENT_BUTTONDOWN,	// button down
  _ACTIVITY_EVENT_BUTTONUP,	// button up
  _ACTIVITY_EVENT_MOTION,
  _ACTIVITY_EVENT_REPAINT, 	// internal repaint
  _ACTIVITY_EVENT_EXPOSE   	// commit to desktop
};

typedef struct activityEventButton {
  int		x;
  int		y;
} activityEventButton;

#define _ACTIVITY_EVENT_TESTOFFSET
typedef struct activityEvent {
  int		active;
  int		redraw; // 0/1
  int		type;
  struct activityEventButton
                button;
  #ifdef _ACTIVITY_EVENT_TESTOFFSET
    unsigned char	data[16];
  #endif
} activityEvent;

typedef struct activityAttributes {
  int				x;
  int				y;
  int				w;
  int				h;
  int				movable;  	// 0/1
  int				visible;  	// 0/1
  int				toplevel; 	// 0/1
  int				angle;		// actual angle
  int				forceangle;	// force an angle
} activityAttributes;

typedef struct activityData {
  struct activityEvent		event;
  struct activityAttributes	attributes;
  void				*surface;
} activityData;

typedef struct activity {
  int		type;
  unsigned char	name[128];
  rootShm	*shm; // struct activityData
  rootSem	*sem;
} activity;

int activityLock( struct activity *act );
int activityUnlock( struct activity *act );

struct activity *activityAlloc( struct activity *act );
int activityInit( struct activity *act, unsigned char *path );
int activityParseData( struct activity *act, unsigned char *data );
int activityRegisterOverSocket( struct activity *act );
int activityEventExposeOverSocket( struct activity *act);
int activityRegisterAccess( struct activity *act, unsigned char *filename ); // register access to the activity to file

int activitySHMInitAttributes( struct activity *act );
int activitySHMCreate( struct activity *act, int surfaceSize );
int activitySHMGetEvent( struct activity *act, struct activityEvent *event );
int activitySHMSetAttributes( struct activity *act, struct activityAttributes *attributes );
int activitySHMGetAttributes( struct activity *act, struct activityAttributes *attributes );
void *activitySHMGetSurface( struct activity *act );

int activityAddEventButtonDown( struct activity *act, int x, int y );
int activityAddEventButtonUp( struct activity *act, int x, int y );
int activityAddEventExpose( struct activity *act );
int activityAddEventRepaint( struct activity *act );
int activityEventActive( struct activity *act );
int activityEventGet( struct activity *act, struct activityEvent *remoteEvent );
int activityEventIsEvent( struct activity *act, int type );

int activitySetMovable( struct activity *act, int movable, int sync );
int activityGetMovable( struct activity *act, int sync );
int activitySetVisible( struct activity *act, int visible, int sync );
int activityGetVisible( struct activity *act, int sync );
int activitySetToplevel( struct activity *act, int toplebel, int sync );
int activityGetToplevel( struct activity *act, int sync );
int activitySetAngle( struct activity *act, int angle, int sync );
int activityGetAngle( struct activity *act, int sync );
int activitySetForceangle( struct activity *act, int angle, int sync );
int activityGetForceangle( struct activity *act, int sync );

#endif
