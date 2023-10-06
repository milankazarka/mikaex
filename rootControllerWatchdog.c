#include "rootControllerWatchdog.h"
#include "mikaWatchdog.h"
#include "config.h"

static mikaWatchdog *watchdog = NULL;
struct rootControllerWatchdog_t *rootWatchdog = NULL;

void *rootControllerWatchdogGlobalInit( void *data )
{
  #ifdef _DEBUG
    printf("rootControllerWatchdogGlobalInit\n");
  #endif
  return NULL;
}
