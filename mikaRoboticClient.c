#include "mikaRoboticClient.h"

#define _MIKA_ROBOTIC_STATIC_NO
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
  
  server = gethostbyname(_MIKA_ROBOTIC_SERVER);
  
  bcopy(
    (char*)server->h_addr,
    (char*)&addr.sin_addr.s_addr,
    server->h_length
    );
    
  addr.sin_family = AF_INET;
  addr.sin_port = htons(_MIKA_ROBOTIC_PORT);
  
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

struct mikaRoboticClient *mikaRoboticClientNew( )
{
  struct mikaRoboticClient *myClient = NULL;
  myClient = (struct mikaRoboticClient*)malloc(sizeof(struct mikaRoboticClient));
  return myClient;
}

int mikaRoboticClientSendCommand( struct mikaRoboticClient *myClient, unsigned char *command )
{
  static unsigned char complete[1024];
  if (!myClient || !command)
    return 1;
  
  sprintf((char*)complete,"<msg>\n<mod>\nroboticArm\n</mod>\n<command>\n%s\n</command>\n</msg>\n",command);
  #ifdef _DEBUG
    printf("mikaRoboticClientSendCommand cmd(%s)\n",(char*)complete);
  #endif
  _sockets_send((unsigned char*)complete);
  return 0;
}

#ifdef _MIKA_ROBOTIC_STATIC
  static struct mikaRoboticClient *client;
  
  int main( )
  {
    client = mikaRoboticClientNew();
    mikaRoboticClientSendCommand(client,(unsigned char*)"test");
    
    return 0;
  }
#endif
