#ifndef _MIKA_EVENT_H_
#define _MIKA_EVENT_H_

typedef struct mikaEvent
{
  int x;
  int y;
  /** object that identifies this event
   */
  void *idObject;
  void *mobject; // mikaObject
} mikaEvent;

#endif
