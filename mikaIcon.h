#ifndef _MIKA_ICON_H_
#define _MIKA_ICON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "rootFrame.h"

#ifdef __cplusplus
  class mikaIcon {
    public:
      struct rootFrame frame;
    
      mikaIcon( );
      ~mikaIcon( );
  };
#endif

#endif
