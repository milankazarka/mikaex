#include "setupBits.h"

#ifndef _DEBUG
  #define _DEBUG
#endif

struct setupBits *globalBits = NULL;

struct setupBits *setupBitsInit( struct setupBits *bits )
{
  #ifdef _DEBUG
    printf("setupBitsInit\n");
  #endif
  if (globalBits)
    return globalBits;
  
  if (!bits)
    bits = (struct setupBits*)malloc(sizeof(struct setupBits));
  
  globalBits = bits;
  bits->bits = NULL;
  
  return bits;
}

int setupBitsAdd( struct setupBits *bits, struct setupBit *bit )
{
  #ifdef _DEBUG
    printf("setupBitsAdd\n");
  #endif
  
  if (!bits||!bit)
  {
    #ifdef _DEBUG
      printf("setupBitsAdd ERROR invalid arguments\n");
    #endif
    return 1;
  }
  
  bit->next = bits->bits;
  bits->bits = bit;
  
  return 0;
}

struct setupBit *setupBitsGet( struct setupBits *bits, unsigned char *name )
{
  #ifdef _DEBUG
    printf("setupBitsGet\n");
  #endif
  
  if (!bits||!name)
  {
    #ifdef _DEBUG
      printf("setupBitsGet ERROR invalid arguments\n");
    #endif
    return NULL;
  }
  
  struct setupBit *current = bits->bits;
  while(current)
  {
    if (current->name[0]!=0x00)
    {
      if (
        strlen((char*)current->name)==
        strlen((char*)name)
        )
      {
        if (strcmp((char*)current->name,(char*)name)==0)
        {
          #ifdef _DEBUG
            printf("setupBitsGet hit\n");
          #endif
          return current;
        }
      }
    }
    current = current->next;
  }
  
  return NULL;
}

struct setupBit *setupBitInit( struct setupBit *bit )
{
  if (!bit)
    bit = (struct setupBit*)malloc(sizeof(struct setupBit));
  bit->name[0] = 0x00;
  bit->data[0] = 0x00;
  return bit;
}
