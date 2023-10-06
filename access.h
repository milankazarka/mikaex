#include <stdio.h>
#include <stdlib.h>
#include "interface.h"

typedef struct mikaAccess
{
  unsigned char filename[512];
} mikaAccess;

struct mikaAccess *mikaAaccessInit( struct mikaAccess *access );
