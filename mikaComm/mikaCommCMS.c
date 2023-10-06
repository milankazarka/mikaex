#include "mikaCommCMS.h"
#include "mikaCommHttp.h"
#include "config.h"
#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"

mikaCommCMS::mikaCommCMS( struct mikaHost *_host )
{
  host = *_host;
  netRequest = new mikaNetRequest();
  requestMessage[0] = 0x00;
  responseMessage[0] = 0x00;
}

mikaCommCMS::~mikaCommCMS( )
{
  delete netRequest;
}

int mikaCommCMS::readSetup( )
{
  return 0;
}

/** get the config file working on low level sockets
 *
 */
void mikaCommCMS::getConfig( )
{
  #ifdef _DEBUG
    printf("mikaCommCMS::getConfig\n");
  #endif
  sprintf((char*)requestMessage,
    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n\
    <request>\n\
    </request>\n"
    );  
  netRequest->handshakeRequest(&host,
    (unsigned char*)requestMessage,
    (unsigned char*)responseMessage,_NET_REQUEST_INBUFFER);
}

/** get the config file using libCurl
 *
 */
int mikaCommCMS::getConfigEx( )
{
  return 0;
}
