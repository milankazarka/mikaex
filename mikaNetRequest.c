/** generic fetching of messages over the network
 *
 */
#include "mikaNetRequest.h"
#include "mikaNetCurl.h"
#include "config.h"

#define _SMALL_BUFFER 4096

mikaNetRequest::mikaNetRequest( )
{
  initCurl();
}

mikaNetRequest::~mikaNetRequest( )
{
}

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

/** send a single request handshake (send/receive)
 */
int mikaNetRequest::sendRequest( struct mikaHost *host, unsigned char *message )
{
  if (!host || !message)
  {
    #ifdef _DEBUG
      printf("mikaNetRequest::sendRequest error invalid arguments\n");
    #endif
    return 1;
  }
  
  #ifdef _DEBUG
    printf("mikaNetRequest::sendRequest message(%s)\n",(char*)message);
  #endif
  
  curl = curl_easy_init();
  const char *request = (const char*)message;
  if (host->name[0]!=0x00)
    curl_easy_setopt(curl, CURLOPT_URL,(char*)host->name);
  else
  {
    #ifdef _DEBUG
      printf("mikaNetRequest::sendRequest error invalid host name\n");
    #endif
    curl_easy_cleanup(curl);
    return 1;
  }
  curl_easy_setopt(curl, CURLOPT_PORT, host->port);
  curl_socket_t sockfd;
  long sockextr;
  CURLcode res;
  
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 4);
  curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
  curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
  curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1L);
  res = curl_easy_perform(curl);
  
  if (CURLE_OK != res)
  {
    #ifdef _DEBUG
      printf("mikaNetRequest::sendRequest error couldn't setup connection\n");
    #endif
    curl_easy_cleanup(curl);
    return 1;
  }
  
  res = curl_easy_getinfo(curl, CURLINFO_LASTSOCKET, &sockextr);
  sockfd = sockextr;
  
  if(!wait_on_socket(sockfd, 0, 60000L))
  {
    #ifdef _DEBUG
      printf("Error: timeout.\n");
    #endif
    return 1;
  }
  
  size_t iolen;
  res = curl_easy_send(curl, (char*)request, strlen((char*)request), &iolen);
  
  curl_easy_cleanup(curl);
  
  return 0;
}

/** send a request to a server and receive response
 *
 */
int mikaNetRequest::handshakeRequest( struct mikaHost *host, unsigned char *message, unsigned char *reply, int buflen )
{
  if (!host || !message || !reply)
  {
    #ifdef _DEBUG
      printf("mikaNetRequest::handshakeRequest error invalid arguments\n");
    #endif
    return 1;
  }
  
  curl = curl_easy_init();
  const char *request = (const char*)message;
  if (host->name[0]!=0x00)
    curl_easy_setopt(curl, CURLOPT_URL,(char*)host->name);
  else
  {
    #ifdef _DEBUG
      printf("mikaNetRequest::handshakeRequest error invalid host name\n");
    #endif
    curl_easy_cleanup(curl);
    return 1;
  }
  curl_easy_setopt(curl, CURLOPT_PORT, host->port);
  curl_socket_t sockfd;
  long sockextr;
  CURLcode res;
  
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 4);
  curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
  curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
  curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1L);
  res = curl_easy_perform(curl);
  
  if (CURLE_OK != res)
  {
    #ifdef _DEBUG
      printf("mikaNetRequest::handshakeRequest error couldn't setup connection\n");
    #endif
    curl_easy_cleanup(curl);
    return 1;
  }
  
  res = curl_easy_getinfo(curl, CURLINFO_LASTSOCKET, &sockextr);
  sockfd = sockextr;
  
  if(!wait_on_socket(sockfd, 0, 60000L))
  {
    #ifdef _DEBUG
      printf("Error: timeout.\n");
    #endif
    return 1;
  }
  
  size_t iolen;
  res = curl_easy_send(curl, (char*)request, strlen((char*)request), &iolen);
  
  strcpy((char*)reply,"");
  static unsigned char buf[_SMALL_BUFFER];
  int total = 0;
  for(;;)
  {
    wait_on_socket(sockfd, 1, 60000L);
    res = curl_easy_recv(curl, buf, _SMALL_BUFFER, &iolen);
    
    if (CURLE_OK != res)
      break;
      
    total += iolen;
    strncat((char*)reply,(char*)buf,iolen);
    reply[total] = 0x00;
  }
  
  #ifdef _DEBUG
    printf("mikaNetRequest::handshakeRequest reply(%s)\n",(char*)reply);
  #endif
  
  curl_easy_cleanup(curl);
  
  return 0;
}
