/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _ACTIVITY_OBJECT_
#define _ACTIVITY_OBJECT_

#include "activity.h"
#include "config.h"

#define _ACTIVITY_OBJECTS_STORAGE 4096*10

typedef struct activityObjects
{
  rootShm		*shm;
  rootSem		*sem;
} activityObjects;

typedef struct activityObject
{
  unsigned char		active; 	// 0x01/0x02
  int			size;		// 02.04.2011 unused, all have the same size (todo)
  unsigned char		name[256];
  unsigned char		fastData[256];
} activityObject;

extern struct activityObjects *actObjects;

int activityObjectsLock( struct activityObjects *objects );
int activityObjectsUnlock( struct activityObjects *objects );
struct activityObjects *activityObjectsInit( ); // from controller
struct activityObjects *activityObjectsClone( ); // from applications

int activityObjectInit( struct activityObject *object );
int activityObjectRegister( struct activityObjects *objects, unsigned char *name );
int activityObjectClone( struct activityObjects *objects, unsigned char *name, struct activityObject *clone );
int activityObjectSet( struct activityObjects *objects, unsigned char *name, struct activityObject *from );
int activityObjectGet( struct activityObjects *objects, unsigned char *name );

#endif
