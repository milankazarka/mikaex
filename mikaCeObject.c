#include "mikaCeObject.h"

int mikaCeObjectCount = 0;

void mikaCeObject::setId( )
{
  unsigned char tmp[64];
  unsigned char countString[64];
  int n;
  int stringLen = 0;
  for(n = 0; n < 8; n++)
    id[n] = '0';
  
  sprintf((char*)countString,"%d",mikaCeObjectCount);
  stringLen = strlen((char*)countString);
  for(n = 0; n < stringLen; n++)
    id[8-stringLen] = countString[n];
  
  mikaCeObjectCount++;
}

mikaCeObject::mikaCeObject( )
{
  test = NULL;
  next = NULL;
  memset((void*)id,0x00,8);
  
  setId();
}

mikaCeObject::~mikaCeObject( )
{
}
