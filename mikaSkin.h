#ifndef _MIKA_SKIN_H_
#define _MIKA_SKIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define _SKIN_PATH "skin.xml"

typedef struct mikaSkinColor
{
  unsigned char hex[8];
} mikaSkinColor;

typedef struct mikaSkinNode
{
  unsigned char name[512];
  struct mikaSkinColor color;
  struct mikaSkinNode *next;
} mikaSkinNode;

typedef struct mikaSkin
{
  struct mikaSkinNode *nodes;
  void *xmlRoot;
} mikaSkin;

#ifdef __cplusplus
  extern "C"
#endif
int mikaSkinGlobalInit( );
#ifdef __cplusplus
  extern "C"
#endif
int mikaSkinGlobalLoadStandardSet( );
#ifdef __cplusplus
  extern "C"
#endif
struct mikaSkinNode *mikaSkinGlobalGetNode( unsigned char *name );
#ifdef __cplusplus
  extern "C"
#endif
int mikaSkinGlobalNodeGetColor( struct mikaSkinNode *node, unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a );

extern struct mikaSkin *skinInstance;

#endif
