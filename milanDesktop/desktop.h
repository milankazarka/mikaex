#ifndef _DESKTOP_H_
#define _DESKTOP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mikaGl/mikaGlWindow.h"

class milanDesktop {
  public:
    mikaGlWindow *glWindow;
  
    milanDesktop( );
    ~milanDesktop( );
};

#endif
