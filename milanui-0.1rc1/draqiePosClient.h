/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _DRAQIE_POS_CLIENT_
#define _DRAQIE_POS_CLIENT_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define _DRAQIE_POS_COMMAND_SIZE 4096
#define _DRAQIE_POS_ORDER_SIZE 1024
#define _DRAQIE_POS_SERVER "127.0.0.1"
#define _DRAQIE_POS_PORT 46000

enum _DRAQIE_POS_COMMANDS
{
  _DRAQIE_POS_COMMAND_NONE,
  _DRAQIE_POS_COMMAND_ADD
};

typedef struct draqiePosCommand
{
  int type;
  unsigned char value[_DRAQIE_POS_COMMAND_SIZE];
} draqiePosCommand;

typedef struct draqiePosOrder
{
  int table;
  unsigned char value[_DRAQIE_POS_ORDER_SIZE];
} draqiePosOrder;

typedef struct draqiePosClient
{
  struct draqiePosCommand command;
} draqiePosClient;

int draqiePosClientInit( struct draqiePosClient *client );
int draqiePosClientSendCommand( struct draqiePosClient *client );
int draqiePosClientAddOrder( struct draqiePosClient *client, struct draqiePosOrder *order );
int draqiePosClientCommandBegin( struct draqiePosClient *client );
int draqiePosClientCommandEnd( struct draqiePosClient *client );
int draqiePosClientCommandAddType( struct draqiePosClient *client, int type );
int draqiePosClientCommandAddDevice( struct draqiePosClient *client, int device );
int draqiePosClientCommandAddOrder( struct draqiePosClient *client, struct draqiePosOrder *order );

#endif
