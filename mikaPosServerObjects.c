/** this implements reading in the objects out of
 *  which the POS command consists parsing them
 *
 *  We rather not put any more complex operations
 *  in these classes for simplicity reasons
 *
 */
#include "mikaPosServerObjects.h"
#include "config.h"

#define _POS_DETAIL_SIZE 4096
#ifndef _IDS_LINE
  #define _IDS_LINE 64
#endif

/** get the ID of an object
 *
 */
int mikaPosServerObjectsGetID( TiXmlElement *item )
{
  if (!item) return -1;
  
  int id = -1;
  char *id_str = (char*)item->Attribute("id");
  if (id_str)
    id = atoi(id_str);
  return id;
}

/** increment an IDS file
 */
int mikaPosServerObjectsIncrementIDS( unsigned char *filename )
{
  if (!filename) return -1;
  static unsigned char line[_IDS_LINE];
  line[0] = 0x00;
  int newID = -1;
  FILE *idfd = fopen((char*)filename,"r");
  if (idfd)
  {
    fgets((char*)line,_IDS_LINE-1,idfd);
    if (strlen((char*)line)>0)
    {
      if (line[strlen((char*)line)-1]=='\n')
        line[strlen((char*)line)-1]=0x00;
      newID = atoi((char*)line);
      newID++;
    }
    fclose(idfd);
  }
  if (newID!=-1)
  {
    idfd = fopen((char*)filename,"w");
    if (idfd)
    {
      fclose(idfd);
    }
  }
  
  return newID;
}

mikaPosServerItem::mikaPosServerItem( TiXmlElement *item )
{
  #ifdef _DEBUG
    printf("mikaPosServerItem::mikaPosServerItem\n");
  #endif

  char *detailStr = (char*)item->Attribute("detail");
  if (detailStr)
  {
    int len = strlen(detailStr);
    if (len>_POS_DETAIL_SIZE-2)
      len = _POS_DETAIL_SIZE-2;
    strncpy((char*)detail,detailStr,len);
    detail[len] = 0x00;
    #ifdef _DEBUG
      printf("mikaPosServerItem::mikaPosServerItem detail(%s)\n",
        detail);
    #endif
  }

  next = NULL;
}

mikaPosServerItem::~mikaPosServerItem( )
{
}

/** fill the order with it's setup and Item objects
 *
 * <order state="new">
 *   <items>
 *   </items>
 * </order>
 */
mikaPosServerOrder::mikaPosServerOrder( TiXmlElement *order )
{
  #ifdef _DEBUG
    printf("mikaPosServerOrder::mikaPosServerOrder\n");
  #endif
  
  next = NULL;
  items = NULL;
  order_id = mikaPosServerObjectsGetID(order);
  order_state = _DEFAULT_ORDER_STATE_NONE;
  
  char *state_str = (char*)order->Attribute("state");
  if (state_str)
  {
    if (strcmp(state_str,"new")==0) 		order_state = _DEFAULT_ORDER_STATE_NEW;
    else if (strcmp(state_str,"accepted")==0)	order_state = _DEFAULT_ORDER_STATE_ACCEPTED;
    else if (strcmp(state_str,"prepaired")==0)	order_state = _DEFAULT_ORDER_STATE_PREPAIRED;
    else if (strcmp(state_str,"served")==0)	order_state = _DEFAULT_ORDER_STATE_SERVED;
    else if (strcmp(state_str,"finalized")==0)	order_state = _DEFAULT_ORDER_STATE_FINALIZED;
  }
  
  TiXmlElement *itemsObject = order->FirstChildElement("items");
  if (itemsObject)
  {
    TiXmlElement *item = itemsObject->FirstChildElement("item");
    if (item)
    {
      mikaPosServerItem *itemObject = new mikaPosServerItem(item);
      if (itemObject)
      {
        itemObject->next = NULL;
        items = itemObject;
        while(item = item->NextSiblingElement())
        {
          itemObject = new mikaPosServerItem(item);
          itemObject->next = items;
          items = itemObject;
        }
      }
    }
  }
  else
  {
    #ifdef _DEBUG
      printf("mikaPosServerOrder::mikaPosServerOrder warning no items in order\n");
    #endif
  }
}

mikaPosServerOrder::~mikaPosServerOrder( )
{
  mikaPosServerItem *current = items;
  mikaPosServerItem *nextItem = NULL;
  while(current)
  {
    nextItem = current->next;
    delete current;
    current = nextItem;
  }
}

/** fill the appliance with it's setup and it's orders
 */
mikaPosServerAppliance::mikaPosServerAppliance( TiXmlElement *appliance )
{
  #ifdef _DEBUG
    printf("mikaPosServerAppliance::mikaPosServerAppliance\n");
  #endif

  orders = NULL;
  appliance_id = mikaPosServerObjectsGetID(appliance);
  
  TiXmlElement *ordersRoot = appliance->FirstChildElement("orders");
  if (ordersRoot)
  {
    TiXmlElement *order = ordersRoot->FirstChildElement("order");
    if (order)
    {
      mikaPosServerOrder *orderObject = new mikaPosServerOrder(order);
      if (orderObject)
      {
        orderObject->next = NULL;
        orders = orderObject;
        while(order = order->NextSiblingElement())
        {
          orderObject = new mikaPosServerOrder(order);
          orderObject->next = orders;
          orders = orderObject;
        }
      }
    }
    else
    {
      #ifdef _DEBUG
        printf("mikaPosServerAppliance::mikaPosServerAppliance warning no order present\n");
      #endif
    }
  }
}

int mikaPosServerAppliance::saveAsNew( struct mikaDb *db, unsigned char *idsFilename )
{
  if (!db || !idsFilename)
  {
    #ifdef _DEBUG
      printf("mikaPosServerAppliance::saveAsNew error invalid arguments\n");
    #endif
    return 1;
  }
  
  return 0;
}

mikaPosServerAppliance::~mikaPosServerAppliance( )
{
  mikaPosServerOrder *current = orders;
  mikaPosServerOrder *nextOrder = NULL;
  while(current)
  {
    nextOrder = current->next;
    delete current;
    current = nextOrder;
  }
}
