#include "configClient.h"

#define _CONFIG_CLIENT_STATIC_NO

struct configClient *configClientNew( )
{
  struct configClient *client = (struct configClient*)malloc(sizeof(struct configClient));
  return client;
}

#ifdef _CONFIG_CLIENT_STATIC
  int main( )
  {
    return 0;
  }
#endif
