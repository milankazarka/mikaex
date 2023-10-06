/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "draqieSockets.h"
#include "mikaServer.h"

struct draqieSockets *draqieSocketsNew( )
{
  struct draqieSockets *ds = (struct draqieSockets*)malloc(sizeof(struct draqieSockets));
  ds->port = _DRAQIE_SOCKETS_DEFA_PORT;
  strcpy((char*)ds->serverName,_DRAQIE_SOCKETS_DEFA_SERVER);
  return ds;
}

int draqieSocketsSend( struct draqieSockets *ds )
{
  if (!ds)
    return 1;
  
  #ifdef _DEBUG
    printf("draqieSocketsSend\n");
  #endif
  
  struct sockaddr_in addr;
  struct hostent *server;
  int sockfd;  
  
  sockfd = socket( AF_INET, SOCK_STREAM, 0 );  
  
  if (sockfd<0)
    return 1;
  
  memset(&addr,0,sizeof(addr));
  
  server = gethostbyname((char*)ds->serverName);
  
  bcopy(
    (char*)server->h_addr,
    (char*)&addr.sin_addr.s_addr,
    server->h_length
    );
    
  addr.sin_family = AF_INET;
  addr.sin_port = htons(ds->port);
  
  int conn = connect(sockfd,(struct sockaddr*)&addr,sizeof(addr));
  if (!conn)
  {
    int data = send(sockfd,(char*)ds->out,strlen((char*)ds->out),0);
    #ifdef _DEBUG
      printf("draqieSocketsSend data(%d)\n",data);
    #endif
    
    unsigned char tmp[512];
    strcpy((char*)ds->out,"");
    while(1)
    {
      int red = read(sockfd,(unsigned char*)tmp,511);
      if (red<1)
        break;
      strcat((char*)ds->in,(char*)tmp);
      if (strstr((char*)tmp,MSGENDING))
      {
        break;
      }
    }
    
    close(sockfd);
  }
  else
  {
    printf("draqieSocketsSend no connection\n");
  }
  
  return 0;
}
