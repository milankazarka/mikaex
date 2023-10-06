#ifndef _MIKA_ITEM_H_
#define _MIKA_ITEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

class mikaItem {
  public:
    mikaItem *next;
  
    mikaItem( );
    ~mikaItem( );
};

#endif
