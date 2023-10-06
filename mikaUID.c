#include "mikaUID.h"
#include "config.h"

mikaUID::mikaUID( )
{
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
  uuid_generate(id);
  memcpy((void*)uuid,id,sizeof(uuid_t));
  uuid[sizeof(uuid_t)] = 0x00;
  #ifdef _DEBUG
    printf("mikaUID::mikaUID uid(%08x-%04x-%04x-%04x-%012x)\n",(char*)uuid);
  #endif
  unsigned char tmp[64];
  memcpy((void*)tmp,(void*)uuid,sizeof(uuid_t));
  tmp[sizeof(uuid_t)] = 0x00;
  sprintf((char*)uuid,"%08x-%04x-%04x-%04x-%012x",(char*)tmp);
}

mikaUID::~mikaUID( )
{
  #ifdef _DEBUG
    printf("mikaUID::~mikaUID\n");
  #endif
  free(uuid);
}
