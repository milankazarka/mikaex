#ifndef _ACTIVITY_CLASS_H_
#define _ACTIVITY_CLASS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "activity.h"
#include "config.h"

class activityClass {
public:
  struct activity *act;
  int attach( );
  int detach( );
};

#endif
