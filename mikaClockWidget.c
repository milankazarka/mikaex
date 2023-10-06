#include "mikaClockWidget.h"

mikaClockWidget::mikaClockWidget( struct mikaWidgetData *data )
  : mikaWidget(data)
{
  runThread();
  tr_SetFontSize((struct tr_Object*)getLow(),20);
}

mikaClockWidget::~mikaClockWidget( )
{
}

void *mikaClockWidgetThread( void *data )
{
  if (!data)
  {
    #ifdef _DEBUG
      printf("mikaClockWidgetThread error, invalid data\n");
    #endif
    return NULL;
  }
  mikaClockWidget *cw = (mikaClockWidget*)data;
  cw->clockThread();
  return NULL;
}

int mikaClockWidget::runThread( )
{
  pthread_t t;
  pthread_create(&t,NULL,mikaClockWidgetThread,this);
  return 0;
}

int mikaClockWidget::clockThread( )
{
  #ifdef _DEBUG
    printf("mikaClockWidget::clockThread\n");
  #endif
  while(1)
  {
    update();
    usleep(_CW_CLOCKTIK);
  }
  return 0;
}

int mikaClockWidget::update( )
{
  static unsigned char timestr[32];
  static int nh = 0;
  static int nm = 0;
  static int ns = 0;
  
  ts = time(NULL);
  tms = *localtime(&ts);
  if (
    tms.tm_hour != nh ||
    tms.tm_min != nm ||
    tms.tm_sec != ns
    )
  {
    sprintf((char*)timestr,"%d:%d:%d",tms.tm_hour,tms.tm_min,tms.tm_sec);
    #ifdef _DEBUG
      printf("mikaClockWidget::update time(%s)\n",(char*)timestr);
    #endif
    #ifdef _TR_SYSTEM
      tr_ChangeTextExPos((struct tr_Object*)getLow(),(char*)timestr,20,30);
      tr_WindowToTop(myData.window);
      tr_WindowExpose(myData.window);
    #endif
    nh = tms.tm_hour;
    nm = tms.tm_min;
    ns = tms.tm_sec;
  }
  return 0;
}
