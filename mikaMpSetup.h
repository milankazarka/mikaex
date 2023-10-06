#ifndef _MIKA_MP_SETUP_H_
#define _MIKA_MP_SETUP_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "mikaXml/tinystr.h"
#include "mikaXml/tinyxml.h"

#define _MIKA_MP_SETUP "mikaMpSetup.xml"

class mikaMpSetup {
  public:
    TiXmlDocument doc;
  
    mikaMpSetup( unsigned char *path );
    ~mikaMpSetup( );
};

extern mikaMpSetup *mpSetupGlobal;
int mikaMpSetupGlobalInit( );
int mikaMpSetupGlobalDeinit( );

#endif
