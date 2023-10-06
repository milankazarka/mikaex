#ifndef _POS_SETUP_H_
#define _POS_SETUP_H_

#include "setupParser.h"
#include "networkSetup.h"

typedef struct posSetup
{
  unsigned char server[64];
  int		port;
} posSetup;

struct posSetup *posSetupInit( struct posSetup *ps );
int posSetupConfigure( struct posSetup *ps, struct setupParser *parser, struct networkSetup *setup );

#endif
