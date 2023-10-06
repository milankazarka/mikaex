/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _BRIDGE_H_
#define _BRIDGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct mikaBridge
{
  unsigned char name[64];
  int semaphore;
  int w;
  int h;
} mikaBridge;

typedef struct mikaWidget
{
  int semaphore;
  int x;
  int y;
  int w;
  int h;
} mikaWidget;

struct mikaBridge *mikaBridgeInit( unsigned char *name );
int mikaBridgeExposeBuffer( struct mikaBridge *bridge, void *buffer );

struct mikaWidget *mikaWidgetAlloc( );

#endif
