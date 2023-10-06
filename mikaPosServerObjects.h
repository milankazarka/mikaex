#ifndef _MIKA_POS_SERVER_OBJECTS_H_
#define _MIKA_POS_SERVER_OBJECTS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"
#include "mikaDb.h"

class mikaPosServerItem {
  public:
    mikaPosServerItem *next;
    int item_id;
    unsigned char detail[4096];
    
    mikaPosServerItem( TiXmlElement *item );
    ~mikaPosServerItem( );
};

enum _DEFAULT_ORDER_STATES {
  _DEFAULT_ORDER_STATE_NONE = 0,
  _DEFAULT_ORDER_STATE_NEW = 1,
  _DEFAULT_ORDER_STATE_ACCEPTED = 2,
  _DEFAULT_ORDER_STATE_PREPAIRED = 3,
  _DEFAULT_ORDER_STATE_SERVED = 4,
  _DEFAULT_ORDER_STATE_FINALIZED = 5
};

class mikaPosServerOrder {
  public:
    mikaPosServerItem *items;
    mikaPosServerOrder *next;
    int order_id;
    int order_state;
    
    mikaPosServerOrder( TiXmlElement *order );
    ~mikaPosServerOrder( );
};

class mikaPosServerAppliance {
  public:
    mikaPosServerOrder *orders;
    int appliance_id;
  
    mikaPosServerAppliance( TiXmlElement *appliance );
    ~mikaPosServerAppliance( );
    int saveAsNew( struct mikaDb *db, unsigned char *idsFilename );
};

#endif
