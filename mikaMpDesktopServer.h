#ifndef _MIKA_MP_DESKTOP_SERVER_H_
#define _MIKA_MP_DESKTOP_SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "mikaNetServer.h"
#include "mikaApps.h"
#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"

#define _MIKA_MP_DESKTOP_SERVER_PORT 48000

class mikaMpDesktopServer {
  public:
    struct mikaNetConnectionDelegate connectionDelegate;
    mikaNetServer *server;
    pthread_t serverThread;
    /** applications / windows that registered at us
     */
    appRegisterObject *apps;
  
    mikaMpDesktopServer( );
    ~mikaMpDesktopServer( );
    
    int runServerThread( );
    int onMessageRoot( TiXmlElement *root );
};

extern mikaMpDesktopServer *mpServerGlobal;
int mikaMpDesktopServerGlobalInit( );

#endif
