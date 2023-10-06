/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */
#include "mikaPosClient.h"
#include "connectionTest.h"

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#define _MIKA_POS_STATIC_NO
#ifndef _DEBUG
  #define _DEBUG
#endif

#define _POS_CURL 	// use CURL for networking

/** prototype of a POS ordering client
 */

static int wait_on_socket(curl_socket_t sockfd, int for_recv, long timeout_ms)
{
  struct timeval tv;
  fd_set infd, outfd, errfd;
  int res;
       
  tv.tv_sec = timeout_ms / 1000;
  tv.tv_usec= (timeout_ms % 1000) * 1000;
            
  FD_ZERO(&infd);
  FD_ZERO(&outfd);
  FD_ZERO(&errfd);
                   
  FD_SET(sockfd, &errfd); /* always check for error */ 
                      
  if(for_recv)
  {
    FD_SET(sockfd, &infd);
  }
  else
  {
    FD_SET(sockfd, &outfd);
  }
                                           
  /* select() returns the number of signalled sockets or -1 */ 
  res = select(sockfd + 1, &infd, &outfd, &errfd, &tv);
  return res;
}

/** simply send a message to the server
 */
int _sockets_send( struct mikaPosClient *client, unsigned char *msg )
{
  if (!msg)
    return 1;
  
  #ifdef _DEBUG
    printf("mikaPosClient send to(%s) port(%d) data(%s)\n",
      (char*)client->server,client->port,(char*)msg);
    printf("_sockets_send\n");
  #endif
  
  #ifdef _POS_CURL
    #ifdef _DEBUG
      printf("mikaPosClient _POS_CURL\n");
    #endif
    static CURL *curl = NULL;
    //if (!curl)
    {
      curl_global_init(CURL_GLOBAL_DEFAULT);
      curl = curl_easy_init();
    }
    
    const char *request = (const char*)msg;
    if (client->server[0]!=0x00)
      curl_easy_setopt(curl, CURLOPT_URL, (char*)client->server);
    else
      curl_easy_setopt(curl, CURLOPT_URL, _MIKA_POS_SERVER);
    curl_easy_setopt(curl, CURLOPT_PORT, client->port);
    
    curl_socket_t sockfd;
    long sockextr;
    CURLcode res;
    
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 4);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);  
    curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1L);
    res = curl_easy_perform(curl);
                 
    if(CURLE_OK != res)
    {
      printf("mikaPosClient connect_only Error: %s\n", strerror(res));
      return 1;
    }
                         
    res = curl_easy_getinfo(curl, CURLINFO_LASTSOCKET, &sockextr);
                                                                  
    if(CURLE_OK != res)
    {
      printf("mikaPosClient last_socket Error: %s\n", curl_easy_strerror(res));
      return 1;
    }
                                                                                           
    sockfd = sockextr;
                                                                                            
    if(!wait_on_socket(sockfd, 0, 60000L))
    {
      printf("Error: timeout.\n");
      return 1;
    }
                                                                                                                                              
    size_t iolen;
    res = curl_easy_send(curl, (char*)request, strlen((char*)request), &iolen);
    
    if (CURLE_OK != res)
    {
      #ifdef _DEBUG
        printf("mikaPosClient couldn't send data\n");
      #endif
    }
    else
    {
      #ifdef _DEBUG
        printf("mikaPosClient send message OK\n");
      #endif
    }
    
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    
    return 0;
  #else
  
  struct sockaddr_in addr;
  struct hostent *server;
  int sockfd;  
  
  sockfd = socket( AF_INET, SOCK_STREAM, 0 );  
  
  if (sockfd<0)
  {
    #ifdef _DEBUG
      printf("mikaPosClient error couldn't create socket\n");
    #endif
    return 1;
  }
  
  #ifdef _DEBUG
    printf("mikaPosClient step01\n");
  #endif
  
  if (client->server[0]!=0x00)
    server = gethostbyname((char*)client->server);
  else
    server = gethostbyname(_MIKA_POS_SERVER);
  
  #ifdef _DEBUG
    printf("mikaPosClient step02\n");
  #endif
  
  if (!server)
  {
    #ifdef _DEBUG
      printf("mikaPosClient error resolving address\n");
    #endif
    return 1;
  }
  
  bcopy(
    (char*)server->h_addr,
    (char*)&addr.sin_addr.s_addr,
    server->h_length
    );
    
  addr.sin_family = AF_INET;
  if (client->port!=0)
    addr.sin_port = htons(client->port);
  else
    addr.sin_port = htons(_MIKA_POS_PORT);
  
  #ifdef _DEBUG
    printf("mikaPosClient before send\n");
  #endif
  
  int conn = connect(sockfd,(struct sockaddr*)&addr,sizeof(addr));
  if (!conn)
  {
    int data = send(sockfd,(char*)msg,strlen((char*)msg),0);
    #ifdef _DEBUG
      printf("mikaPosClient _sockets_send data(%d)\n",data);
    #endif
    close(sockfd);
  }
  else
  {
    printf("mikaPosClient _sockets_send no connection to %s\n",(char*)client->server);
  }
  #endif
  
  return 0;
}

int mikaPosClientInit( struct mikaPosClient *client )
{
  if (!client)
    return 1;
  client->command.type = _MIKA_POS_COMMAND_NONE;
  client->command.value[0] = 0x00;
  client->server[0] = 0x00;
  client->port = 0;
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

/** simply send a message order described through the value attribute
 */
#define _SIMPLE_ORDER
int mikaPosClientCommandAddOrder( struct mikaPosClient *client, struct mikaPosOrder *order )
{
  #ifdef _DEBUG
    printf("mikaPosClientCommandAddOrder\n");
  #endif

  if (!client || !order)
  {
    #ifdef _DEBUG
      printf("mikaPosClientCommandAddOrder ERROR invalid arguments\n");
    #endif
    return 1;
  }
  
  if (order->value[0] == 0x00)
  {
    #ifdef _DEBUG
      printf("mikaPosClientCommandAddOrder ERROR value not set\n");
    #endif
    return 1;
  }
  
  #ifdef _SIMPLE_ORDER
    sprintf((char*)client->command.value,"%s",(char*)order->value);
  #else
    mikaPosClientCommandBegin(client);
    mikaPosClientCommandAddType(client,_MIKA_POS_COMMAND_ADD);
    mikaPosClientCommandAddDevice(client,order->table);
    unsigned char tmp[_MIKA_POS_ORDER_SIZE];
    sprintf((char*)tmp,"<order>\n%s\n</order>\n",(char*)order->value);
    strcat((char*)client->command.value,(char*)tmp);
  #endif
  
  #ifdef _DEBUG
    printf("mikaPosClientCommandAddOrder msg(%s)\n",
      (char*)client->command.value);
    if (client->server[0]!=0x00 && client->port != 0)
    {
      printf("mikaPosClientCommandAddOrder server(%s) port(%d)\n",
        (char*)client->server,client->port);
    }
  #endif
  _sockets_send(client,(unsigned char*)client->command.value);
  
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
