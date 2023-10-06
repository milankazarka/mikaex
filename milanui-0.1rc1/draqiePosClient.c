/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */
#include "draqiePosClient.h"

#define _DRAQIE_POS_STATIC_NO
#ifndef _DEBUG
  #define _DEBUG
#endif

/** prototype of a POS ordering client
 */

int _sockets_send( unsigned char *msg )
{
  if (!msg)
    return 1;
  
  #ifdef _DEBUG
    printf("_sockets_send\n");
  #endif
  
  struct sockaddr_in addr;
  struct hostent *server;
  int sockfd;  
  
  sockfd = socket( AF_INET, SOCK_STREAM, 0 );  
  
  if (sockfd<0)
    return 1;
  
  memset(&addr,0,sizeof(addr));
  
  server = gethostbyname(_DRAQIE_POS_SERVER);
  
  bcopy(
    (char*)server->h_addr,
    (char*)&addr.sin_addr.s_addr,
    server->h_length
    );
    
  addr.sin_family = AF_INET;
  addr.sin_port = htons(_DRAQIE_POS_PORT);
  
  int conn = connect(sockfd,(struct sockaddr*)&addr,sizeof(addr));
  if (!conn)
  {
    int data = send(sockfd,(char*)msg,strlen((char*)msg),0);
    #ifdef _DEBUG
      printf("_sockets_send data(%d)\n",data);
    #endif
    close(sockfd);
  }
  else
  {
    printf("_sockets_send no connection\n");
  }
  
  return 0;
}

int draqiePosClientInit( struct draqiePosClient *client )
{
  if (!client)
    return 1;
  client->command.type = _DRAQIE_POS_COMMAND_NONE;
  client->command.value[0] = 0x00;
  return 0;
}

int draqiePosClientSendCommand( struct draqiePosClient *client )
{
  if (!client)
    return 1;
  return 0;
}

int draqiePosClientCommandBegin( struct draqiePosClient *client )
{
  if (!client)
    return 1;
  
  strcpy((char*)client->command.value,"<msg>\n<mod>\ndraqiePos\n</mod>\n");
  
  return 0;
}

int draqiePosClientCommandEnd( struct draqiePosClient *client )
{
  if (!client)
    return 1;
  strcat((char*)client->command.value,"</msg>\n");
  return 0;
}

int draqiePosClientCommandAddType( struct draqiePosClient *client, int type )
{
  if (!client)
    return 1;
  
  switch(type)
  {
    case _DRAQIE_POS_COMMAND_ADD:
      strcat((char*)client->command.value,"<command>\nADD\n</command>\n");
    break;
  }
  
  return 0;
}

int draqiePosClientCommandAddDevice( struct draqiePosClient *client, int device )
{
  if (!client)
    return 1;
  
  unsigned char tmp[256];
  sprintf((char*)tmp,"<device>\n%d\n</device>\n",device);
  strcat((char*)client->command.value,(char*)tmp);
  
  return 0;
}

int draqiePosClientCommandAddOrder( struct draqiePosClient *client, struct draqiePosOrder *order )
{
  if (!client || !order)
    return 1;
  
  if (order->value[0] == 0x00)
    return 1;
  
  draqiePosClientCommandBegin(client);
  draqiePosClientCommandAddType(client,_DRAQIE_POS_COMMAND_ADD);
  draqiePosClientCommandAddDevice(client,order->table);
  unsigned char tmp[_DRAQIE_POS_ORDER_SIZE];
  sprintf((char*)tmp,"<order>\n%s\n</order>\n",(char*)order->value);
  strcat((char*)client->command.value,(char*)tmp);
  
  strcpy((char*)client->command.value,(char*)order->value);
  
  #ifdef _DEBUG
  //  printf("draqiePosClientCommandAddOrder msg(\n%s)\n",(char*)client->command.value);
  #endif
  _sockets_send((unsigned char*)client->command.value);
  
  return 0;
}

#ifdef _DRAQIE_POS_STATIC
  static struct draqiePosClient client;
  static struct draqiePosOrder order;
  
  int main( )
  {
    draqiePosClientInit(&client);
    strcpy((char*)order.value,"cocacola #1");
    order.table = 22;
    draqiePosClientCommandAddOrder(&client,&order);
    return 0;
  }
#endif
