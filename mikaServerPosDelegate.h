#ifndef _MIKA_SERVER_POS_DELEGATE_H_
#define _MIKA_SERVER_POS_DELEGATE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"

typedef struct mikaServerPosDelegate {
  void *(*onHandshake)(void *myServerMod, void *myServer, void *myConnection);
} mikaServerPosDelegate;

struct mikaServerPosDelegate *mikaServerPosDelegateInit( struct mikaServerPosDelegate *posDelegate );
void *mikaServerPosDelegateOnHandshake( void *myServerMod, void *myServer, void *myConnection );
int mikaServerPosDelegateOnCommand( struct mikaServerMod *serverMod, struct mikaServer *server, struct mikaConnection *connection, TiXmlElement *command );

int mikaServerPosDelegateOnNewOrder( struct mikaServerMod *serverMod, struct mikaServer *server, struct mikaConnection *connection );
int mikaServerPosDelegateOnNewBill( struct mikaServerMod *serverMod, struct mikaServer *server, struct mikaConnection *connection );
int mikaServerPosDelegateOnModOrder( struct mikaServerMod *serverMod, struct mikaServer *server, struct mikaConnection *connection );
int mikaServerPosDelegateOnRmOrder( struct mikaServerMod *serverMod, struct mikaServer *server, struct mikaConnection *connection );
int mikaServerPosDelegateOnFinBill( struct mikaServerMod *serverMod, struct mikaServer *server, struct mikaConnection *connection );

#endif
