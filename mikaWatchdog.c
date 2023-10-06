#include "mikaWatchdog.h"
#include "config.h"

mikaWatchdogDelegateClass::mikaWatchdogDelegateClass( )
{
  delegate.onRestart = NULL;
  delegate.onClocktik = NULL;
}

mikaWatchdogDelegateClass::~mikaWatchdogDelegateClass( )
{
}

/** the main thread that checks the application's sanity
 */
void *mikaWatchdogThread( void *watchdogObject )
{
  if (!watchdogObject)
    return NULL;
  mikaWatchdog *dWatchdog = (mikaWatchdog*)watchdogObject;
  dWatchdog->handshake(); // give it extra time
  
  void *(*onClocktik)(void*data) = NULL;
  void *(*onRestart)(void*data) = NULL;
  int clocktik = 0;
  
  if (dWatchdog->myDelegate)
  {
    onClocktik = dWatchdog->myDelegate->onClocktik;
    onRestart = dWatchdog->myDelegate->onRestart;
    clocktik = dWatchdog->myDelegate->clocktik;
  }  
  else if (dWatchdog->myDelegateClass)
  {
    onClocktik = dWatchdog->myDelegateClass->delegate.onClocktik;
    onRestart = dWatchdog->myDelegateClass->delegate.onRestart;
    clocktik = dWatchdog->myDelegateClass->delegate.clocktik;
  }
  else
  {
    #ifdef _DEBUG
      printf("mikaWatchdogThread no delegate, exiting thread\n");
    #endif
    return NULL;
  }
  
  while(1)
  {
    time_t currentTime = time(NULL);
    #ifdef _DEBUG
      printf("mikaWatchdogThread clocktik %d last %d\n",
        currentTime,dWatchdog->lastHandshake);
    #endif
    if (dWatchdog->lastHandshake+clocktik<
      currentTime)
    {
      #ifdef _DEBUG
        printf("mikaWatchdogThread trigger restart\n");
      #endif
      if (onRestart)
      {
        /** trigger the restart
         */
        onRestart(dWatchdog);
      }
    }
    if (onClocktik)
    {
      /** trigger the clocktik routine
       */
      onClocktik(dWatchdog);
    }
    sleep(clocktik);
  }
}

mikaWatchdog::mikaWatchdog( struct mikaWatchdogDelegate *delegate )
{
  if (!delegate)
  {
    #ifdef _DEBUG
      printf("mikaWatchdog::mikaWatchdog error no delegate\n");
    #endif
  }
  myDelegate = delegate;
  myDelegateClass = NULL;
  pthread_mutex_init(&mutex,NULL);
}

mikaWatchdog::mikaWatchdog( mikaWatchdogDelegateClass *delegateClass )
{
  if (!delegateClass)
  {
    #ifdef _DEBUG
      printf("mikaWatchdog::mikaWatchdog error no delegate\n");
    #endif
  }
  myDelegate = NULL;
  myDelegateClass = delegateClass;
  pthread_mutex_init(&mutex,NULL);
}

mikaWatchdog::~mikaWatchdog( )
{
  #ifdef _DEBUG
    printf("mikaWatchdog::~mikaWatchdog\n");
  #endif
  pthread_mutex_destroy(&mutex);
}

int mikaWatchdog::run( )
{
  pthread_create(&thread,NULL,mikaWatchdogThread,this);
  return 0;
}

int mikaWatchdog::checkState( )
{
  return 0;
}

/** tell the watchdog, that we are alive
 */
int mikaWatchdog::handshake( )
{
  #ifdef _DEBUG
    printf("mikaWatchdog::handshake\n");
  #endif
  pthread_mutex_lock(&mutex);
  lastHandshake = time(NULL);
  pthread_mutex_unlock(&mutex);
  return 0;
}
