#ifndef _MIKA_WM_ERROR_H_
#define _MIKA_WM_ERROR_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sysCommon.h"

int mikaErrorHandler( Display *dpy, XErrorEvent *event );

#endif
