#ifndef _MIKA_SCENE_H_
#define _MIKA_SCENE_H_

#include "mikaCommon.h"
#include "dHandle.h"

class mikaScene {
  public:
    dhandle lowScene;
    
    mikaScene( dhandle window );
    ~mikaScene( );
};

#endif
