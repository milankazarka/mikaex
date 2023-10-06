#include "connectionWatchdog.h"
#include "config.h"

#define _WATCHDOG_TIK 30

/** initialize the connectionWatchdog structure
 */
struct connectionWatchdog *connectionWatchdogInit( struct connectionWatchdog *watchdog )
{
  #ifdef _DEBUG
    printf("connectionWatchdogInit\n");
  #endif
  if (!watchdog)
  {
    watchdog = (struct connectionWatchdog*)malloc(sizeof(struct connectionWatchdog));
  }  
  
  watchdog->server[0] = 0x00;
  watchdog->delegate = NULL;
  
  return watchdog;
}

/** find out if we have a live connection
 */
#define _CHECK_CONNECTION_NSLOOKUP
int connectionWatchdogConnected( struct connectionWatchdog *watchdog )
{
  #ifdef _DEBUG
    printf("connectionWatchdogConnected\n");
  #endif
  if (!watchdog)
  {
    #ifdef _DEBUG
      printf("connectionWatchdogConnected ERROR invalid arguments\n");
    #endif
    return 1;
  }

  if (watchdog->server[0]==0x00)
  {
    #ifdef _DEBUG
      printf("connectionWatchdogConnected ERROR no server defined\n");
    #endif
    return 1;
  }
  
  int connection = 1;
  
  unsigned char cmd[512];
  #ifdef _CHECK_CONNECTION_NSLOOKUP
    sprintf((char*)cmd,"nslookup %s",(char*)watchdog->server);
  #endif
  #ifdef _DEBUG
    printf("connectionWatchdogConnected command(%s)\n",(char*)cmd);
  #endif
  
  FILE* pipe = popen(cmd, "r");
  if (!pipe)
  {
    #ifdef _DEBUG
      printf("connectionWatchdogConnected ERROR could not create pipe server(%s)\n",
        (char*)watchdog->server);
    #endif
    return 1;
  }
  char buffer[128];
  
  while(!feof(pipe)) 
  {
    if(fgets(buffer, 128, pipe) != NULL)
    {
      #ifdef _DEBUG
        printf("connectionWatchdogConnected response line(%s)\n",(char*)buffer);
      #endif
      #ifdef _CHECK_CONNECTION_NSLOOKUP
        if (strstr((char*)buffer,(char*)"Name:"))
      #endif
      {
        #ifdef _DEBUG
          printf("connectionWatchdogConnected connection OK\n");
        #endif
        connection = 0;
      }
    }
  }
  pclose(pipe);
  
  return connection;
}

void *_connection_watchdog_thread(void*watchdogptr)
{
  struct connectionWatchdog *watchdog = (struct connectionWatchdog*)watchdogptr;
  while(1)
  {
    int alive = connectionWatchdogConnected(watchdog);
    if (watchdog->delegate)
    {
      if (watchdog->delegate->onConnectionStatus)
      {
        watchdog->delegate->onConnectionStatus(watchdog,alive);
      }
    }
    sleep(_WATCHDOG_TIK);
  }
}

int connectionWatchdogRun( struct connectionWatchdog *watchdog )
{
  pthread_t t;
  
  if (!watchdog)
  {
    #ifdef _DEBUG
      printf("connectionWatchdogRun ERROR invalid arguments\n");
    #endif
    return 1;
  }
  
  pthread_create(&t,NULL,_connection_watchdog_thread,watchdog);
  
  return 0;
}
