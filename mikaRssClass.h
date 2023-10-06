#ifndef _MIKA_RSS_CLASS_H_
#define _MIKA_RSS_CLASS_H_

#include "libMrss/mrss.h"

class mikaRssClass {
  public:
    struct mrss_t *mrss;
  
    mikaRssClass( unsigned char *filename );
    ~mikaRssClass( );
};

#endif
