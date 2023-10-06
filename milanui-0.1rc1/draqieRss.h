/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _DRAQIE_RSS_H_
#define _DRAQIE_RSS_H_

#include "common.h"
#define _RSS_DATA_SIZE 1024*256

typedef struct draqieRss {
  unsigned char server[256];
  unsigned char remotePath[256];
  unsigned char data[_RSS_DATA_SIZE];
  void *(*parseCall)(struct draqieRss *myDraqieRss);
} draqieRss;

struct draqieRss *draqieRssInit( );
int draqieRssRelease( struct draqieRss *myDraqieRss );
int draqieRssParse( struct draqieRss *myDraqieRss, unsigned char *local );
int draqieRssGetData( struct draqieRss *myDraqieRss, unsigned char *local );

#endif
