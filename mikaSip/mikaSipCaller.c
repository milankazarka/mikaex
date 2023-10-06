#include "mikaSipCaller.h"

mikaSipCaller::mikaSipCaller( unsigned char *number )
{
  myNumber = (unsigned char*)malloc(strlen((char*)number)+1);
  strcpy((char*)myNumber,(char*)number);
}

mikaSipCaller::~mikaSipCaller( )
{
  if (myNumber)
    free(myNumber);
}

int mikaSipCaller::call( )
{
  unsigned char *command = NULL;

  if (!myNumber)
  {
    printf("mikaSipCaller::call error, no number defined\n");
    return 1;
  }
  
  printf("mikaSipCaller::call %s\n",(char*)myNumber);
  command = (unsigned char*)malloc(strlen((char*)myNumber)+256);
  sprintf((char*)command,"baresip -p %s",(char*)myNumber);
  system((char*)command);
  return 0;
}
