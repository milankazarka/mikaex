#include "mikaWatchdog.h"
#include "config.h"

void *onClocktik( void *data )
{
  mikaWatchdog *dWatchdog = (mikaWatchdog*)data;
  #ifdef _DEBUG
    printf("onClocktik\n");
  #endif
}

void *onRestart( void *data )
{
  mikaWatchdog *dWatchdog = (mikaWatchdog*)data;
  #ifdef _DEBUG
    printf("onRestart\n");
  #endif
}

int main( )
{
  struct mikaWatchdogDelegate delegate;
  mikaWatchdogDelegateClass *cDelegate = new mikaWatchdogDelegateClass();
  delegate.clocktik = 2;
  delegate.onRestart = onRestart;
  delegate.onClocktik = onClocktik;
  mikaWatchdog *watchdog = new mikaWatchdog(&delegate);
  watchdog->run();
  while(1)
  { 
    watchdog->handshake();
    usleep(250000); 
  }
  return 0;
}
