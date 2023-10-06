/** these are simple classes which handle socket connections
 *  and manage processing of the requests
 */
#include "mikaNetServer.h"
#include "mikaNetCurl.h"
#include "config.h"

#define _USE_CURL_NO
#define _TIMEOUT 10
#define _SOCKET_TIMEOUT 1

/** wait on a non-blocking socket
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

mikaNetServer::mikaNetServer( int port )
{
  myPort = port;
  customConnectionDelegate = NULL;
}

mikaNetServer::~mikaNetServer( )
{
}

void *defaultRequestProcessing( void *connectionObject )
{
  #ifdef _DEBUG
    printf("defaultRequestProcessing\n");
  #endif
  if (!connectionObject)
    return NULL;
  mikaNetConnection *connection = (mikaNetConnection*)connectionObject;
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
  
  struct mikaNetConnectionDelegate delegate;
  delegate.onRequest = defaultRequestProcessing;
  struct sockaddr_storage cli_addr;
  int newcd;
  mikaNetConnection *connection;
  socklen_t clilen;
  clilen = sizeof(cli_addr);
  while(1)
  {
    newcd = accept(sysSocket, (struct sockaddr *)&cli_addr, &clilen);
    connection = new mikaNetConnection(newcd);
    /** the connection delegate handles the processing of
     *  the request and generates the reply
     */
    if (customConnectionDelegate)
      connection->delegate = customConnectionDelegate;
    else
      connection->delegate = &delegate;
    connection->handshake();
    delete connection;
  }
}

mikaNetConnection::mikaNetConnection( int connection )
{
  #ifdef _DEBUG
    printf("mikaNetConnection::mikaNetConnection\n");
  #endif
  sysConnection = connection;
  /** set socket to non-blocking mode
   */
  int x;
  x=fcntl(sysConnection,F_GETFL,0);
  fcntl(sysConnection,F_SETFL,x | O_NONBLOCK);
  
  inBuffers = NULL;
  outBuffers = NULL;
  firstInBuffer = NULL;
  firstOutBuffer = NULL;
  fastBuffer[0] = 0x00;
}

void releaseBuffers( struct mikaBuffer *buffers )
{
  struct mikaBuffer *current = NULL;
  struct mikaBuffer *next = NULL;
  current = buffers;
  while(current)
  {
    next = current->next;
    free(current);
    current = next;
  }
}

/** close the connection and release all buffers
 */
mikaNetConnection::~mikaNetConnection( )
{
  #ifdef _DEBUG
    printf("mikaNetConnection::~mikaNetConnection\n");
  #endif
  close(sysConnection);
  releaseBuffers(inBuffers);
  releaseBuffers(outBuffers);
}

/** running the handshake routine
 */
int mikaNetConnection::handshake( )
{
  #ifdef _DEBUG
    printf("mikaNetConnection::handshake\n");
  #endif
  myRecv();
  if (delegate)
    if (delegate->onRequest)
      delegate->onRequest((void*)this);
  reply();
  return 0;
}

/** return the complete size of the data in the buffers
 *
 */
int mikaNetConnection::inBuffersSize( )
{
  int size = 0;
  struct mikaBuffer *current = inBuffers;
  while(current)
  {
    size += current->size;
    current = current->next;
  }
  return size;
}

int mikaNetConnection::outBuffersSize( )
{
  int size = 0;
  struct mikaBuffer *current = outBuffers;
  while(current)
  {
    size += current->size;
    current = current->next;
  }
  return size;
}

/** add data to the out buffers, spliting it if needed
 */
int mikaNetConnection::outBuffersAddData( unsigned char *data )
{
  if (!data)
  {
    #ifdef _DEBUG
      printf("mikaNetConnection::outBuffersAddData error invalid argument\n");
    #endif
    return 1;
  }
  
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
    printf("%s",(char*)data);
  #endif
  free(data);  
  
  return 0;
}

int debugAllBuffers( struct mikaBuffer *firstBuffer )
{
  #ifdef _DEBUG
    printf("debugAllBuffers data(");
  #endif
  if (!firstBuffer)
    return 1;
  
  struct mikaBuffer *current = firstBuffer;
  while(current)
  {
    debugBuffer(current);
    current = current->last;
  }
  #ifdef _DEBUG
    printf(")\n");
  #endif
  
  return 0;
}

/** fill the data with the contents of all buffers - this merges them
 *  into one data buffer. You first need to get the size to allocate
 *  the right size of 'data'
 */
int mergeBuffers( struct mikaBuffer *firstBuffer, unsigned char *data )
{
  if (!firstBuffer || !data)
    return 1;
  struct mikaBuffer *current = firstBuffer;
  while(current)
  {
    memcpy(data,(void*)current->data,current->size);
    data+=current->size;
    current = current->last;
  }
  return 0;
}

/** return the data in the inBuffers, eg. merge the buffers
 */
int mikaNetConnection::returnInData( unsigned char *data )
{
  if (!data)
    return 1;
  if (mergeBuffers(firstInBuffer,data)!=0)
    return 1;
  return 0;
}

int mikaNetConnection::returnOutData( unsigned char *data )
{
  if (!data)
    return 1;
  if (mergeBuffers(firstOutBuffer,data)!=0)
    return 1;
  return 0;
}

/** receive data from socket and store it in a list
 *  of buffers
 */
int mikaNetConnection::myRecv( )
{ 
  #ifdef _DEBUG
    printf("mikaNetConnection::recv new buffer\n");
  #endif
  struct mikaBuffer *buffer = (struct mikaBuffer*)malloc(sizeof(struct mikaBuffer));
  buffer->next = NULL;
  buffer->last = NULL;
  buffer->size = 0;
  inBuffers = buffer;
  firstInBuffer = inBuffers;
  unsigned char tmp[_MIKA_BUFFER_SMALL];
  int position = 0;
  int red = 0;
  while(1)
  {
    #ifdef _DEBUG
      printf("mikaNetConnection::myRecv wait_on_socket\n");
    #endif
    wait_on_socket(sysConnection, 1, _SOCKET_TIMEOUT*1000L);
    if (1) red = read(sysConnection,(unsigned char*)tmp,_MIKA_BUFFER_SMALL);
    if (0) red = recv(sysConnection,(unsigned char*)tmp,_MIKA_BUFFER_SMALL,0);
    if (red<1)
      break;
    if (buffer->size+red>_MIKA_BUFFER_SIZE)
    {
      debugBuffer(buffer);
      #ifdef _DEBUG
        printf("mikaNetConnection::recv new buffer\n");
      #endif
      buffer = (struct mikaBuffer*)malloc(sizeof(struct mikaBuffer));
      buffer->next = inBuffers;
      buffer->last = NULL;
      buffer->size = 0;
      inBuffers->last = buffer;
      inBuffers = buffer;
      position = 0;
    }
    strncpy((char*)buffer->data+position,(char*)tmp,red);
    buffer->size += red;
    position+=red;
  }
  debugBuffer(buffer);
  debugAllBuffers(firstInBuffer);
  
  return 0;
}

/** reply the client with the data from the mikaBuffer list
 */
int mikaNetConnection::reply( )
{
  #ifdef _DEBUG
    printf("mikaNetConnection::send\n");
  #endif
  
  if (fastBuffer[0]!=0x00)
  {
    write(sysConnection,(void*)fastBuffer,strlen((char*)fastBuffer));
  }
  else
  {
    struct mikaBuffer *current = firstOutBuffer;
    while(current)
    {
      write(sysConnection,(void*)current->data,current->size);
      current = current->last;
    }
  }
  return 0;
}
