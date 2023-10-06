/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _MIKA_IPC_H_
#define _MIKA_IPC_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

typedef struct mikaIpcMsg
{
  int value;
} mikaIpcMsg;

typedef struct mikaIpc
{
  int id;
  unsigned char name[256];
  struct mikaIpcMsg msg;
} mikaIpc;

struct mikaIpc *mikaIpcInit( );
int mikaIpcCreateListener( struct mikaIpc *myIpc );
int mikaIpcSend( struct mikaIpc *myIpc );

#endif
