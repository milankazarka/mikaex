/** Milan Kazarka 2011
 */
#ifndef _MIKA_MP_SCHED_H_
#define _MIKA_MP_SCHED_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define _MP_SCHED_CLOCKTIK 20000
/** merge events by not adding an event if there already
 *  is one of the same ID in the queue
 */
#define _MP_SCHED_STUPID_MERGE

/** aditional data to be used by an event
 */
typedef struct mikaMpEventData_t {
  void *data;
  int id;
  struct mikaMpEventData_t *next;
} mikaMpEventData_t;

/** a simple event
 */
typedef struct mikaMpEvent_t {
  int Id;
  struct mikaMpEventData_t *custom;
  struct mikaMpEvent_t *next;
} mikaMpEvent_t;

struct mikaMpEvent_t *mikaMpEventNew( );
void mikaMpEventDelete( struct mikaMpEvent_t *event );
int mikaMpEventAddCustom( struct mikaMpEvent_t *event, void *data, int id );

typedef struct mikaMpSchedRuleDelegate_t {
  void *(*onEvent)( void *sched, struct mikaMpEvent_t *event );
} mikaMpSchedRuleDelegate_t;

/** rule to be applied for an event
 */
class mikaMpSchedRule {
  public:
    struct mikaMpSchedRuleDelegate_t myDelegate;
    mikaMpSchedRule *next;
    int eventId;
    
    // last triggered rule
    long lastTrigger;
    // generis setup
    int timeout;
    int merge; 	// merge events of this type for timeout, good for setting
                // up fps
      
    mikaMpSchedRule( struct mikaMpSchedRuleDelegate_t *delegate );
    ~mikaMpSchedRule( );
};

/** scheduller of events
 */
class mikaMpSched {
  public:
    /** todo - check to use less mutexes in
     *         system
     */
    pthread_mutex_t myMutex;
    pthread_t eventThread;
  
    mikaMpSched( );
    ~mikaMpSched( );
    mikaMpSchedRule *rules;
    struct mikaMpEvent_t *queue;
    
    int addRule( mikaMpSchedRule *rule );
    int onEvent( struct mikaMpEvent_t *event );
    void eventLoopThread( );
    void eventLoop( );
    struct mikaMpEvent_t *nextEvent( );
    int removeEvent( struct mikaMpEvent_t *event );
};

extern mikaMpSched *globalSched;

int mikaMpSchedGlobalInit( );
void *mikaMpSchedEventLoop( void *data );

#endif
