#include "mikaRotateWidget.h"

mikaRotateWidget::mikaRotateWidget( struct mikaWidgetData *data )
  : mikaWidget(data)
{
  #ifdef _DEBUG
    printf("mikaRotateWidget::mikaRotateWidget\n");
  #endif
  predefined = _ROTATE_NONE;
  flip = 0;
}

mikaRotateWidget::mikaRotateWidget( struct mikaWidgetData *data, int angle )
  : mikaWidget(data)
{
  #ifdef _DEBUG
    printf("mikaRotateWidget::mikaRotateWidget with angle\n");
  #endif
  predefined = angle;
  current = predefined;
  flip = 0;
}

mikaRotateWidget::~mikaRotateWidget( )
{
}

void mikaRotateWidget::onEvent( struct mikaEvent *event )
{
  #ifdef _DEBUG
    printf("mikaRotateWidget::onEvent\n");
  #endif
  int forceangle = activityGetForceangle(&bigInterface->activity,1);
  if (forceangle!=-1)
  {
    if (predefined!=_ROTATE_NONE)
    {
      rotateDefined();
    }
    else
    {
      if (forceangle==270)
        forceangle=0;
      else
        forceangle+=90;
      activitySetForceangle(&bigInterface->activity,forceangle,1);
    }
    #ifdef _TR_SYSTEM
      tr_WindowToTop(myData.window);
      tr_WindowExpose(myData.window);
    #endif
  }
}

void mikaRotateWidget::onButtonDown( struct mikaEvent *event )
{
}

void mikaRotateWidget::onButtonUp( struct mikaEvent *event )
{
}

int mikaRotateWidget::update( )
{
  #ifdef _TR_SYSTEM
    tr_WindowToTop(myData.window);
    tr_WindowExpose(myData.window);
  #endif
  return 0;
}

int mikaRotateWidget::rotateLeft( ) // 90
{
  activitySetForceangle(&bigInterface->activity,90,1);
  return 0;
}

int mikaRotateWidget::rotateRight( ) // 270
{
  activitySetForceangle(&bigInterface->activity,270,1);
  return 0;
}

int mikaRotateWidget::rotateUp( ) // 180
{
  activitySetForceangle(&bigInterface->activity,180,1);
  return 0;
}

int mikaRotateWidget::rotateDown( ) // 0
{
  activitySetForceangle(&bigInterface->activity,0,1);
  return 0;
}

int mikaRotateWidget::rotateDefined( )
{
  #ifdef _DEBUG
    printf("mikaRotateWidget::rotateDefined\n");
  #endif
  if (flip==0)
    activitySetForceangle(&bigInterface->activity,predefined,1);
  else
  {
    if (current+180==360)
      current = 0;
    else if (current+180==450)
      current = 90; 
    else
      current+=180;
      
    activitySetForceangle(&bigInterface->activity,current,1);
  }
  return 0;
}
