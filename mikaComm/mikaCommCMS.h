#ifndef _MIKA_COMM_CMS_H_
#define _MIKA_COMM_CMS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mikaHost.h"
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include "mikaNetRequest.h"
#define _COMM_CMS_SETUP "cmsSetup.xml"

class mikaCommCMS {
  public:
    struct mikaHost host;
    mikaNetRequest *netRequest;
    unsigned char requestMessage[_NET_REQUEST_INBUFFER];
    unsigned char responseMessage[_NET_REQUEST_INBUFFER];
    
    mikaCommCMS( struct mikaHost *_host );
    ~mikaCommCMS( );
    /** read the setup of the CMS processor
     */
    int readSetup( );
    /** get the config file using low level sockets
     */
    void getConfig( );
    /** get the config file using libCurl
     */
    int getConfigEx( );
};

#endif
