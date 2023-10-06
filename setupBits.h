#ifndef _SETUP_BITS_H_
#define _SETUP_BITS_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct setupBit
{
  unsigned char name[128];
  unsigned char data[256];
  struct setupBit *next;
} setupBit;

typedef struct setupBits
{
  struct setupBit *bits;
} setupBits;

extern struct setupBits *globalBits;

struct setupBits *setupBitsInit( struct setupBits *bits );
int setupBitsAdd( struct setupBits *bits, struct setupBit *bit );
struct setupBit *setupBitsGet( struct setupBits *bits, unsigned char *name );

struct setupBit *setupBitInit( struct setupBit *bit );

#endif
