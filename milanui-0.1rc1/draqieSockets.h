/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _DRAQIE_SOCKETS_H_
#define _DRAQIE_SOCKETS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define _DRAQIE_SOCKETS_MSG 4096
#define _DRAQIE_SOCKETS_DEFA_PORT 46000
#define _DRAQIE_SOCKETS_DEFA_SERVER "127.0.0.1"

typedef struct draqieSockets
{
  unsigned char serverName[512];
  int port;
  unsigned char out[_DRAQIE_SOCKETS_MSG];
  unsigned char in[_DRAQIE_SOCKETS_MSG];
} draqieSockets;

struct draqieSockets *draqieSocketsNew( );
int draqieSocketsSend( struct draqieSockets *ds );

#endif
