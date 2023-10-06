#ifndef _XML_NETWORK_SETUP_H_
#define _XML_NETWORK_SETUP_H_

#include "setupParser.h"
#include "networkSetup.h"

typedef struct xmlNetworkSetup
{
} xmlNetworkSetup;

struct xmlNetworkSetup *xmlNetworkSetupInit( struct xmlNetworkSetup *xmlnet );
int xmlNetworkSetupConfigure( struct xmlNetworkSetup *xmlnet, struct setupParser *parser, struct networkSetup *setup );

#endif
