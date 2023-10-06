extern "C" {
  #include "interface.h"
  #include "common.h"
  #include "config.h"
  #include "os.h"
  #include "system.h"
  #include "systemip.h"
  #include "global.h"
  #include "xml.h"
  #include "wm.h"
  #include "dom.h"
  #include "activityObject.h"
}
#include "kitchen.h"
#include "posKitchen.h"

#define _KITCHEN_ORDERS 6

struct tr_Window *root		= NULL;
struct tr_Window *wmain		= NULL;
struct tr_Window *wdetail	= NULL;

struct tr_Object *cmain		= NULL;
struct tr_Object *cdetail	= NULL;
struct tr_Object *detailText	= NULL;
struct tr_Object *title		= NULL;
struct tr_Object *button	= NULL;
struct tr_Attr			orderAttr;
struct tr_Attr			mainAttr;
struct tr_Attr			tmpAttr;
struct tr_Attr			buttonAttr;
struct tr_Attr			detailTextAttr;

struct kitchenPos		*pos;

typedef struct detail
{
  struct kitchenPosOrder	*order;
} detail;

typedef struct order
{
  struct kitchenPosOrder 	*order; // refference order
  struct tr_Object 		*widget;
} order;

typedef struct orderList
{
  struct order olist[_KITCHEN_ORDERS];
} orderList;

struct detail detailData; // holds the current order data
struct orderList orders;

unsigned long currentTime( )
{
  static int usetimer = 0;
  static unsigned long long inittime;
  struct tms cputime;
  
  if (usetimer == 0)
  {
    inittime = (unsigned long long)times(&cputime);
    usetimer = 1;
  }
  
  return (unsigned long)((times(&cputime) - inittime)*1000UL/sysconf(_SC_CLK_TCK));
}

void *onOrder(void*data)
{
  #ifdef _DEBUG
    printf("onOrder\n");
  #endif
  struct gui_Event *event = (struct gui_Event*)data;
  struct gui_Widget *widget = event->widget;
  
  int n;
  for(n = 0; n < _KITCHEN_ORDERS; n++)
  {
    if (widget==orders.olist[n].widget->widget.sub)
    {
      #ifdef _DEBUG
        printf("onOrder found order\n");
      #endif
      detailData.order = orders.olist[n].order;
      tr_ChangeTextExPos(detailText,(char*)detailData.order->detail,20,detailTextAttr.h/2);
      tr_WindowToTop(wdetail);
      tr_WindowExpose(wdetail);
    }
  }
}

/** update the list with an order - the element of the GUI, which
 *  gets to be updated is determined in this method
 */
int updateListWithOrderSYNCED( struct kitchenPosOrder *order, int n )
{
  if (!order)
  {
    #ifdef _DEBUG
      printf("updateListWithOrderSYNCED error invalid arguments\n");
    #endif
    return 1;
  }
  
  if (n>=_KITCHEN_ORDERS)
  {
    #ifdef _DEBUG
      printf("updateListWithOrderSYNCED error item exceeds list\n");
    #endif
    return 1;
  }
  
  orders.olist[n].order = order;
  tr_ChangeTextExPos(orders.olist[n].widget,(char*)order->detail,100,tmpAttr.h/2);
  
  return 0;
}

/** update the orders list in intervals
 */
#define _UPDATE_LIST_TIK 1
void *updateListThread(void*data)
{
  unsigned long update = currentTime();
  struct kitchenPosOrder *order = NULL;
  int n = 0;
  while(1)
  {
    pthread_mutex_lock(&pos->mutex);
    if (
      pos->fetched==0
    )
    {
      #ifdef _DEBUG
        printf("updateListThread update\n");
      #endif
      binarySemaphoreWait(bigInterface->activity.sem->id);
      order = pos->orders.order;
      n = 0;
      while(order)
      {
        updateListWithOrderSYNCED(order,n);
        order = order->next;
        n++;
      }
      binarySemaphorePost(bigInterface->activity.sem->id);
      tr_WindowExpose(wmain);
      pos->fetched = 1;
    }
    pthread_mutex_unlock(&pos->mutex);
    sleep(_UPDATE_LIST_TIK);
  }
}

void *onBack(void*data)
{
  #ifdef _DEBUG
    printf("onBack\n");
  #endif
  tr_WindowToTop(wmain);
  tr_WindowExpose(wmain);
}

void *onFinished(void*data)
{
  #ifdef _DEBUG
    printf("onFinished\n");
  #endif
  tr_WindowToTop(wmain);
  tr_WindowExpose(wmain);
}

void *onRemove(void*data)
{
  #ifdef _DEBUG
    printf("onRemove\n");
  #endif
  tr_WindowToTop(wmain);
  tr_WindowExpose(wmain);
}

void *onCall(void*data)
{
  #ifdef _DEBUG
    printf("onCall\n");
  #endif
  tr_WindowToTop(wmain);
  tr_WindowExpose(wmain);
}

void *onStock(void*data)
{
  #ifdef _DEBUG
    printf("onStock\n");
  #endif
  tr_WindowToTop(wmain);
  tr_WindowExpose(wmain);
}

int main( )
{
  osInit();
  trop_Init();
  gui_Init();
  tr_WmInit();
  
  pos = kitchenPosInit(pos);
  kitchenPosCreateTestList(pos);
  kitchenPosQueue(pos);
  
  layout = NULL;
  rotation = ROT_NONE;
  
  mainAttr.x = 0;
  mainAttr.y = 0;
  mainAttr.w = 800;
  mainAttr.h = 600;
  
  root = tr_WindowInit(NULL,root,&mainAttr);
  wmain = tr_WindowInit(NULL,root,&mainAttr);
  wdetail = tr_WindowInit(NULL,root,&mainAttr);
  
  tr_WindowMap(wmain);
  activitySetMovable(&bigInterface->activity,0,1);
  
  tr_WindowToTop(wmain);
  cmain = tr_CanvasInit(NULL,&mainAttr,wmain);
  tr_SetRound(cmain,3);
  tr_SetColor(cmain,(char*)"#fefefe");
  form_CanvasSetMov(cmain->widget.sub,CAN_MOV_NONE);
  buttonAttr.x = 10;
  buttonAttr.y = 10;
  buttonAttr.w = mainAttr.w-20;
  buttonAttr.h = 70;
  title = tr_ButtonInitEx2(NULL,&buttonAttr,cmain);
  tr_SetColor(title,(char*)"#808080");
  tr_SetRound(title,3);
  tr_SetFontColor(title,0xff,0xff,0xff,240);
  tr_SetFontSize(title,26);
  tr_ChangeTextExPos(title,(char*)"title",20,buttonAttr.h/2);
  tr_CanvasAddObjStatic(cmain,title);
    
  tr_WindowToTop(wdetail);
  cdetail = tr_CanvasInit(NULL,&mainAttr,wdetail);
  tr_SetRound(cdetail,3);
  tr_SetColor(cdetail,(char*)"#303030");
  form_CanvasSetMov(cdetail->widget.sub,CAN_MOV_NONE);
  buttonAttr.x = mainAttr.w-200;
  buttonAttr.y = mainAttr.h-100;
  buttonAttr.w = 190;
  buttonAttr.h = 90;
  button = tr_ButtonInitEx2(NULL,&buttonAttr,cdetail);
  tr_SetColor(button,(char*)"#808080");
  tr_SetRound(button,3);
  tr_SetFontColor(button,0xff,0xff,0xff,240);
  tr_SetFontSize(button,26);
  tr_ChangeTextExPos(button,(char*)"back",20,buttonAttr.h/2);
  tr_SetAction(button,onBack,TR_ACTION_BUTTON);
  tr_CanvasAddObjStatic(cdetail,button);
  detailTextAttr.x = 10;
  detailTextAttr.y = 10;
  detailTextAttr.w = mainAttr.w-20;
  detailTextAttr.h = 120;
  detailText = tr_ButtonInitEx2(NULL,&detailTextAttr,cdetail);
  tr_SetColor(detailText,(char*)"#404040");
  tr_SetRound(detailText,3);
  tr_SetFontColor(detailText,0xff,0xff,0xff,240);
  tr_SetFontSize(detailText,20);
  tr_ChangeTextExPos(detailText,(char*)"order details",20,detailTextAttr.h/2);
  tr_CanvasAddObjStatic(cdetail,detailText);
  
  detailData.order = NULL;
  // (finished, remove, signal bar, out of stock) buttons
  buttonAttr.x = 10;
  buttonAttr.y = 140;
  buttonAttr.w = (mainAttr.w/2)-30;
  buttonAttr.h = 140;
  button = tr_ButtonInitEx2(NULL,&buttonAttr,cdetail);
  tr_SetColor(button,(char*)"#ababab");
  tr_SetRound(button,3);
  tr_SetFontColor(button,0xff,0xff,0xff,240);
  tr_SetFontSize(button,24);
  tr_ChangeTextExPos(button,(char*)"Prepaired (signal bar)",20,buttonAttr.h/2);
  tr_SetAction(button,onFinished,TR_ACTION_BUTTON);
  tr_CanvasAddObjStatic(cdetail,button);
  buttonAttr.x = (mainAttr.w/2)-10;
  buttonAttr.y = 140;
  buttonAttr.w = (mainAttr.w/2)-30;
  buttonAttr.h = 140;
  button = tr_ButtonInitEx2(NULL,&buttonAttr,cdetail);
  tr_SetColor(button,(char*)"#ababab");
  tr_SetRound(button,3);
  tr_SetFontColor(button,0xff,0xff,0xff,240);
  tr_SetFontSize(button,24);
  tr_ChangeTextExPos(button,(char*)"Remove order",20,buttonAttr.h/2);
  tr_SetAction(button,onRemove,TR_ACTION_BUTTON);
  tr_CanvasAddObjStatic(cdetail,button);
  buttonAttr.x = 10;
  buttonAttr.y = 290;
  buttonAttr.w = (mainAttr.w/2)-30;
  buttonAttr.h = 140;
  button = tr_ButtonInitEx2(NULL,&buttonAttr,cdetail);
  tr_SetColor(button,(char*)"#ababab");
  tr_SetRound(button,3);
  tr_SetFontColor(button,0xff,0xff,0xff,240);
  tr_SetFontSize(button,24);
  tr_ChangeTextExPos(button,(char*)"Request waiter (call bar)",20,buttonAttr.h/2);
  tr_SetAction(button,onCall,TR_ACTION_BUTTON);
  tr_CanvasAddObjStatic(cdetail,button);
  buttonAttr.x = (mainAttr.w/2)-10;
  buttonAttr.y = 290;
  buttonAttr.w = (mainAttr.w/2)-30;
  buttonAttr.h = 140;
  button = tr_ButtonInitEx2(NULL,&buttonAttr,cdetail);
  tr_SetColor(button,(char*)"#ababab");
  tr_SetRound(button,3);
  tr_SetFontColor(button,0xff,0xff,0xff,240);
  tr_SetFontSize(button,24);
  tr_ChangeTextExPos(button,(char*)"Out of stock",20,buttonAttr.h/2);
  tr_SetAction(button,onStock,TR_ACTION_BUTTON);
  tr_CanvasAddObjStatic(cdetail,button);
  //
  tr_WindowToTop(wmain);
  
  orderAttr.h = 90;
  
  int n;
  tmpAttr.x = 5;
  tmpAttr.y = orderAttr.h+5;
  tmpAttr.w = mainAttr.w-90;
  tmpAttr.h = ((mainAttr.h-orderAttr.h-20)/_KITCHEN_ORDERS)-5;
  for(n = 0; n < _KITCHEN_ORDERS; n++)
  {
    orders.olist[n].widget = tr_ButtonInitEx2(NULL,&tmpAttr,cmain);
    tr_SetColor(orders.olist[n].widget,(char*)"#b0b0b0");
    tr_SetRound(orders.olist[n].widget,2);
    tr_SetFontColor(orders.olist[n].widget,0xff,0xff,0xff,240);
    tr_SetFontSize(orders.olist[n].widget,20);
    tr_ChangeTextExPos(orders.olist[n].widget,(char*)"order",100,tmpAttr.h/2);
    tr_SetAction(orders.olist[n].widget,onOrder,TR_ACTION_BUTTON);
    tr_CanvasAddObjStatic(cmain,orders.olist[n].widget);
    tmpAttr.y+=tmpAttr.h+5;
  }
  
  pthread_t updateThread;
  pthread_create(&updateThread,NULL,updateListThread,NULL);
  
  tr_WindowToTop(wmain);
  tr_WindowQueue(root);
  
  return 0;
}
