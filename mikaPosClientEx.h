#ifndef _MIKA_POS_CLIENT_EX_H_
#define _MIKA_POS_CLIENT_EX_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include "mikaNetRequest.h"
#include "mikaHost.h"

class mikaPosClientEx {
  public:
    struct mikaHost *host;
    mikaNetRequest *requestHandler;
  
    mikaPosClientEx( struct mikaHost *_host );
    ~mikaPosClientEx( );
    int testHandshake( );
};

#endif
