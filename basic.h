#ifndef _BASIC_H_
#define _BASIC_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct basicAtom
{
  unsigned char key[256];
  unsigned char value[256];
  struct basicAtom *next;
} basicAtom;

typedef struct basic
{
  struct basicAtom *atoms;
} basic;

struct basic *basicInit( struct basic *b );
unsigned char *basicGet( struct basic *b, unsigned char *key );

#endif
