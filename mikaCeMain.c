#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mikaCe.h"
#include "config.h"
#define _STATIC

#ifdef _STATIC
  int main( )
  {
    mikaCe *ce = new mikaCe();
    if (ce)
    {
      ce->runEngine();
      while(1)
      { sleep(1); }
    }
    return 0;
  }
#endif
