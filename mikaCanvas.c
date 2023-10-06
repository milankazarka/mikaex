#include "mikaCanvas.h"

mikaCanvas::mikaCanvas( struct mikaWidgetData *data )
{
  #ifdef _DEBUG
    printf("mikaCanvas::mikaCanvas\n");
  #endif
  myData = data;
}

mikaCanvas::~mikaCanvas( )
{
}

int mikaCanvas::addWidget( mikaWidget *widget )
{
  #ifdef _DEBUG
    printf("mikaCanvas::addWidget\n");
  #endif
  if (!widget)
  {
    #ifdef _DEBUG
      printf("mikaCanvas::addWidget error, invalid widget\n");
    #endif
    return 1;
  }
  #ifdef _TR_SYSTEM
    tr_CanvasAddObjStatic(myData->canvas,widget->myData.widget);
  #endif
  return 0;
}

/** exchange a widget in canvas - simple way to switch part of the content
 */
int mikaCanvas::exchangeWidget( mikaWidget *original, mikaWidget *widget )
{
  #ifdef _DEBUG
    printf("mikaCanvas::exchangeWidget\n");
  #endif
  if (!original || !widget)
  {
    #ifdef _DEBUG
      printf("mikaCanvas::exchangeWidget error, invalid arguments\n");
    #endif
    return 1;
  }
  
  #ifdef _TR_SYSTEM
    tr_CanvasExchangeObjStatic(myData->canvas,original->myData.widget,widget->myData.widget);
  #endif
  
  return 0;
}
