/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

/** todo - implement our IPC wrapper
 */

#include "mikaIpc.h"
#include "config.h"
#include "mikaServer.h"
struct mikaIpc *mikaIpcInit( )
{
  struct mikaIpc *myIpc = (struct mikaIpc*)malloc(sizeof(struct mikaIpc));
  return myIpc;
}

int mikaIpcCreateListener( struct mikaIpc *myIpc )
{
  if (!myIpc)
    return 1;
  return 0;
}

int mikaIpcSend( struct mikaIpc *myIpc )
{
  if (!myIpc)
    return 1;
  return 0;
}
