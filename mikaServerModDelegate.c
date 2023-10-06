#include "mikaServerModDelegate.h"
#include "mikaServerMod.h"
#include "mikaServer.h"
#include "config.h"

void *onHandshakeDefault( void *myServerMod, void *myServer, void *myConnection )
{
  #ifdef _DEBUG
    printf("onHandshakeDefault\n");
  #endif
}

struct mikaServerModDelegate *mikaServerModDelegateInit( struct mikaServerModDelegate *delegate )
{
  if (!delegate)	
    delegate = (struct mikaServerModDelegate*)malloc(sizeof(struct mikaServerModDelegate));
  
  delegate->onHandshake = onHandshakeDefault;
  
  return delegate;
}
