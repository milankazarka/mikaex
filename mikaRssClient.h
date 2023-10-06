#ifndef _MIKA_RSS_CLIENT_H_
#define _MIKA_RSS_CLIENT_H_

#include <stdio.h>
#include <stdlib.h>
#include "mikaRssClass.h"
#include "wm.h"

class mikaRssClient {
  public:
    mikaRssClass *rssc;
    struct tr_Object *header;
    struct tr_Object *prev;
    struct tr_Object *next;
  
    mikaRssClient( unsigned char *filename );
    ~mikaRssClient( );
};

#endif
