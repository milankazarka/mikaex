#include "config.h"
#include "access.h"
#include "interface.h"

struct mikaAaccess *mikaAccessInit( struct mikaAccess *access )
{
  if (!access)
    access = (struct mikaAccess*)malloc(sizeof(struct mikaAccess));
  
  access->filename[0] = 0x00;
  
  return access;
}

int mikaAccessRecord( struct mikaAccess *access )
{
  if (!access)
  {
    #ifdef _DEBUG
      printf("mikaAccessRecord error invalid arguments\n");
    #endif
    return 1;
  }
  FILE *fd = fopen((char*)access->filename,"w");
  unsigned char tmp[256];
  if (fd)
  {
    sprintf((char*)tmp,"SURFACESEM %d");
    fclose(fd);
  }
  return 0;
}
