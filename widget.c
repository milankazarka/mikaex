#include "system.h"
#include "global.h"
#include "interface.h"
#include "xml.h"
#include "wm.h"

#define _STATIC_NO

#define _API_0
#define _API_MIKA

typedef unsigned int INT;

typedef struct _UIPoint
{
  INT		x;
  INT		y;
} UIPoint;

typedef UIPoint UIOrigin;

typedef struct _UISize
{
  INT		w;
  INT		h;
} UISize;

typedef struct _UIFrame
{
  UIOrigin		origin;
  UISize		size;
} UIFrame;

class UIApp;
class UIWidget;
class UIQueue;

void *_mika_event_queue( void*data )
{
  struct interface_event_t ievent;
      
  for(;;)
  {
    bigInterface->nextGuiEvent(bigInterface,&ievent);
    /**
     *
     *   PUSH the os event or the app generated event
     *
     */
    gui_WidgetPushEvent(&ievent);
  }
}

#define _WQ_ASYNC	0
#define _WQ_SYNC	1
#define _WQ_TYPE	_WQ_ASYNC

class UIApp
{
  private:
    trop_Atom *at;
    
  public:
    
    UIApp( )
    {
      at = NULL;
    }
    ~UIApp( ) { }
    
    void eventQueue( )
    {
      if (_WQ_TYPE==_WQ_ASYNC)
      {
        gui_GetMain()->queue = (struct trop_Atom*)malloc(sizeof(struct trop_Atom));
        gui_GetMain()->queue->call = _mika_event_queue;
        gui_GetMain()->queue->data = gui_GetMain();
        trop_AtomInit(gui_GetMain()->queue);
      }
      else if (_WQ_TYPE==_WQ_SYNC)
      {
        _mika_event_queue(gui_GetMain());
      }
    }
    
    void run( void *(*call)(void*data) )
    {
       at = (struct trop_Atom*)malloc(sizeof(struct trop_Atom));
       at->call = call;
       at->data = this;
       trop_AtomInit(at);
    }
    
    void lock( )
    { for(;;) usleep(1000); }
};

class UIWidget
{
  public:
    #ifdef _API_0
      gui_Widget        *guiWidget;
    #endif
    
    #ifdef _API_MIKA
      BOOL              bvisible;
    #endif
    
    UIWidget( UIFrame *frame, UIWidget *parent )
    {
      guiWidget = NULL;
      #ifdef _API_0
        
        struct gui_Widget *guiParent = NULL;
        
        if (parent)
          guiParent = parent->guiWidget;
      
        guiWidget = gui_NewWidget();
        
        guiWidget->attr.x = frame->origin.x;
        guiWidget->attr.y = frame->origin.y;
        guiWidget->attr.w = frame->size.w;
        guiWidget->attr.h = frame->size.h;
        
        gui_WidgetInit(guiWidget,guiParent);
        gui_WidgetMap(guiWidget);
      #endif
    }
    ~UIWidget( )
    {
      #ifdef _API_0
        if (guiWidget)
        {
          free(guiWidget);
        }
      #endif
    }
};

class UIQueue
{
  public:
    UIWidget *widget;
    
    UIQueue( UIWidget *_widget )
    {
      if (!_widget)
      { }
      widget = _widget;
    }
    
    void run( )
    {
      #ifdef _API_0
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf(":UIQueue.run\n");
        #endif
        gui_WidgetQueue( widget->guiWidget );
      #endif
    }
};

#ifdef _STATIC
  
  UIWidget *widget  = NULL;
  UIQueue *queue    = NULL;
  
  void *appThread( void *data )
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("appThread\n");
    #endif
    for(;;)
    {
      usleep(1000);
    }
  }
  
  int main( )
  {
    
    UIApp app;
    
    UIFrame frame;
    frame.size.w = 480;
    frame.size.h = 320;
    frame.origin.x = 0;
    frame.origin.y = 0;
    
    trop_Init();
    gui_Init();
    tr_WmInit();
    rotation = ROT_NONE;
    
    widget = new UIWidget(&frame,NULL);
    if (1)
      app.eventQueue();
    if (0)
    {
      queue = new UIQueue(widget);
      queue->run();
    }
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":widget should not reach\n");
    #endif
    
    app.run(appThread);
    app.lock();
    
    return 0;
  }
#endif
