#include "mikaPosClient.h"

#define _MIKA_POS_STATIC_NO
#ifndef _DEBUG
  #define _DEBUG
#endif

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
  
  server = gethostbyname(_MIKA_POS_SERVER);
  
  bcopy(
    (char*)server->h_addr,
    (char*)&addr.sin_addr.s_addr,
    server->h_length
    );
    
  addr.sin_family = AF_INET;
  addr.sin_port = htons(_MIKA_POS_PORT);
  
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

int mikaPosClientInit( struct mikaPosClient *client )
{
  if (!client)
    return 1;
  client->command.type = _MIKA_POS_COMMAND_NONE;
  client->command.value[0] = 0x00;
  return 0;
}

int mikaPosClientSendCommand( struct mikaPosClient *client )
{
  if (!client)
    return 1;
  return 0;
}

int mikaPosClientCommandBegin( struct mikaPosClient *client )
{
  if (!client)
    return 1;
  
  strcpy((char*)client->command.value,"<msg>\n<mod>\nmikaPos\n</mod>\n");
  
  return 0;
}

int mikaPosClientCommandEnd( struct mikaPosClient *client )
{
  if (!client)
    return 1;
  strcat((char*)client->command.value,"</msg>\n");
  return 0;
}

int mikaPosClientCommandAddType( struct mikaPosClient *client, int type )
{
  if (!client)
    return 1;
  
  switch(type)
  {
    case _MIKA_POS_COMMAND_ADD:
      strcat((char*)client->command.value,"<command>\nADD\n</command>\n");
    break;
  }
  
  return 0;
}

int mikaPosClientCommandAddDevice( struct mikaPosClient *client, int device )
{
  if (!client)
    return 1;
  
  unsigned char tmp[256];
  sprintf((char*)tmp,"<device>\n%d\n</device>\n",device);
  strcat((char*)client->command.value,(char*)tmp);
  
  return 0;
}

int mikaPosClientCommandAddOrder( struct mikaPosClient *client, struct mikaPosOrder *order )
{
  if (!client || !order)
    return 1;
  
  if (order->value[0] == 0x00)
    return 1;
  
  mikaPosClientCommandBegin(client);
  mikaPosClientCommandAddType(client,_MIKA_POS_COMMAND_ADD);
  mikaPosClientCommandAddDevice(client,order->table);
  unsigned char tmp[_MIKA_POS_ORDER_SIZE];
  sprintf((char*)tmp,"<order>\n%s\n</order>\n",(char*)order->value);
  strcat((char*)client->command.value,(char*)tmp);
  
  mikaPosClientCommandEnd(client);
  #ifdef _DEBUG
    printf("mikaPosClientCommandAddOrder msg(\n%s)\n",(char*)client->command.value);
  #endif
  _sockets_send((unsigned char*)client->command.value);
  
  return 0;
}

#ifdef _MIKA_POS_STATIC
  static struct mikaPosClient client;
  static struct mikaPosOrder order;
  
  int main( )
  {
    mikaPosClientInit(&client);
    strcpy((char*)order.value,"cocacola #1");
    order.table = 22;
    mikaPosClientCommandAddOrder(&client,&order);
    return 0;
  }
#endif
