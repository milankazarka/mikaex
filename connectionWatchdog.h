#ifndef _CONNECTION_WATCHDOG_H_
#define _CONNECTION_WATCHDOG_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct connectionWatchdogDelegate
{
  void		*(*onConnectionStatus)( void *watchdogptr, int alive );
} connectionWatchdogDelegate;

typedef struct connectionWatchdog
{
  unsigned char 			server[256];
  struct connectionWatchdogDelegate 	*delegate;
} connectionWatchdog;

struct connectionWatchdog *connectionWatchdogInit( struct connectionWatchdog *watchdog );
int connectionWatchdogConnected( struct connectionWatchdog *watchdog ); // 0=yes/0<error
int connectionWatchdogRun( struct connectionWatchdog *watchdog );

#endif
