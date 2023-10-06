#include "menuSetup.h"
#include "config.h"

#define _MS_SERVER 	"ftp://localhost"
#define _MS_USERNAME 	"anonymous"
#define _MS_PASSWORD 	"secret"
#define _MS_PORT	21

struct menuSetup *menuSetupInit( struct menuSetup *ms )
{
  if (!ms)
    ms = (struct menuSetup*)malloc(sizeof(struct menuSetup));
  
  ms->setup = networkSetupInit(NULL);
  
  return ms;
}
