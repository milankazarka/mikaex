#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/un.h>

#define _UDS_PATH "webkitapp.sock"

int main( )
{
  int 			sockfd;
  struct sockaddr_un 	address;
  size_t 		address_len;
  
  sockfd = socket(PF_UNIX,SOCK_STREAM,0);
  if (sockfd<0)
  {
    printf("webkitapp couldn't create socket\n");
    return NULL;
  }
  address.sun_family = AF_UNIX;
  address_len = sizeof(address.sun_family) + sprintf(address.sun_path,_UDS_PATH);
  if (connect(sockfd,(struct sockaddr*)&address,address_len) != 0)
  {
    printf("webkitappclient couldn't connect\n");
    return 1;
  }
  
  unsigned char msg[128];
  sprintf((char*)msg,"123 178\n");
  write(sockfd,(char*)msg,strlen((char*)msg));
  
  close(sockfd);

  return 0;
}
