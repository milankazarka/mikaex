#include "mikaApps.h"
#include "config.h"
#include <signal.h>

mikaApps *apps = NULL;

void _terminationHandler( int sig )
{
  #ifdef _DEBUG
    printf("_terminationHandler");
  #endif
  delete apps;
  exit(0);
}

int main( )
{
  apps = new mikaApps((unsigned char*)"apps");
  
  signal(SIGHUP,_terminationHandler);
  signal(SIGINT,_terminationHandler);
  signal(SIGTERM,_terminationHandler);
  
  apps->runAll();
  usleep(100000);
  apps->runProcessing();
  while(1)
  { sleep(1); }
  return 0;
}
