/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _MIKA_PARSER_H_
#define _MIKA_PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <db.h>
#include "mikaDb.h"

typedef struct mikaParserIterator
{
  unsigned char id[128];
  unsigned char base[512];
  unsigned char root[256];
  int index;
  int limit;
  int status;
  void	*(*callNextPage)(struct mikaParserIterator *myIterator);
  void	*(*callPageLevel02)(struct mikaParserIterator *myIterator, unsigned char *filename);
  void	*(*callPageLevel03)(struct mikaParserIterator *myIterator, unsigned char *filename);
  void 	*(*callPageExternal)(struct mikaParserIterator *myIterator, unsigned char *url);
} mikaParserIterator;

typedef struct mikaParser
{
  int				masterIndex;
  FILE				*log;
  pthread_mutex_t		mutexId;
  struct mikaParserIterator 	*iterator;
} mikaParser;

extern struct mikaParser *parser;

struct mikaParserIterator *mikaParserIteratorInit( );
int mikaParserRunIterator( struct mikaParserIterator *myIterator );

struct mikaParser *mikaParserInit( );
int mikaParserLock( struct mikaParser *myParser );
int mikaParserUnlock( struct mikaParser *myParser );

int mikaHtmlLineForSection( struct mikaParserIterator *myIterator, FILE *fd, unsigned char *starter, unsigned char *ident, unsigned char *storage );

#endif
