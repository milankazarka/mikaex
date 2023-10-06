/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _APPS_H_
#define _APPS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define _APPS_DIRECTORY "./applications"

typedef struct app
{
  unsigned char name[256];
  struct app *next;
} app;

extern struct app *apps;

struct app *appNew( );
void appRelease( struct app *myApp );
int appIsRegistered( unsigned char *name );
void appListApplications( );
void appRegisterAll( );

#endif
