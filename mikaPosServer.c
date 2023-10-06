/** this is an implementation of a POS server for Mika appliances
 *  and affiliated products
 */
#include "mikaPosServer.h"
#include "config.h"

#define _IDS_LINE 64
#define _ORDERS_IDS_FILE "pos/orders.ids"
#define _BILLS_IDS_FILE "pos/bills.ids"
#define _ITEMS_IDS_FILE "pos/items.ids"

void mikaPosServer::lock( )
{
  pthread_mutex_lock(&mutex);
}

void mikaPosServer::unlock( )
{
  pthread_mutex_unlock(&mutex);
}

/**  - create the mutexes
 *   - create the databases
 *
 */
mikaPosServer::mikaPosServer( )
{
  pthread_mutex_init(&mutex,NULL);
  
  FILE *dbf = NULL;
  if ((dbf = fopen("pos/bills.db","r"))!=NULL)	billsDb = mikaDbInit(NULL,(unsigned char*)"pos/bills.db");
  else						billsDb = mikaDbCreate(NULL,(unsigned char*)"pos/bills.db");
  
  if ((dbf = fopen("pos/orders.db","r"))!=NULL)	ordersDb = mikaDbInit(NULL,(unsigned char*)"pos/orders.db");
  else						ordersDb = mikaDbCreate(NULL,(unsigned char*)"pos/orders.db");
  
  if ((dbf = fopen("pos/orderStates.db","r"))!=NULL)	orderStatesDb = mikaDbInit(NULL,(unsigned char*)"pos/orderStates.db");
  else							orderStatesDb = mikaDbCreate(NULL,(unsigned char*)"pos/orderStates.db");  
    
  if ((dbf = fopen("pos/items.db","r"))!=NULL)	itemsDb = mikaDbInit(NULL,(unsigned char*)"pos/items.db");
  else						itemsDb = mikaDbCreate(NULL,(unsigned char*)"pos/items.db");
}

mikaPosServer::~mikaPosServer( )
{
}

/** reset the ID's of all databases
 *
 */
void mikaPosServer::resetIds( )
{
  #ifdef _DEBUG
    printf("mikaPosServer::resetIds\n");
  #endif
}

/** commit the order to the database
 */
int mikaPosServer::commitOrder( 
  struct mikaPosServerOrder *order
  )
{
  if (!order)
  {
    #ifdef _DEBUG
      printf("mikaPosServer::commitOrder error invalid arguments\n");
    #endif
    return 1;
  }
  
  #ifdef _DEBUG
    printf("mikaPosServer::commitOrder key(%d) value(%d)\n",
      order->order_id,order->order_state);
  #endif
  
  static unsigned char key[_IDS_LINE];
  static unsigned char value[_IDS_LINE];
  sprintf((char*)key,"%d",order->order_id);
  sprintf((char*)value,"%d",order->order_state);
  mikaDbPut(orderStatesDb,(unsigned char*)key,(unsigned char*)value);
  
  return 0;
}

/** set a new order ID making it a number (+1) higher
 *  than the last recorded ID
 *
 */
int mikaPosServer::setOrderID( mikaPosServerOrder *order )
{
  if (!order)
  {
    #ifdef _DEBUG
      printf("mikaPosServer::incrementOrder\n");
    #endif
    return 1;
  }
  
  int order_id = -1;
  static unsigned char line[_IDS_LINE];
  FILE *idfd = fopen(_ORDERS_IDS_FILE,"r");
  if (idfd)
  {
    fgets((char*)line,_IDS_LINE-1,idfd);
    if (line[strlen((char*)line)-1]=='\n')
      line[strlen((char*)line)-1]=0x00;
    order_id = atoi((char*)line);
    #ifdef _DEBUG
      printf("mikaPosServer::newOrder orders.ids last(%d)\n",
        order_id);
    #endif
    
    order_id++;
    #ifdef _DEBUG
      printf("mikaPosServer::newOrder orders.ids recording(%d)\n",
        order_id);
    #endif
    fclose(idfd);
    idfd = fopen(_ORDERS_IDS_FILE,"w");
    if (idfd)
    {
      sprintf((char*)line,"%d\n",order_id);
      fwrite((void*)line,strlen((char*)line),1,idfd);
      fclose(idfd);
    }
    order->order_id = order_id;
  }
  
  return 0;
}

/** record order id with item details
 *
 */
int mikaPosServer::recordOrderID( mikaPosServerOrder *order )
{
  if (!order)
  {
    #ifdef _DEBUG
      printf("mikaPosServer::recordOrderID error invalid arguments\n");
    #endif
    return 1;
  }
  
  static unsigned char key[_IDS_LINE];
  static unsigned char value[_IDS_LINE];
  sprintf((char*)key,"%d",order->order_id);
  mikaPosServerItem *posItem = order->items;
  if (posItem)
    sprintf((char*)value,"%s",(char*)posItem->detail);
    
  mikaDbPut(ordersDb,(unsigned char*)key,(unsigned char*)value);
  // check for the commit outcome:
  value[0] = 0x00;
  mikaDbGet(ordersDb,(unsigned char*)key,(unsigned char*)value);
  #ifdef _DEBUG
    printf("mikaPosServer::newOrder backwards test(%s)\n",
      (char*)value);
  #endif
  
  return 0;
}

/**
 *   <command type="NewOrder">
 *    <appliance>
 *     <orders>
 *      <order>
 *       <items>
 *         <item detail="Coca-Cola 0.33" ?id="33">
 *         </item>
 *       </items>
 *      </order>
 *     </orders>
 *    </appliance>
 *   </command>
 */
int mikaPosServer::newOrder(
  struct mikaServer *server, struct mikaServerMod *serverMod,
  struct mikaConnection *connection, TiXmlElement *command
  )
{
  if (!server || !serverMod || !connection || !command)
  {
    #ifdef _DEBUG
      printf("mikaPosServer::newOrder error invalid arguments\n");
    #endif
    return 1;
  }
  
  #ifdef _DEBUG
    printf("mikaPosServer::newOrder\n");
  #endif
  
  int appliance_id = -1;
  
  mikaPosServerAppliance *posAppliance = NULL;
  TiXmlElement *appliance = command->FirstChildElement("appliance");
  if (appliance)
  {
    posAppliance = new mikaPosServerAppliance(appliance);
    if (posAppliance)
      appliance_id = posAppliance->appliance_id;
    else
      return 1;
  }
  else
  {
    #ifdef _DEBUG
      printf("mikaPosServer::newOrder <appliance> missing\n");
    #endif
    return 1;
  }
  
  lock();
    
  mikaPosServerItem *posItem = NULL;
  mikaPosServerOrder *posOrder = NULL;
  posOrder = posAppliance->orders;
  while(posOrder)
  {
    setOrderID(posOrder);	// set order ID
    recordOrderID(posOrder);	// record order, id & item details
    commitOrder(posOrder);	// commit order state
    posItem = posOrder->items;
    while(posItem)
    {
      if (!posItem->next) break;
      posItem = posItem->next;
    }
    posOrder = posOrder->next;
  }
  
  delete posAppliance;
  unlock();
    
  return 0;
}

/** remove an orders or list of orders
 */
int mikaPosServer::rmOrder(
  struct mikaServer *server, struct mikaServerMod *serverMod,
  struct mikaConnection *connection, TiXmlElement *command
  )
{
  if (!server || !serverMod || !connection || !command)
  {
    #ifdef _DEBUG
      printf("mikaPosServer::rmOrder error invalid arguments\n");
    #endif
    return 1;
  }
  #ifdef _DEBUG
    printf("mikaPosServer::rmOrder\n");
  #endif
  
  unsigned char key[_IDS_LINE];
  mikaPosServerAppliance *posAppliance = NULL;
  mikaPosServerOrder *posOrder = NULL;
  TiXmlElement *appliance = command->FirstChildElement("appliance");
  if (appliance)
  {
    posAppliance = new mikaPosServerAppliance(appliance);
    if (!posAppliance)
      return 1;
    else
    {
      posOrder = posAppliance->orders;
      while(posOrder)
      {
        sprintf((char*)key,"%d",posOrder->order_id);
        mikaDbRm(ordersDb,(unsigned char*)key);
        mikaDbRm(orderStatesDb,(unsigned char*)key);
        posOrder = posOrder->next;
      }
      delete posAppliance;
    }
  }
  
  return 0;
}

/** modify an orders status
 */
int mikaPosServer::modOrder(
  struct mikaServer *server, struct mikaServerMod *serverMod,
  struct mikaConnection *connection, TiXmlElement *command
  )
{
  mikaPosServerAppliance *posAppliance = NULL;
  mikaPosServerOrder *posOrder = NULL;
  TiXmlElement *appliance = command->FirstChildElement("appliance");
  if (appliance)
  {
    posAppliance = new mikaPosServerAppliance(appliance);
    if (!posAppliance)
      return 1;
    else
    {
      if (posAppliance->orders)
      {
        posOrder = posAppliance->orders;
        while(posOrder)
        {
          commitOrder(posOrder);
          posOrder = posOrder->next;
        }
      }
      delete posAppliance;
    }
  }
      
  return 0;
}

/** retrieve objects of a single appliance
 */
int mikaPosServer::getAppliance(
  struct mikaServer *server, struct mikaServerMod *serverMod,
  struct mikaConnection *connection, TiXmlElement *command
  )
{
  if (!server || !serverMod || !connection || !command)
  {
    #ifdef _DEBUG
      printf("mikaPosServer::getAppliance error invalid arguments\n");
    #endif
    return 1;
  }
  #ifdef _DEBUG
    printf("mikaPosServer::getAppliance\n");
  #endif
  return 0;
}

/** retrieve all orders
 */
int mikaPosServer::getAllOrders(
  struct mikaServer *server, struct mikaServerMod *serverMod,
  struct mikaConnection *connection, TiXmlElement *command
  )
{
  if (!server || !serverMod || !connection || !command)
  {
    #ifdef _DEBUG
      printf("mikaPosServer::newOrder error invalid arguments\n");
    #endif
    return 1;
  }
  #ifdef _DEBUG
    printf("mikaPosServer::getAllOrders\n");
  #endif
  return 0;
}
