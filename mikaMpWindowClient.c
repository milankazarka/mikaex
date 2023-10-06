#include "mikaMpWindowClient.h"
#include "mikaMpDesktopServer.h"
#include "config.h"

mikaMpWindowClient::mikaMpWindowClient( struct mikaHost *host )
{
  myApp = NULL;
  if (host)
    myHost = *host;
  netRequest = new mikaNetRequest( );
}

/** initialize passing it a mikaApp object - it's UID is used
 *  in identifying this Client
 */
mikaMpWindowClient::mikaMpWindowClient( mikaApp *app, struct mikaHost *host )
{
  if (!app)
  {
    #ifdef _DEBUG
      printf("mikaMpWindowClient:mikaMpWindowClient error, !app\n");
    #endif
  }
  
  myApp = app;
  
  if (host)
    myHost = *host;
  netRequest = new mikaNetRequest( );
}

mikaMpWindowClient::~mikaMpWindowClient( )
{
  delete netRequest;
}

int mikaMpWindowClient::sendMessageData( unsigned char *data )
{
  if (!data)
  {
    #ifdef _DEBUG
      printf("mikaMpWindowClient::sendMessageData error invalid argument\n");
    #endif
    return 1;
  }
  
  netRequest->sendRequest(&myHost,data);
  
  return 0;
}
