#ifndef _MIKA_MP_WINDOW_CLIENT_H_
#define _MIKA_MP_WINDOW_CLIENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "mikaHost.h"
#include "mikaNetRequest.h"
#include "mikaApps.h"

class mikaMpWindowClient {
  public:
    mikaNetRequest *netRequest;
    struct mikaHost myHost;
    mikaApp *myApp;
  
    mikaMpWindowClient( struct mikaHost *host );
    mikaMpWindowClient( mikaApp *app, struct mikaHost *host );
    ~mikaMpWindowClient( );
    
    int sendMessageData( unsigned char *data );
};

#endif
