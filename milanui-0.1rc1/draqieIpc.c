/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

/** todo - implement our IPC wrapper
 */

#include "draqieIpc.h"
#include "config.h"
#include "mikaServer.h"
struct draqieIpc *draqieIpcInit( )
{
  struct draqieIpc *myIpc = (struct draqieIpc*)malloc(sizeof(struct draqieIpc));
  return myIpc;
}

int draqieIpcCreateListener( struct draqieIpc *myIpc )
{
  if (!myIpc)
    return 1;
  return 0;
}

int draqieIpcSend( struct draqieIpc *myIpc )
{
  if (!myIpc)
    return 1;
  return 0;
}
