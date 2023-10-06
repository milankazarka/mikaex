#include "mikaCeNetwork.h"
#include "config.h"
#include "mikaCe.h"
#include "mikaNetRequest.h"

mikaCeNetwork::mikaCeNetwork( )
{
}

mikaCeNetwork::~mikaCeNetwork( )
{
}

mikaCeNetworkListener::mikaCeNetworkListener( int _port )
{
  if (!dce)
  {
    #ifdef _DEBUG
      printf("mikaCeNetworkListener::mikaCeNetworkListener error mikaCe not initialized\n");
    #endif
  }
  port = _port;
}

mikaCeNetworkListener::~mikaCeNetworkListener( )
{
}

/** add an object into the parcel cluster
 */
int mikaCeNetworkListener::addObject( TiXmlElement *root )
{
  static TiXmlElement *objectElement;
  static const char *attr = NULL;
  #ifdef _DEBUG
    printf("mikaCeNetworkListener::addObject\n");
  #endif
  objectElement = root->FirstChildElement("mikaCeObject");
  if (objectElement)
  {
    mikaCeObject *object = new mikaCeObject();
    
    attr = objectElement->Attribute("x");	if (attr) object->frame.x = atoi(attr);
    attr = objectElement->Attribute("y");	if (attr) object->frame.y = atoi(attr);
    attr = objectElement->Attribute("w");	if (attr) object->frame.w = atoi(attr);
    attr = objectElement->Attribute("h");	if (attr) object->frame.h = atoi(attr);
    
    dce->addObject(object);
  }
  
  return 0;
}

/** remove an object of an ID
 */
int mikaCeNetworkListener::rmObject( TiXmlElement *root )
{
  static TiXmlElement *objectElement;
  static const char *attr = NULL;
  #ifdef _DEBUG
    printf("mikaCeNetworkListener::rmObject\n");
  #endif
  objectElement = root->FirstChildElement("mikaCeObject");
  if (objectElement)
  {
    attr = objectElement->Attribute("id");
    if (attr)
    {
      mikaCeObject *object = dce->objectAtId((char*)attr);
      if (object)
      {
        dce->rmObject(object);
      }
    }
  }
  return 0;
}

/** move an object
 */
int mikaCeNetworkListener::mvObject( TiXmlElement *root )
{
  static TiXmlElement *objectElement;
  static const char *attr = NULL;
  #ifdef _DEBUG
    printf("mikaCeNetworkListener::mvObject\n");
  #endif
  
  struct mikaFrame newFrame;
  objectElement = root->FirstChildElement("mikaCeObject");
  if (objectElement)
  {
    attr = objectElement->Attribute("x");	if (attr) newFrame.x = atoi(attr);
    attr = objectElement->Attribute("y");	if (attr) newFrame.y = atoi(attr);
    attr = objectElement->Attribute("w");	if (attr) newFrame.w = atoi(attr);
    attr = objectElement->Attribute("h");	if (attr) newFrame.h = atoi(attr);
  
    attr = objectElement->Attribute("id");
    if (attr)
    {
      mikaCeObject *object = dce->objectAtId((char*)attr);
      if (object)
      {
        dce->mvObject(object,&newFrame);
      }
    }
  }
  
  return 0;
}

/** add a parcel into the parcel cluster
 */
int mikaCeNetworkListener::addParcel( TiXmlElement *root )
{
  static TiXmlElement *parcelElement;
  static const char *attr = NULL;
  #ifdef _DEBUG
    printf("mikaCeNetworkListener::addParcel\n");
  #endif
  parcelElement = root->FirstChildElement("mikaCeParcel");
  if (parcelElement)
  {
    mikaCeParcel *parcel = new mikaCeParcel();
    
    attr = parcelElement->Attribute("x");	if (attr) parcel->frame.x = atoi(attr);
    attr = parcelElement->Attribute("y");	if (attr) parcel->frame.y = atoi(attr);
    attr = parcelElement->Attribute("w");	if (attr) parcel->frame.w = atoi(attr);
    attr = parcelElement->Attribute("h");	if (attr) parcel->frame.h = atoi(attr);
    
    dce->addParcel(parcel);
  }
  return 0;
}

/** process a mikaCe message
 */
int mikaCeNetworkListener::processMessage( unsigned char *messageBuffer )
{
  static TiXmlDocument doc;
  static TiXmlElement *root = NULL;
  static char *attr = NULL;

  if (!messageBuffer)
  {
    #ifdef _DEBUG
      printf("mikaCeNetworkListener::processMessage error invalid arguments\n");
    #endif
    return 1;
  }
  
  doc.Parse((const char*)messageBuffer,0,TIXML_ENCODING_UTF8);
  root = doc.FirstChildElement("mikaCe");
  if (root)
  {
    attr = (char*)root->Attribute("type");
    if (attr)
    {
      if (strcmp(attr,"addObject")==0)
        addObject(root);
      else if (strcmp(attr,"rmObject")==0)
        rmObject(root);
      else if (strcmp(attr,"mvObject")==0)
        mvObject(root);
      else if (strcmp(attr,"addParcel")==0)
        addParcel(root);
    }
  }
  else
  {
    #ifdef _DEBUG
      printf("mikaCeNetworkListener::processMessage error invalid message root\n");
    #endif
    return 1;
  }
  
  return 0;
}

/** connection thread
 */
void *mikaCeNetworkListenerConnectionThread(void*data)
{
  struct mikaCeNetworkListenerHolder *holder = (mikaCeNetworkListenerHolder*)data;
  mikaCeNetworkListener *listener = holder->listener;
  int newsocket = holder->newsocket;
  unsigned char out[4096*10];

  int red, total, objsize = 0;
  struct sockaddr_storage cli_addr;
  socklen_t clilen;
  clilen = sizeof(cli_addr);
  
  total = 0;
  holder->buffer[0] = 0x00;
  strcpy((char*)holder->buffer,"");
  while(1)
  {
    objsize = 0;
    strcpy((char*)holder->buffer,"");
    // read request
    while(1)
    {
      red = read(newsocket,(unsigned char*)holder->prebuffer,_MIKA_CE_PRELISTENBUFFER);
      if (red<1)
        break;
      
      memcpy((char*)holder->buffer+total,(char*)holder->prebuffer,red);
      total += red;
    
      if (strstr((char*)holder->prebuffer,"</mikaCe>"))
      {
        break;
      }
    }
    if (total>0)
    {
      holder->buffer[total] = 0x00;
      #ifdef _DEBUG
        printf("mikaCeNetworkListenerThread in(%s)\n",(char*)holder->buffer);
      #endif
      listener->processMessage((unsigned char*)holder->buffer);
    }
    
    // respond to request
    sprintf((char*)out,"<mikaCe></mikaCe>\n");
    write(newsocket,(char*)out,strlen((char*)out));
  }
    
  dce->lock();
    
  dce->unlock();
  close(newsocket);
}

/** connection thread trigger
 */
int mikaCeNetworkListener::connection( int newsocket )
{
  pthread_t t;
  struct mikaCeNetworkListenerHolder *holder = 
    (struct mikaCeNetworkListenerHolder*)malloc(sizeof(struct mikaCeNetworkListenerHolder));
  holder->listener = this;
  holder->newsocket = newsocket;
  pthread_create(&t,NULL,mikaCeNetworkListenerConnectionThread,holder);
  
  return 0;
}

/** thread handling new connections
 */
void *mikaCeNetworkListenerThread(void*data)
{
  #ifdef _DEBUG
    printf("mikaCeNetworkListenerThread\n");
  #endif
  if (!data)
  {
    #ifdef _DEBUG
      printf("mikaCeNetworkListenerThread error invalid data\n");
    #endif
    return NULL;
  }
  mikaCeNetworkListener *dcnl = (mikaCeNetworkListener*)data;
  int newsocket, red, total;
  struct sockaddr_storage cli_addr;
  socklen_t clilen;
  clilen = sizeof(cli_addr);
  
  while(1)
  {
    newsocket = accept(dcnl->sock,(struct sockaddr*)&cli_addr,&clilen);
    dcnl->connection(newsocket);
  }
}

/** binds the socket and starts the listener thread
 */
int mikaCeNetworkListener::Listen( )
{
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock<0)
  {
    #ifdef _DEBUG
      printf("mikaCeNetworkListener::listen error socket couldn't be created\n");
    #endif
    return 1;
  }
  
  struct sockaddr_in sin;
  int ret, n;
  memset((char*)&sin,0,sizeof(struct sockaddr_in));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  sin.sin_addr.s_addr = INADDR_ANY;
  
  for(n = 0; n < 20; n++)
  {
    ret = bind(sock,(struct sockaddr*)&sin,sizeof(sin));
    if (ret>-1)
      break;
    else
    {
      #ifdef _DEBUG
        printf("mikaCeNetworkListener::listen binding try(%d)\n",n);
      #endif
    }
    sleep(1);
  }
  
  if (ret<0)
  {
    #ifdef _DEBUG
      printf("mikaCeNetworkListener::listen couldn bind\n");
    #endif
    return 1;
  }
  
  if (listen(sock,10)<0)
  {
    #ifdef _DEBUG
      printf("mikaCeNetworkListener::listen could start listening\n");
    #endif
    return 1;
  }
  
  pthread_t t;
  pthread_create(&t,NULL,mikaCeNetworkListenerThread,this);
  
  return 0;
}
