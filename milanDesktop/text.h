#ifndef _TEXT_H_
#define _TEXT_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "object.h"

class milanText : public object {
  public:
    unsigned char *myStr;
  
    milanText( unsigned char *str );
    ~milanText( );
};

#endif
