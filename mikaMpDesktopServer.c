/** managing connections to the Mp Desktop
 *
 */
#include "mikaMpDesktopServer.h"
#include "mikaHost.h"
#include "config.h"

mikaMpDesktopServer *mpServerGlobal = NULL;

/** parse the message sent to this server
 */
int mikaMpDesktopServer::onMessageRoot( TiXmlElement *root )
{
  #ifdef _DEBUG
    printf("mikaMpDesktopServer::onMessageRoot\n");
  #endif
  if (!root)
    return 1;
  
  TiXmlElement *commands = root->FirstChildElement("commands");
  if (commands)
  {
  }
  
  return 0;
}

/** this is called by the 'handshake' method of mikaNetConnection -
 *  here we react on the data of the connection and respond
 *
 *  <mikaMpWindowClient>
 *	<commands>
 *          <command></command>
 *	</commands>
 *  </mikaMpWindowClient>
 *
 */
void *onRequest( void *connectionObject )
{
  #ifdef _DEBUG
    printf("onRequest\n");
  #endif
  if (!connectionObject)
    return NULL;
  struct mikaNetConnection *connection = (mikaNetConnection*)connectionObject;
  #ifdef _DEBUG
    printf("onRequest buffers");
    debugAllBuffers(connection->firstInBuffer);
    printf("\n");
  #endif
  
  int totalSize = 0;
  totalSize = connection->inBuffersSize();
  if (totalSize>0)
  {
    unsigned char *data = (unsigned char*)malloc(totalSize+1);
    connection->returnInData(data);
    data[totalSize] = 0x00;
    #ifdef _DEBUG
      printf("onRequest size(%d) data(%s)\n",totalSize,(char*)data);
    #endif
    TiXmlDocument doc;
    doc.Parse((const char*)data,0,TIXML_ENCODING_UTF8);
    TiXmlElement *root = doc.FirstChildElement("mikaMpWindowClient");
    if (root)
    {
      if (mpServerGlobal)
        mpServerGlobal->onMessageRoot(root);
    }
    else
    {
      #ifdef _DEBUG
        printf("onRequest error no root object in data\n");
      #endif
    }
    free(data);
  }
}

void *mikaMpDesktopServerThread( void *data )
{
  #ifdef _DEBUG
    printf("mikaMpDesktopServerThread\n");
  #endif
  if (!data)
    return NULL;
  mikaMpDesktopServer *desktopServer = (mikaMpDesktopServer*)data;
  desktopServer->server->start();
}

int mikaMpDesktopServer::runServerThread( )
{
  pthread_create(&serverThread,NULL,mikaMpDesktopServerThread,this);
  return 0;
}

mikaMpDesktopServer::mikaMpDesktopServer( )
{
  #ifdef _DEBUG
    printf("mikaMpDesktopServer::mikaMpDesktopServer\n");
  #endif
  apps = NULL;
  connectionDelegate.onRequest = onRequest;
  server = new mikaNetServer(_MIKA_MP_DESKTOP_SERVER_PORT);
  server->customConnectionDelegate = &connectionDelegate;
  runServerThread();
}

mikaMpDesktopServer::~mikaMpDesktopServer( )
{
}

int mikaMpDesktopServerGlobalInit( )
{
  if (mpServerGlobal)
    return 1;
  mpServerGlobal = new mikaMpDesktopServer();
  return 0;
}

