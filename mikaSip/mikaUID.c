/**  
  Copyright (C) 2012 Milan Kazarka
  milan.kazarka.office@gmail.com
      
  You may distribute under the terms of the Artistic License, 
  as specified in the README file. You can find a copy of the
  license at LICENSE.
*/
#include "mikaUID.h"
#include "config.h"

#define _TEST

mikaUID::mikaUID( )
{
  #ifdef _TEST
    static int run = 0;
  #endif
  #ifdef _DEBUG
    printf("mikaUID::mikaUID\n");
  #endif
  uuid = (unsigned char*)malloc(64);
  if (!uuid)
  {
    #ifdef _DEBUG
      printf("mikaUID::mikaUID error could't allocate uuid\n");
    #endif
  }
  uuid_t id;
  uuid_generate_random(id);
  if (!id)
  {
    #ifdef _DEBUG
      printf("mikaUID::mikaUID couldn't retrieve uuid\n");
    #endif
    exit(2);
  }
  memcpy((void*)uuid,id,sizeof(uuid_t));
  uuid[sizeof(uuid_t)] = 0x00;
  
  unsigned char tmp[64];
  memcpy((void*)tmp,(void*)uuid,sizeof(uuid_t));
  tmp[sizeof(uuid_t)] = 0x00;
  sprintf((char*)uuid,"%08x-%04x-%04x-%04x-%012x",(char*)tmp);
  #ifdef _TEST
    printf("mikaUID run(%d) uuid(%s)\n",run,(char*)uuid);
    run++;
  #endif
}

mikaUID::~mikaUID( )
{
  #ifdef _DEBUG
    printf("mikaUID::~mikaUID\n");
  #endif
  free(uuid);
}
