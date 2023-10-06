#ifndef _ROOT_CONTROLLER_WATCHDOG_H_
#define _ROOT_CONTROLLER_WATCHDOG_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct rootControllerWatchdog_t {
  void *sysWatchdog;
} rootControllerWatchdog_t;

void *rootControllerWatchdogGlobalInit( void *data );

extern struct rootControllerWatchdog_t *rootWatchdog;

#endif
