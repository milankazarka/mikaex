#ifndef _MIKA_NET_REQUEST_H_
#define _MIKA_NET_REQUEST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include "mikaHost.h"

#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"

#define _NET_REQUEST_INBUFFER 4096*1000

class mikaNetRequest {
  public:
    CURL *curl;
    unsigned char inBuffer[_NET_REQUEST_INBUFFER];
    
    mikaNetRequest( );
    ~mikaNetRequest( );
    
    int sendRequest( struct mikaHost *host, unsigned char *message );
    /** handshakeRequest sends a request and awaits a reply
     */
    int handshakeRequest( struct mikaHost *host, unsigned char *message, unsigned char *reply, int buflen );
};

#endif
