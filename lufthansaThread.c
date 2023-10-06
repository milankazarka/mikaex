#include "mikaServer.h"

/**
 * custom connection handler when receiving Lufthansa mobile client requests
 */
void *_call_connection_processing_custom( struct mikaServer *server, struct mikaConnection *connection )
{
  #ifdef DEBUG
    printf("_call_connection_processing_custom\n");
  #endif

  if (!server || !connection)
    return NULL;

  mikaConnectionClientGet(connection);

  if (connection->msg->inBuffer)
  {
    struct mikaDom *dom = mikaDomInit(NULL);
    mikaDomParseData(dom,connection->msg->inBuffer);
    
    #ifdef DEBUG
      printf("_call_connection_processing_custom process dom\n");
    #endif
    
    sprintf((char*)connection->msg->outBuffer,"<response>\n</response>\n");
    
    // main processing of request
    
    mikaDomRelease(dom);
  }
  
  if (connection->msg->outBuffer)
    mikaConnectionClientPut(connection);
}
