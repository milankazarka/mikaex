/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _MIKA_SOCKETS_H_
#define _MIKA_SOCKETS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define _MIKA_SOCKETS_MSG 4096
#define _MIKA_SOCKETS_DEFA_PORT 46000
#define _MIKA_SOCKETS_DEFA_SERVER "127.0.0.1"

typedef struct mikaSockets
{
  unsigned char serverName[512];
  int port;
  unsigned char out[_MIKA_SOCKETS_MSG];
  unsigned char in[_MIKA_SOCKETS_MSG];
} mikaSockets;

struct mikaSockets *mikaSocketsNew( );
int mikaSocketsSend( struct mikaSockets *ds );

#endif
