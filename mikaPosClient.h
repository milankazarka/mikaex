/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _MIKA_POS_CLIENT_
#define _MIKA_POS_CLIENT_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define _MIKA_POS_COMMAND_SIZE 4096
#define _MIKA_POS_ORDER_SIZE 1024
#define _MIKA_POS_SERVER "127.0.0.1"
#define _MIKA_POS_PORT 46000

enum _MIKA_POS_COMMANDS
{
  _MIKA_POS_COMMAND_NONE,
  _MIKA_POS_COMMAND_ADD
};

typedef struct mikaPosCommand
{
  int type;
  unsigned char value[_MIKA_POS_COMMAND_SIZE];
} mikaPosCommand;

typedef struct mikaPosOrder
{
  int table;
  unsigned char value[_MIKA_POS_ORDER_SIZE];
} mikaPosOrder;

typedef struct mikaPosClient
{
  struct mikaPosCommand command;
  unsigned char server[128];
  int port;
} mikaPosClient;

int mikaPosClientInit( struct mikaPosClient *client );
int mikaPosClientSendCommand( struct mikaPosClient *client );
int mikaPosClientAddOrder( struct mikaPosClient *client, struct mikaPosOrder *order );
int mikaPosClientCommandBegin( struct mikaPosClient *client );
int mikaPosClientCommandEnd( struct mikaPosClient *client );
int mikaPosClientCommandAddType( struct mikaPosClient *client, int type );
int mikaPosClientCommandAddDevice( struct mikaPosClient *client, int device );
int mikaPosClientCommandAddOrder( struct mikaPosClient *client, struct mikaPosOrder *order );

#endif
