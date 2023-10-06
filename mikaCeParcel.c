#include "mikaCeParcel.h"
#include "config.h"

int mikaCeParcelCount = 0;

void mikaCeParcel::setId( )
{
  unsigned char tmp[64];
  unsigned char countString[64];
  int n;
  int stringLen = 0;
  for(n = 0; n < 8; n++)
    id[n] = '0';
    
  sprintf((char*)countString,"%d",mikaCeParcelCount);
  stringLen = strlen((char*)countString);  
  for(n = 0; n < stringLen; n++)
    id[8-stringLen] = countString[n];
  
  strncpy((char*)tmp,(char*)id,8);
  tmp[8] = 0x00;
  #ifdef _DEBUG
    printf("mikaCeParcel::setId id(%s)\n",(char*)tmp);
  #endif
  
  mikaCeParcelCount++;
}

mikaCeParcel::mikaCeParcel( )
{
  int n;
  frame.x = 0;
  frame.y = 0;
  frame.w = 0;
  frame.h = 0;
  frame.active = 1;
  memset((void*)id,0x00,8);
  next = NULL;
  
  setId();
}

mikaCeParcel::mikaCeParcel( struct mikaFrame _frame )
{
  int n;
  frame = _frame;
  memset((void*)id,0x00,8);
  next = NULL;
  
  setId();
}

mikaCeParcel::~mikaCeParcel( )
{
}
