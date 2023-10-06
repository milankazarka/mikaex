#ifndef _MIKA_COMM_CMS_TEST_H_
#define _MIKA_COMM_CMS_TEST_H_

#include "mikaCommCMS.h"

class mikaCommCMSTest {
  public:
    mikaCommCMS *cms;
    
    mikaCommCMSTest( mikaCommCMS *_cms );
    ~mikaCommCMSTest( );
    void start();
};

#endif
