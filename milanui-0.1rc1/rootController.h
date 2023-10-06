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

#define _TEST_REDRAW                  // redraw the surface on any event
#define _ROOT_DEFAULT_WIDTH 800
#define _ROOT_DEFAULT_HEIGHT 600
#define _DEFAULT_BG "img/wallpaper009.png"

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

typedef struct rootObject
{
  struct activity	*act;
  
  struct rootShm	*shm;
  struct _frame		frame;
  struct rootObject	*next;
  unsigned char		name[32];
  int			poll;
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

#endif
