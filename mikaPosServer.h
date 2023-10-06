#ifndef _MIKA_POS_SERVER_H_
#define _MIKA_POS_SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "mikaServer.h"
#include "mikaServerMod.h"
#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"
#include "mikaDb.h"
#include "mikaPosServerObjects.h"

class mikaPosServer {
  public:
    struct mikaDb *billsDb;
    struct mikaDb *ordersDb;
    struct mikaDb *orderStatesDb;
    struct mikaDb *itemsDb;
    pthread_mutex_t mutex;
    
    mikaPosServer( );
    ~mikaPosServer( );
    
    void lock();
    void unlock();

    void resetIds( );
    int commitOrder( mikaPosServerOrder *order );
    int setOrderID( mikaPosServerOrder *order);
    int recordOrderID( mikaPosServerOrder *order );
    int newOrder( struct mikaServer *server, struct mikaServerMod *serverMod, 
                struct mikaConnection *connection, TiXmlElement *command );
    int modOrder( struct mikaServer *server, struct mikaServerMod *serverMod, 
                struct mikaConnection *connection, TiXmlElement *command );
    int rmOrder( struct mikaServer *server, struct mikaServerMod *serverMod, 
                struct mikaConnection *connection, TiXmlElement *command );
    int getAppliance( struct mikaServer *server, struct mikaServerMod *serverMod,
                struct mikaConnection *connection, TiXmlElement *command );
    /** get a list of all the orders
     */
    int getAllOrders( struct mikaServer *server, struct mikaServerMod *serverMod,
                struct mikaConnection *connection, TiXmlElement *command );
};

#endif
