#include "posKitchen.h"
#include "config.h"

struct kitchenPos *kitchenPosInit( struct kitchenPos *pos )
{
  if (!pos)
    pos = (struct kitchenPos*)malloc(sizeof(struct kitchenPos));
  
  pos->fetched = 1;
  pos->server[0] = 0x00;
  pos->curl = NULL;
  pos->orders.order = NULL;
  
  pthread_mutex_init(&pos->mutex,NULL);
  
  return pos;
}

int kitchenPosConfigure( struct kitchenPos *pos, unsigned char *server, unsigned char *username, unsigned char *password )
{
  if (!pos || !server || !username || !password)
  {
    #ifdef _DEBUG
      printf("kitchenPosConfigure error invalid arguments\n");
    #endif
    return 1;
  }
  
  strcpy((char*)pos->server,(char*)server);
  strcpy((char*)pos->username,(char*)username);
  strcpy((char*)pos->password,(char*)password);
  
  return 0;
}

void *kitchenPosQueueThread(void*data)
{
  #ifdef _DEBUG
    printf("kitchenPosQueueThread\n");
  #endif
  struct kitchenPos *pos = (struct kitchenPos*)data;
  while(1)
  {
    pthread_mutex_lock(&pos->mutex);
    pos->fetched = 0;
    pthread_mutex_unlock(&pos->mutex);
    usleep(_POS_KITCHEN_TIK);
  }
}

int kitchenPosCreateTestList( struct kitchenPos *pos )
{
  if (!pos)
  {
    #ifdef _DEBUG
      printf("kitchenPosCreateTestList error invalid arguments\n");
    #endif
    return 1;
  }
  
  pthread_mutex_lock(&pos->mutex);
  
  int n;
  for(n = 0; n < 10; n++)
  {
    struct kitchenPosOrder *order = (struct kitchenPosOrder*)malloc(sizeof(struct kitchenPosOrder));
    if (order)
    {
      sprintf((char*)order->detail,"order %d",n);
      kitchenPosAddOrder(pos,order);
    }
  }
  
  pthread_mutex_unlock(&pos->mutex);
  
  return 0;
}

int kitchenPosQueue( struct kitchenPos *pos )
{
  if (!pos)
  {
    #ifdef _DEBUG
      printf("kitchenPosQueue error invalid arguments\n");
    #endif
    return 1;
  }
  
  pthread_create(&pos->queueThread,NULL,kitchenPosQueueThread,pos);
  
  return 0;
}

int kitchenPosAddOrder( struct kitchenPos *pos, struct kitchenPosOrder *order )
{
  if (!pos || !order)
  {
    #ifdef _DEBUG
      printf("kitchenPosAddOrder error invalid arguments\n");
    #endif
    return 1;
  }
  
  order->next = pos->orders.order;
  pos->orders.order = order;
  
  return 0;
}

int kitchenPosGetOrders( struct kitchenPos *pos )
{
  if (!pos)
  {
    #ifdef _DEBUG
      printf("kitchenPosGetOrders error invalid arguments\n");
    #endif
    return 1;
  }
  return 0;
}

int kitchenPosMergeOrders( struct kitchenPos *pos )
{
  if (!pos)
  {
    #ifdef _DEBUG
      printf("kitchenPosMergeOrders error invalid arguments\n");
    #endif
    return 1;
  }
  return 0;
}
