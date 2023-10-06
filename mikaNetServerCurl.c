#include "mikaNetServer.h"
#include "config.h"

#define _USE_CURL_NO
#define _TIMEOUT 10

void initCurl( )
{
  static int run = 0;
  if (run>0)
    return;
  curl_global_init(CURL_GLOBAL_DEFAULT);
  run++;
}

void deinitCurl( )
{
  static int run = 0;
  if (run>0)
    return;
  curl_global_cleanup();
  run++;
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

static curl_socket_t opensocket(void *clientp,
                                curlsocktype purpose,
                                struct curl_sockaddr *address)
{
  #ifdef _DEBUG
    printf("opensocket\n");
  #endif
  curl_socket_t sockfd = *(curl_socket_t *)clientp;
  /* the actual externally set socket is passed in via the OPENSOCKETDATA
     option */ 
  return sockfd;
}

static int sockopt_callback(void *clientp, curl_socket_t curlfd,
                            curlsocktype purpose)
{
  /* This return code was added in libcurl 7.21.5 */ 
  return CURL_SOCKOPT_ALREADY_CONNECTED;
}

mikaNetServer::mikaNetServer( int port )
{
  myPort = port;
  initCurl();
}

mikaNetServer::~mikaNetServer( )
{
  deinitCurl();
}

int mikaNetServer::start( )
{
  #ifdef _DEBUG
    printf("mikaNetServer::start\n");
  #endif
  sysSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (socket < 0)
  {
    #ifdef _DEBUG
      printf("mikaNetServer::start error couldn't create socket\n");
    #endif
    return 1;
  }
  struct sockaddr_in sin;
  memset((char*)&sin,0,sizeof(struct sockaddr_in));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(myPort);
  sin.sin_addr.s_addr = INADDR_ANY;
  
  int ret = -1;
  int start = time(NULL);
  
  while((time(NULL)-start<=_TIMEOUT) && (ret<0))
  {
    #ifdef _DEBUG
      printf("mikaNetServer::start binding\n");
    #endif
    ret = bind(sysSocket, (struct sockaddr *)&sin, sizeof(sin));
    if (ret<0)
      sleep(1);
  }
  if (ret<0)
  {
    #ifdef _DEBUG
      printf("mikaNetServer::start binding failed\n");
    #endif
    return 1;
  }
  
  ret = listen(sysSocket,10);
  if (ret<0)
  {
    #ifdef _DEBUG
      printf("mikaNetServer::start listen on socket failed\n");
    #endif
    return 1;
  }
  
  struct sockaddr_storage cli_addr;
  int newcd;
  mikaNetConnection *connection;
  socklen_t clilen;
  clilen = sizeof(cli_addr);
  while(1)
  {
    newcd = accept(sysSocket, (struct sockaddr *)&cli_addr, &clilen);
    connection = new mikaNetConnection(newcd);
  }
}

mikaNetConnection::mikaNetConnection( int connection )
{
  #ifdef _DEBUG
    printf("mikaNetConnection::mikaNetConnection\n");
  #endif
  sysConnection = connection;
  buffers = NULL;
  
  #ifdef _USE_CURL
    curl = curl_easy_init();
    //curl_easy_setopt(curl, CURLOPT_URL, "http://99.99.99.99:9999");
    /* no progress meter please */ 
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    /* call this function to get a socket */ 
    curl_easy_setopt(curl, CURLOPT_OPENSOCKETFUNCTION, opensocket);
    curl_easy_setopt(curl, CURLOPT_OPENSOCKETDATA, &sysConnection);
    /* call this function to set options for the socket */ 
    curl_easy_setopt(curl, CURLOPT_SOCKOPTFUNCTION, sockopt_callback);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1L);
    int res = curl_easy_perform(curl);
  #endif
  
  handshake();
  
  close(sysConnection);
  delete this;
}

mikaNetConnection::~mikaNetConnection( )
{
  curl_easy_cleanup(curl);
}

int mikaNetConnection::handshake( )
{
  #ifdef _DEBUG
    printf("mikaNetConnection::handshake\n");
  #endif
  recv();
  return 0;
}

int debugBuffer( struct mikaBuffer *buffer )
{
  if (!buffer)
    return 1;
  
  unsigned char *data = (unsigned char*)malloc(buffer->size+1);
  strncpy((char*)data,(char*)buffer->data,buffer->size);
  data[buffer->size] = 0x00;
  #ifdef _DEBUG
    printf("debugBuffer data(%s)\n",(char*)data);
  #endif
  free(data);  
  
  return 0;
}

#define _SMALL_BUFFER 4096
int mikaNetConnection::recv( )
{
  #ifdef _DEBUG
    printf("mikaNetConnection::recv\n");
  #endif
  
  #ifdef _USE_CURL
    curl_socket_t sockfd;
    long sockextr;
    int res = curl_easy_getinfo(curl, CURLINFO_LASTSOCKET, &sockextr);
    #ifdef _DEBUG
      printf("mikaNetConnection::recv CURLINFO_LASTSOCKET\n");
    #endif
  
    static unsigned char buf[_SMALL_BUFFER];
    size_t iolen;
    int total = 0;
    for(;;)
    {
      wait_on_socket(sockfd, 1, 60000L);
      res = curl_easy_recv(curl, buf, _SMALL_BUFFER, &iolen);
      
      if (CURLE_OK != res)
        break;
      #ifdef _DEBUG
        printf("mikaNetConnection::recv iolen(%d)\n",iolen);
      #endif
    }
  #else
    #ifdef _DEBUG
      printf("mikaNetConnection::recv new buffer\n");
    #endif
    struct mikaBuffer *buffer = (struct mikaBuffer*)malloc(sizeof(struct mikaBuffer));
    buffer->next = NULL;
    buffer->size = 0;
    buffers = buffer;
    unsigned char tmp[_MIKA_BUFFER_SMALL];
    int position = 0;
    int red = 0;
    while(1)
    {
        red = read(sysConnection,(unsigned char*)tmp,_MIKA_BUFFER_SMALL);
        if (red<1)
          break;
        if (buffer->size+red>_MIKA_BUFFER_SIZE)
        {
          debugBuffer(buffer);
          #ifdef _DEBUG
            printf("mikaNetConnection::recv new buffer\n");
          #endif
          buffer = (struct mikaBuffer*)malloc(sizeof(struct mikaBuffer));
          buffer->next = buffers;
          buffer->size = 0;
          buffers = buffer;
          position = 0;
        }
        strncpy((char*)buffer->data+position,(char*)tmp,red);
        buffer->size += red;
        position+=red;
    }
    debugBuffer(buffer);
  #endif
  
  return 0;
}
