/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _DRAQIE_IPC_H_
#define _DRAQIE_IPC_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

typedef struct draqieIpcMsg
{
  int value;
} draqieIpcMsg;

typedef struct draqieIpc
{
  int id;
  unsigned char name[256];
  struct draqieIpcMsg msg;
} draqieIpc;

struct draqieIpc *draqieIpcInit( );
int draqieIpcCreateListener( struct draqieIpc *myIpc );
int draqieIpcSend( struct draqieIpc *myIpc );

#endif
