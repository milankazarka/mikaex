/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "mikaServer.h"

void *_call_connection_mika_pos( struct mikaServer *server, struct mikaConnection *connection )
{
  #ifdef _DEBUG
    printf("_call_connection_mika_pos\n");
  #endif
  
  if (!server || !connection)
    return NULL;
  
  mikaConnectionClientGet(connection);
  
  if (connection->msg->inBuffer)
  { }
  
  /**  changed to the mod paradigm
   */
  
  return NULL;
}
