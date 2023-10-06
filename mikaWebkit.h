#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct mikaWebkit *mikaWebkitInit( struct mikaWebkit *dw )
{
  if (!dw)
    dw = (struct mikaWebkit*)malloc(sizeof(struct mikaWebkit));
  return dw;
}
