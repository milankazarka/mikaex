#ifndef _MIKA_UID_H_
#define _MIKA_UID_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <uuid/uuid.h>

class mikaUID {
  public:
    unsigned char *uuid;
  
    mikaUID( );
    ~mikaUID( );
};

#endif
