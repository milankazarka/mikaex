/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _LAYOUT_H_
#define _LAYOUT_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "libTinyXml/tinyxml.h"

typedef struct layoutEx
{
  TiXmlDocument doc;
} layoutEx;

struct layoutEx *layoutNew( );
void layoutRelease( struct layoutEx *myLayout );
int layoutInitWithPath( struct layout *myLayout, unsigned char *path );

#endif
