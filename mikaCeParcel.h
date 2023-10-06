#ifndef _MIKA_CE_PARCEL_H_
#define _MIKA_CE_PARCEL_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mikaFrame.h"

class mikaCeParcel {
  public:
    mikaCeParcel *next;
    struct mikaFrame frame;
    unsigned char id[8];
  
    mikaCeParcel( );
    mikaCeParcel( struct mikaFrame _frame );
    ~mikaCeParcel( );
    
    void setId( );
};

extern int mikaCeParcelCount;

#endif
