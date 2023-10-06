#ifndef _MIKA_CE_OBJECT_H_
#define _MIKA_CE_OBJECT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mikaCeParcel.h"

class mikaCeObject {
  public:
    unsigned char id[8];
    struct mikaFrame frame;
    void *test;
    mikaCeObject *next;
    mikaCeObject( );
    ~mikaCeObject( );
    
    void setId( );
};

extern int mikaCeObjectCount;

#endif
