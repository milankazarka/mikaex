/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef __DOM_H__
#define __DOM_H__

#include <stdio.h>
#include <stdlib.h>

typedef struct _dom
{
  void	*low;
} dom;

typedef struct _domObject
{
  void	*low;
} domObject;

#ifdef __cplusplus
  extern "C" {
#endif

dom *dom_InitNamed( unsigned char *filename );
domObject *dom_Object( dom *dom, domObject *parent, unsigned char *named );

#ifdef __cplusplus
  }
#endif

#endif
