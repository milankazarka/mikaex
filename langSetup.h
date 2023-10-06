#ifndef _LANG_SETUP_H_
#define _LANG_SETUP_H_

#include "setupParser.h"

typedef struct langItem
{
  unsigned char key[256];
  unsigned char value[256];
  struct langItem *next;
} langItem;

typedef struct langSetup
{
  struct langItem *items;
} langSetup;

struct langSetup *langSetupInit( struct langSetup *ls, struct setupParser *parser );
int langSetupProcessItem( struct langSetup *ls, TiXmlElement *element );
unsigned char *langGet( struct langSetup *ls, unsigned char *key );

#endif
