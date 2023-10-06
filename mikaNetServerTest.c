#include "mikaNetServer.h"
#include "common.h"
#include "config.h"
#define _STATIC

void *onRequest( void *connectionObject )
{
  #ifdef _DEBUG
    printf("onRequest\n");
  #endif
  if (!connectionObject)
    return NULL;
  struct mikaNetConnection *connection = (mikaNetConnection*)connectionObject;
}

#ifdef _STATIC
  struct mikaNetConnectionDelegate connectionDelegate;
  
  int main( )
  {
    connectionDelegate.onRequest = onRequest;
    mikaNetServer *server = new mikaNetServer(46001);
    server->customConnectionDelegate = &connectionDelegate;
    server->start();
    return 0;
  }
#endif
