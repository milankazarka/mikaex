/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _ROOT_CONTROLLER_H_
#define _ROOT_CONTROLLER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/utsname.h>
#include <semaphore.h>
#include <dirent.h>

#include "os.h"
#include "activity.h"
#include "rootFrame.h"
#include "mikaIcon.h"

#define _SHM_ID_FILENAME "logShmRootController"
#define _FILE_LINESIZE 128
#define _FRAME_DEFAULTW 360
#define _FRAME_DEFAULTH 360

#define _STATIC

#ifndef XLIB
  #define XLIB
#endif

#ifndef USECAIRO
  #define USECAIRO
#endif

#include <cairo/cairo.h>

#define _TEST_REDRAW                  	// redraw the surface on any event
#define _ROOT_DEFAULT_WIDTH 600
#define _ROOT_DEFAULT_HEIGHT 700
#define _ROOT_OBJECT_FRAME 20
//#define _DEFAULT_BG "FilesShop/bg02.png"
//#define _DEFAULT_BG "adidasFiles/background.png"
#define _DEFAULT_BG "imgHotel/bg.png"
#define _HIDE_CURSOR_NO
#define _MOTION_ALL		 	// move the toplevel object even if we didn't
                                        // hit it - makes, the motion more convenient
#define _MOTION_ACCEL		 	// motion acceleration
#define _TRANSPARENCY_VALUE 0xd2 	// transparency of the toplevel object
#define _EXPOSE_RESPONSE
#define _TOPLEVEL_PRIORITY_NO		// only toplevel window movable

enum _STATE
{
  _STATE_ACTIVE,
  _STATE_INACTIVE
};

typedef struct eventPlatform
{
  int			state;
  XEvent		event;
} eventPlatform;

typedef struct eventApi
{
  int			state;
  struct activityEvent	event;
  XEvent		xevent;
} eventApi;

typedef struct _frame
{
  int	x;
  int	y;
  int	w;
  int	h;
  int	angle;
  int	sector;
} _frame;

typedef struct rootAtom
{
  pthread_t		id;
  void			*data;
  void			*(*call)(void*data);
} rootAtom;

/** idealy the icons and the state of the objects should be
 *  managed by the WM - that's the right architecture
 */
enum _ROOT_OBJECT_TYPE {
  _ROOT_OBJECT_TYPE_NONE,
  _ROOT_OBJECT_TYPE_APP,
  _ROOT_OBJECT_TYPE_ICON
};

typedef struct rootObject
{
  int 			type;
  // refactoring
  int			forceangle; // 361 for not set

  struct activity	*act;
  
  struct rootShm	*shm;
  struct _frame		frame;
  struct rootObject	*next;
  unsigned char		name[32];
  int			poll;
  
  cairo_surface_t	*surface;	// surface data manipulation
  void			*surfaceData;	// copy of the interface surface
} rootObject;

typedef struct rootDesktop
{
  struct lowWindow	*window;
  struct _frame		frame;
  struct rootObject	*children;
  struct rootObject	*active; // currently used object
  struct rootShm	*shmSurface;
  struct rootSem	*sem;
  
  struct rootAtom	*atomActPoll;
  struct rootAtom	*atomPlatformPoll;
  
  struct eventPlatform	*ePlatform;
  
  int			pointerx;
  int			pointery;
} rootDesktop;

extern struct rootDesktop *mainRd; // deprecated
extern struct rootDesktop *rdGlobal;

int windowEventButtonDown( struct rootDesktop *rd, struct rootObject *active, int x, int y );
int windowEventButtonUp( struct rootDesktop *rd, struct rootObject *active, int x, int y );

struct eventPlatform eventPlatformMake();
struct eventApi eventApiMake();
struct rootShm *rshmInit( struct rootDesktop *rd, struct rootShm *shm );
int rshmAlloc( struct rootDesktop *rd, struct rootShm *shm, int size );
struct rootSem *rsemInit( struct rootDesktop *rd, struct rootSem *sem );
int rdPollObjects( struct rootDesktop *rd );
void *rdPollObjectsThread( void *data );
struct rootAtom *ratomAlloc( struct rootDesktop *rd, struct rootAtom *atom );
int ratomInit( struct rootDesktop *rd, struct rootAtom *atom );
int rdAddObject( struct rootDesktop *rd, struct rootObject *ro );
int rdRemoveObject( struct rootDesktop *rd, struct rootObject *ro );
int rdDrawObjectSurfaceESYNC( struct rootDesktop *rd, struct rootObject *ro, cairo_t *cairo );
int pollExposuresStart( struct rootDesktop *rd );
struct rootObject *rdPositionToObject( struct rootDesktop *rd, int x, int y );
void *_idle_thread_call(void*data);
void *rootControllerWatchdog(void*data);
void _trigger_expose( );

void rootControllerLock( int id );
void rootControllerUnlock( int id );

#endif
