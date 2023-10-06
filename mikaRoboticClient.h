#ifndef _MIKA_ROBOTIC_CLIENT_H_
#define _MIKA_ROBOTIC_CLIENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define _MIKA_ROBOTIC_SERVER "192.168.0.106"
#define _MIKA_ROBOTIC_PORT 80

typedef struct mikaRoboticClient
{
  
} mikaRoboticClient;

struct mikaRoboticClient *mikaRoboticClientNew( );
int mikaRoboticClientSendCommand( struct mikaRoboticClient *myClient, unsigned char *command );

#endif
