/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _OS_H_
#define _OS_H_

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

#include "config.h"

#define _FILE_LINESIZE 128

#ifndef XLIB // extremely bad practice (todo - remake)
  #define XLIB
#endif

enum _MSG_TYPES
{
  _MSG_TYPE_REDRAW,
  _MSG_TYPE_BUTTONDOWN,
  _MSG_TYPE_BUTTONUP
};

typedef struct msgRedraw
{
  unsigned char		activityName;
} msgRedraw;

/**
 *  shared memory
 */
typedef struct rootShm
{
  unsigned char		*data;
  int			size;
  int			id;
} rootShm;

typedef struct rootSem
{
  int			id;
} rootSem;

#ifdef XLIB
  #include <X11/Xlib.h>
  #include <X11/X.h>
  #include <X11/Xutil.h>
  #include <X11/extensions/shape.h>
  
  typedef struct lowWindow
  {
    Display		*dpy;
    int			screen;
    Window		id;
    
    GC			*gc;
    XGCValues		*gcv;
    
    Pixmap		p;
    Pixmap		m;
  } lowWindow;
#endif

typedef struct osWindowEvents
{
  void *(*callButtonPress)(int x, int y);
  void *(*callButtonRelease)(int x, int y);
} osWindowEvents;

typedef struct osWindowAttr
{
  int x;
  int y;
  int w;
  int h;
  struct osWindowEvents events;
} osWindowAttr;

typedef struct osWindow
{
  struct osWindowAttr	attr;
  struct osWindowEvents	events;
  struct lowWindow	low;
} osWindow;

int binarySemaphoreInitialize( int semid );
int binarySemaphoreWait( int semid );
int binarySemaphorePost( int semid );
struct rootShm *os_rshmInit( struct rootShm *shm );
int os_rshmAlloc( struct rootShm *shm, int size, int activityType, unsigned char *activityName );
struct rootSem *os_rsemInit( struct rootSem *sem );
int os_rsemLog( struct rootSem *sem, unsigned char *filename );
int os_rshmszLog( struct rootShm *shm, unsigned char *filename );
struct osWindow *osWindowInit( struct osWindowAttr *attr );
int osWindowEventLoop( struct osWindow *window );
int osWindowShow( struct osWindow *window );
int osWindowHide( struct osWindow *window );
int osInit( );

extern unsigned char *activityFilename;

#endif
