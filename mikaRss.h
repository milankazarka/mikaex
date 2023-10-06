/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _MIKA_RSS_H_
#define _MIKA_RSS_H_

#include "common.h"
#define _RSS_DATA_SIZE 1024*256

typedef struct mikaRss {
  unsigned char server[256];
  unsigned char remotePath[256];
  unsigned char data[_RSS_DATA_SIZE];
  void *(*parseCall)(struct mikaRss *myMikaRss);
} mikaRss;

struct mikaRss *mikaRssInit( );
int mikaRssRelease( struct mikaRss *myMikaRss );
int mikaRssParse( struct mikaRss *myMikaRss, unsigned char *local );
int mikaRssGetData( struct mikaRss *myMikaRss, unsigned char *local );

#endif
