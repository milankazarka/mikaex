#ifndef _MIKA_CE_NETWORK_H_
#define _MIKA_CE_NETWORK_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <pthread.h>

#include "mikaXml/tinystr.h"
#include "mikaXml/tinyxml.h"

#define _MIKA_CE_LISTENER_PORT 29000
#define _MIKA_CE_PRELISTENBUFFER 4096*200
#define _MIKA_CE_LISTENBUFFER 4096*1000*2 // 8MB

class mikaCeNetwork {
  public:
    mikaCeNetwork( );
    ~mikaCeNetwork( );
};

class mikaCeNetworkListener {
  public:
    int port;
    int sock;
  
    mikaCeNetworkListener( int port );
    ~mikaCeNetworkListener( );
    int Listen( );
    int connection( int newsocket );
    int processMessage( unsigned char *messageBuffer );
    
    int addObject( TiXmlElement *root );
    int rmObject( TiXmlElement *root );
    int mvObject( TiXmlElement *root );
    int addParcel( TiXmlElement *root );
};

typedef struct mikaCeNetworkListenerHolder
{
  mikaCeNetworkListener *listener;
  unsigned char prebuffer[_MIKA_CE_PRELISTENBUFFER];
  unsigned char buffer[_MIKA_CE_LISTENBUFFER];
  int newsocket;
} mikaCeNetworkListenerHolder;

#endif
