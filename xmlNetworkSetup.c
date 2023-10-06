#include "xmlNetworkSetup.h"
#include "config.h"

struct xmlNetworkSetup *xmlNetworkSetupInit( struct xmlNetworkSetup *xmlnet )
{
  if (!xmlnet)
    xmlnet = (struct xmlNetworkSetup*)malloc(sizeof(struct xmlNetworkSetup));
  return xmlnet;
}

int xmlNetworkSetupConfigure( struct xmlNetworkSetup *xmlnet, struct setupParser *parser, struct networkSetup *setup )
{
  #ifdef _DEBUG
    printf("xmlNetworkSetupConfigure\n");
  #endif
  if (!xmlnet || !parser || !setup)
  {
    #ifdef _DEBUG
      printf("xmlNetworkSetupConfigure ERROR invalid arguments\n");
    #endif
    return 1;
  }
  return 0;
}

