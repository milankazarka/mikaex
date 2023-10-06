#ifndef _MIKA_CANVAS_H_
#define _MIKA_CANVAS_H_

#include "mikaObject.h"
#include "mikaWidget.h"
#include "config.h"

class mikaCanvas : public mikaObject {
  public:
    #ifdef _TR_SYSTEM
      mikaWidgetData *myData;
    #endif
    
    mikaCanvas( struct mikaWidgetData *data );
    ~mikaCanvas( );
    
    int addWidget( mikaWidget *widget );
    int exchangeWidget( mikaWidget *original, mikaWidget *widget );
};

#endif
