/** Milan Kazarka 2011
 */
#include "mikaMpSched.h"
#include "config.h"

mikaMpSched *globalSched = NULL;

/** allocate a new event
 */
struct mikaMpEvent_t *mikaMpEventNew( )
{
  struct mikaMpEvent_t *event = (struct mikaMpEvent_t*)malloc(sizeof(struct mikaMpEvent_t));
  event->custom = NULL;
  event->next = NULL;
  return event;
}

/** add a custom object
 */
int mikaMpEventAddCustom( struct mikaMpEvent_t *event, void *data, int id )
{
  if (!event || !data)
    return 1;
  
  struct mikaMpEventData_t *custom = (struct mikaMpEventData_t*)malloc(sizeof(struct mikaMpEventData_t));
  custom->data = data;
  custom->id = id;
  custom->next = event->custom;
  event->custom = custom;
  
  return 0;
}

/** release an event
 */
void mikaMpEventDelete( struct mikaMpEvent_t *event )
{
  if (!event)
    return;
  struct mikaMpEventData_t *current = event->custom;
  struct mikaMpEventData_t *next = NULL;
  while(current)
  {
    next = current->next;
    free(current);
    current = next;
  }
  free(event);
}

mikaMpSchedRule::mikaMpSchedRule( struct mikaMpSchedRuleDelegate_t *delegate )
{
  if (delegate)
    myDelegate = *delegate;
  else
    myDelegate.onEvent = NULL;
  next = NULL;
  lastTrigger = 0;	// last time the rule gotten applied
  timeout = 0; 		// get event executed immediately
  merge = 0;		// don't merge events
}

mikaMpSchedRule::~mikaMpSchedRule( )
{
}

/** todo - enable setting external mutex for use in
 *         syncing
 */
mikaMpSched::mikaMpSched( )
{
  rules = NULL;
  queue = NULL;
  pthread_mutex_init(&myMutex,NULL);
}

mikaMpSched::~mikaMpSched( )
{
  pthread_mutex_destroy(&myMutex);
}

int mikaMpSched::addRule( mikaMpSchedRule *rule )
{
  if (!rule)
  {
    #ifdef _DEBUG
      printf("mikaMpSched::addRule error invalid argument\n");
    #endif
    return 1;
  }
  
  rule->next = rules;
  rules = rule;
  
  return 0;
}

/** add & process an event, apply rules on it
 */
int mikaMpSched::onEvent( struct mikaMpEvent_t *event )
{
  if (!event)
  {
    #ifdef _DEBUG
      printf("mikaMpSched::onEvent error invalid argument\n");
    #endif
    return 1;
  }
  
  static struct mikaMpSchedRule *current = NULL;
  static struct mikaMpEvent_t *currentEvent = NULL;
  pthread_mutex_lock(&myMutex);
  
  #ifdef _MP_SCHED_STUPID_MERGE
    current = rules;
    while(current)
    {
      if (
        current->eventId == event->Id &&
        current->merge == 1
        )
      {
        currentEvent = queue;
        while(currentEvent)
        {
          if (currentEvent->Id==event->Id)
          {
            pthread_mutex_unlock(&myMutex);
            return 0;
          }
          currentEvent = currentEvent->next;
        }
      }
      current = current->next;
    }
  #endif
  
  event->next = queue;
  queue = event;
  pthread_mutex_unlock(&myMutex);
  
  return 0;
}

int mikaMpSched::removeEvent( struct mikaMpEvent_t *event )
{
  if (!event)
    return 1;
  struct mikaMpEvent_t *current = queue;
  struct mikaMpEvent_t *last = NULL;
  while(current)
  {
    if (current==event)
    {
      if (last)
        last->next = current->next;
      else
        queue = current->next;
      free(current);
      return 0;
    }
    last = current;
    current = current->next;
  }
  return 0;
}

/** retrieve the next event in queue
 */
struct mikaMpEvent_t *mikaMpSched::nextEvent( )
{
  static struct mikaMpEvent_t *current = NULL;
  current = queue;
  
  while(current)
  {
    if (current->next == NULL)
      return current;
    current = current->next;
  }
  
  return NULL;
}

/** run threaded processing of events
 */
void mikaMpSched::eventLoopThread( )
{
  #ifdef _DEBUG
    printf("mikaMpSched::thread\n");
  #endif
  struct mikaMpEvent_t *event = NULL;
  for(;;)
  {
    #ifdef _DEBUG
    //  printf("mikaMpSched::thread loop\n");
    #endif
    pthread_mutex_lock(&myMutex);
    
    mikaMpSchedRule *currentRule = rules;
  
    event = nextEvent();
    if (event)
    {
      while(currentRule)
      {
        if (
          currentRule->eventId==event->Id &&
          currentRule->myDelegate.onEvent
          )
        {
          currentRule->myDelegate.onEvent(this,event);
          break;
        }
        currentRule = currentRule->next;
      }
    }
    
    pthread_mutex_unlock(&myMutex);
    usleep(_MP_SCHED_CLOCKTIK);
  }
}

/** run the eventLoopThread method of the scheduller
 *  inside this thread
 */
void *mikaMpSchedEventLoop( void *data )
{
  if (!data)
  {
    #ifdef _DEBUG
      printf("mikaMpSchedEventLoop error invalid argument\n");
    #endif
    return NULL;
  }
  mikaMpSched *sched = (mikaMpSched*)data;
  sched->eventLoopThread();
}

/** start the thread managing the events
 */
void mikaMpSched::eventLoop( )
{
  pthread_create(&eventThread,NULL,mikaMpSchedEventLoop,this);
}

int mikaMpSchedGlobalInit( )
{
  if (!globalSched)
    globalSched = new mikaMpSched( );
  return 0;
}
