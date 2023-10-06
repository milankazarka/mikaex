/**  
  Copyright (C) 2012 Milan Kazarka
  milan.kazarka.office@gmail.com
      
  You may distribute under the terms of the Artistic License, 
  as specified in the README file. You can find a copy of the
  license at LICENSE.
*/
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
