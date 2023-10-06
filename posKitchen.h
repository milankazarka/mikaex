#ifndef _KITCHEN_POS_H_
#define _KITCHEN_POS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include <pthread.h>

#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"

#define _POS_KITCHEN_TIK 100000

typedef struct kitchenPosOrder
{
  unsigned char detail[128];
  struct kitchenPosOrder *next;
} kitchenPosOrder;

typedef struct kitchenPosOrders
{
  struct kitchenPosOrder *order;
} kitchenPosOrders;

typedef struct kitchenPos
{
  int			fetched; // if we already processed this data
  int			active;
  unsigned char 	server[128];
  unsigned char 	username[128];
  unsigned char 	password[128];
  CURL 			*curl;
  CURLcode 		res;
  pthread_mutex_t	mutex;
  pthread_t		queueThread;
  struct kitchenPosOrders orders;
} kitchenPos;

struct kitchenPos *kitchenPosInit( struct kitchenPos *pos );
int kitchenPosConfigure( struct kitchenPos *pos, unsigned char *server, unsigned char *username, unsigned char *password );
int kitchenPosQueue( struct kitchenPos *pos );
int kitchenPosCreateTestList( struct kitchenPos *pos );
int kitchenPosAddOrder( struct kitchenPos *pos, struct kitchenPosOrder *order );
int kitchenPosGetOrders( struct kitchenPos *pos );
int kitchenPosMergeOrders( struct kitchenPos *pos );

#endif
