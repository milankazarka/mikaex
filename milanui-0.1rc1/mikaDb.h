/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _MIKA_DB_H_
#define _MIKA_DB_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <db.h> // BerkeleyDB

typedef struct mikaDb
{
    DB *db;    
} mikaDb;

struct mikaDb *mikaDbCreate( struct mikaDb *db, unsigned char *filename );
struct mikaDb *mikaDbInit( struct mikaDb *db, unsigned char *filename );
int mikaDbRelease( struct mikaDb *db );
int mikaDbPut( struct mikaDb *db, unsigned char *key, unsigned char *value );

#endif
