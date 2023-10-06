#include "mikaMpSched.h"
#include "config.h"

enum _EVENTS
{
  _EVENT_EXPOSE
};

void *onExpose( void *sdata, struct mikaMpEvent_t *event )
{
  #ifdef _DEBUG
    printf("onExpose\n");
  #endif
  mikaMpSched *sched = (mikaMpSched*)sdata;
  sched->removeEvent(event);
}

int main( )
{
  mikaMpSchedGlobalInit();
  globalSched->eventLoop();
  
  struct mikaMpSchedRuleDelegate_t ruleDelegate;
  ruleDelegate.onEvent = onExpose;
  mikaMpSchedRule *ruleExpose = new mikaMpSchedRule(&ruleDelegate);
  ruleExpose->eventId = _EVENT_EXPOSE;
  globalSched->addRule(ruleExpose);
  
  struct mikaMpEvent_t *event = NULL;
  for(;;)
  {
    event = mikaMpEventNew();
    event->Id = _EVENT_EXPOSE;
    globalSched->onEvent(event);
    sleep(1);
  }
  return 0;
}
