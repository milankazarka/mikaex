#ifndef _MIKA_WIDGET_DELEGATE_H_
#define _MIKA_WIDGET_DELEGATE_H_

#include "mikaEventReceiver.h"
#include "mikaObject.h"

struct delegateData {
  int 		type;
  void 		*parent;
  void 		*data;
  /** preffered to identify widget:
   */
  mikaObject 	*object;
};

class mikaWidgetDelegate : mikaObject {
  public:
    mikaWidgetDelegate( );
    ~mikaWidgetDelegate( );
    
    virtual void onWidgetDelegate( struct delegateData *data );
};

#endif
