#ifndef _MIKA_KIT_H_
#define _MIKA_KIT_H_

#include "mikaCommon.h"

class mikaKit {
  public:
    mikaKit( unsigned char *appName );
    ~mikaKit( );
};

extern mikaKit *globalMikaKit;

#endif
