#ifndef _MIKA_SIP_CALLER_H_
#define _MIKA_SIP_CALLER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

class mikaSipCaller {
  public:
    unsigned char *myNumber;
  
    mikaSipCaller( unsigned char *number );
    ~mikaSipCaller( );
    
    int call( );
};

#endif
