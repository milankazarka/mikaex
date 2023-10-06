/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _MIKA_SERVER_MOD_
#define _MIKA_SERVER_MOD_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mikaMessage.h"
#include "mikaServerModDelegate.h"

#define _MIKA_SERVER_MOD_CLASS_NO

enum _SERVER_MOD_MODES
{
  _SERVER_MOD_GENERIC
};

typedef struct mikaServerMod
{
  unsigned char id[256];
  int mode;
  void *(*processData)( unsigned char *in, unsigned char *out );
  struct mikaServerMod *next;
  struct mikaServerModDelegate *delegate;
} mikaServerMod;

struct mikaServerMod *mikaServerModInit( );
int mikaServerModRunMessage( struct mikaServerMod *myServerMod, unsigned char *msg, unsigned char *response );
int mikaServerModIsMessage( struct mikaServerMod *myServerMod, unsigned char *msg );
/** general handling of the connection
 */
int mikaServerModConnection( struct mikaServerMod *myServerMod, void *myServer, void *myConnection );

#endif
