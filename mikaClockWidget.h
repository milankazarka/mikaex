#ifndef _MIKA_CLOCK_WIDGET_H_
#define _MIKA_CLOCK_WIDGET_H_

#include "mikaWidget.h"
#include "config.h"

#define _CW_CLOCKTIK 1*100000	// sampling rate

class mikaClockWidget : public mikaWidget {
  public:
    time_t ts;
    struct tm tms;
  
    mikaClockWidget( struct mikaWidgetData *data );
    ~mikaClockWidget( );
    
    int runThread( );
    int clockThread( );
    
    int update( );
};

#endif
