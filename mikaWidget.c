#include "mikaWidget.h"
#include "mikaWindow.h"
#include "mikaSkinEx.h"

/** place the widget onto the respective canvas
 *
 */
int mikaWidget::placeWidget( )
{
  #ifdef _DEBUG
    printf("mikaWidget::placeWidget\n");
  #endif
  if (!myParent)
  {
    mikaWindow *mw = (mikaWindow*)myData.mwindow;
    mw->addWidget(this);
  }
  else
  {
    #ifdef _DEBUG
      printf("mikaWidget::placeWidgetToParent\n");
    #endif
    #ifdef _TR_SYSTEM
      tr_PutObject((struct tr_Object*)myParent->low,(struct tr_Object*)low);
    #endif
  }
  return 0;
}

mikaWidget::mikaWidget( struct mikaWidgetData *data )
  : mikaEventReceiver()
{
  if (!data)
  {
    #ifdef _DEBUG
      printf("mikaWidget::mikaWidget error, invalid data\n");
    #endif
  }
  myData.mwindow = NULL;
  myData.window = NULL;
  myData.canvas	= NULL;
  myData.widget	= NULL;
  low 		= NULL;
  next 		= NULL;
  children 	= NULL;
  myParent	= NULL;
  if (data)
  {
    myData = *data;
    #ifdef _TR_SYSTEM
      low = data->widget;
      tr_SetAction((struct tr_Object*)low,lowMikaEventTransmit,TR_ACTION_BUTTON);
    #endif
    setup((unsigned char*)"mikaButton");
    
    placeWidget();
  }
}

mikaWidget::mikaWidget( struct mikaWidgetData *data, mikaWidget *parent )
  : mikaEventReceiver()
{
  if (!data || !parent)
  {
    #ifdef _DEBUG
      printf("mikaWidget::mikaWidget error, invalid data\n");
    #endif
  }
  myData.mwindow = NULL;
  myData.window = NULL;
  myData.canvas	= NULL;
  myData.widget	= NULL;
  low 		= NULL;
  next 		= NULL;
  children 	= NULL;
  myParent	= parent;
  if (data)
  {
    myData = *data;
    #ifdef _TR_SYSTEM
      low = data->widget;
      tr_SetAction((struct tr_Object*)low,lowMikaEventTransmit,TR_ACTION_BUTTON);
    #endif
    setup((unsigned char*)"mikaButton");
    
    placeWidget();
  }
}

void *mikaWidget::getLow( )
{
  return low;
}

mikaWidget::~mikaWidget( )
{
  #ifdef _DEBUG
    printf("mikaWidget::~mikaWidget\n");
  #endif
  mikaWidget *current = children;
  while(current)
  {
    #ifdef _DEBUG
      printf("mikaWidget::~mikaWidget delete child(%p)\n",current);
    #endif
    delete current;
    current = current->next;
  }
}

/** setup the widget from the skin configuration according to name
 *
 */
int mikaWidget::setup( unsigned char *name )
{
  #ifdef _DEBUG
    printf("mikaWidget::setup\n");
  #endif
  
  if (name)
  {
    #ifdef _DEBUG
      printf("mikaWidget::setup name(%s)\n",name);
    #endif
    #ifdef _TR_SYSTEM
      mikaSkinEx *skin = getSkin();
      skin->setupWidget((struct tr_Object*)low,name);
      tr_SetRound((struct tr_Object*)low,2);
    #endif
  }
  return 0;
}

void mikaWidget::onEvent( struct mikaEvent *event )
{
  #ifdef _DEBUG
    printf("mikaWidget::onEvent\n");
  #endif
}

void mikaWidget::onButtonDown( struct mikaEvent *event )
{
  #ifdef _DEBUG
    printf("mikaWidget::onButtonDown\n");
  #endif
}

void mikaWidget::onButtonUp( struct mikaEvent *event )
{
  #ifdef _DEBUG
    printf("mikaWidget::onButtonUp\n");
  #endif
}

/** place another widget into this widget object
 */
int mikaWidget::addWidget( mikaWidget *widget )
{
  #ifdef _DEBUG
    printf("mikaWidget::addWidget\n");
  #endif
  if (!widget)
  {
    #ifdef _DEBUG
      printf("mikaWidget::addWidget error, invalid widget\n");
    #endif
    return 1;
  }
  
  widget->next = children;
  children = widget;
  
  #ifdef _TR_SYSTEM
    if (myData.widget && widget->myData.widget)
      tr_PutObject(myData.widget,widget->myData.widget);
  #endif
  
  return 0;
}

mikaWidgetTest::mikaWidgetTest( mikaWidgetData *data )
  : mikaWidget(data)
{
}

mikaWidgetTest::~mikaWidgetTest( )
{
}

void mikaWidgetTest::onWidgetDelegate( struct delegateData *data )
{
  #ifdef _DEBUG
    printf("mikaWidgetTest::onWidgetDelegate\n");
  #endif
  if (!data)
  {
    #ifdef _DEBUG
      printf("mikaWidgetTest::onWidgetDelegate error, invalid data\n");
    #endif
    return;
  }
  #ifdef _DEBUG
    printf("mikaWidgetTest::onWidgetDelegate type(%d) parent(%p) data(%p) mikaObject(%p)\n",
      data->type,data->parent,data->data,data->object);
  #endif
}

#ifdef _TR_SYSTEM
  struct mikaWidgetData *mikaWidgetDataNew( 
    struct tr_Window *window, struct tr_Object *canvas, struct tr_Object *widget )
  {
    #ifdef _DEBUG
      printf("mikaWidgetDataNew\n");
    #endif
    mikaWidgetData *data = (mikaWidgetData*)malloc(sizeof(struct mikaWidgetData));
    data->attr.x = 0;
    data->attr.y = 0;
    data->attr.w = 0;
    data->attr.h = 0;
    
    data->window = window;
    data->canvas = canvas;
    data->widget = widget;
  }
#endif

/** automatically create the data holding the low level widget
 *
 *  this is used for mikaWidget
 */
struct mikaWidgetData *mikaWidgetDataNewEx( mikaObject *window, struct tr_Attr *attr )
{
  #ifdef _DEBUG
    printf("mikaWidgetDataNewEx\n");
  #endif
  struct mikaWidgetData *data = (struct mikaWidgetData*)malloc(sizeof(struct mikaWidgetData));
  mikaWindow *mw = (mikaWindow*)window;
  data->mwindow = window;
  #ifdef _TR_SYSTEM
    tr_WindowToTop(mw->window);
    data->widget = tr_ButtonInitEx2(NULL,attr,mw->canvas);
    data->canvas = mw->canvas;
    data->window = mw->window;
    
    data->attr.x = attr->x;
    data->attr.y = attr->y;
    data->attr.w = attr->w;
    data->attr.h = attr->h;
    
    // setup widget
    mikaSkinEx *skin = getSkin();
    skin->setupWidget(data->widget,(unsigned char*)"mikaButton");    
  #endif
  return data;
}
