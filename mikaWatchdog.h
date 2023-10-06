#ifndef _MIKA_WATCHDOG_H_
#define _MIKA_WATCHDOG_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

typedef struct mikaWatchdogDelegate {
  int clocktik;
  void *(*onRestart)(void*data);
  void *(*onClocktik)(void*data);
} mikaWatchdogDelegate;

class mikaWatchdogDelegateClass {
  public:
    struct mikaWatchdogDelegate delegate;
    mikaWatchdogDelegateClass( );
    ~mikaWatchdogDelegateClass( );
};

class mikaWatchdog {
  public:
    struct mikaWatchdogDelegate *myDelegate;
    mikaWatchdogDelegateClass *myDelegateClass;
    pthread_t thread;
    pthread_mutex_t mutex;
    time_t lastHandshake;
    
    mikaWatchdog( struct mikaWatchdogDelegate *delegate );
    mikaWatchdog( mikaWatchdogDelegateClass *delegateClass );
    ~mikaWatchdog( );
    /** run the watchdog thread 
     */
    int run( );
    int checkState( );
    /** tell the watchdog, that we are alive
     */
    int handshake( );
};

#endif
