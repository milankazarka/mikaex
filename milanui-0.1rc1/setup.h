/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _SETUP_H_
#define _SETUP_H_

#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"

typedef struct setup {
  TiXmlDocument data;
  TiXmlElement *root;
} setup;

struct setup *setupInit( struct setup *s, unsigned char *filename );
int setupGet( struct setup *s, unsigned char *name, unsigned char *attr, unsigned char *to );

#endif

