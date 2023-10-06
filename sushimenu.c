#include "sushimenu.h"

sushimenu *menu = NULL;

void flushList( );

void *onBack( void *data )
{
  flushList();
  menu->list->moveBack();
  return NULL;
}

void *onUp( void *data )
{
  flushList();
  menu->list->moveUp();
  return NULL;
}

void *onDown( void *data )
{
  flushList();
  menu->list->moveDown();
  return NULL;
}

int initCanvas( struct tr_Object *canvas )
{
  if (!canvas)
    return 1;
  form_CanvasSetMov(canvas->widget.sub,CAN_MOV_NONE);
  tr_SetRound(canvas,2);
  tr_SetColor(canvas,(char*)"#ffffff");
  

  struct tr_Attr battr;
  battr = tr_GetAttr(200,50,10,0);
  struct tr_Object *title = tr_ButtonInitEx2(NULL,&battr,canvas);
  tr_SetColor(title,(char*)"#ffffff");
  tr_SetRound(title,2);
  tr_CanvasAddObjStatic(canvas,title);
  struct tr_Object *titleBg = tr_ImageInitEx(NULL,&battr,(char*)"imgSushi/logo.jpg");
  tr_AddImage(title,titleBg);
  tr_ImagePosCenter(titleBg,title);
  
  battr = tr_GetAttr(80,80,_APP_SIZE_W-100,_APP_SIZE_H-90);
  struct tr_Object *back = tr_ButtonInitEx2(NULL,&battr,canvas);
  tr_SetColor(back,(char*)"#ffffff");
  tr_SetRound(back,3);
  tr_CanvasAddObjStatic(canvas,back);
  tr_SetAction(back,onBack,TR_ACTION_BUTTON);
  struct tr_Object *image = tr_ImageInitEx(NULL,&battr,(char*)"img/home_small_new2.jpg");
  tr_AddImage(back,image);
  tr_ImagePosCenter(image,back);
  
  battr = tr_GetAttr(80,80,_APP_SIZE_W-103,(_APP_SIZE_H/2)-110);
  struct tr_Object *up = tr_ButtonInitEx2(NULL,&battr,canvas);
  tr_SetColor(up,(char*)"#ffffff");
  tr_SetRound(up,3);
  tr_CanvasAddObjStatic(canvas,up);
  tr_SetAction(up,onUp,TR_ACTION_BUTTON);
  image = tr_ImageInitEx(NULL,&battr,(char*)"img/uparrow.jpg");
  tr_AddImage(up,image);
  tr_ImagePosCenter(image,up);
  
  battr = tr_GetAttr(80,80,_APP_SIZE_W-103,(_APP_SIZE_H/2)+20);
  struct tr_Object *down = tr_ButtonInitEx2(NULL,&battr,canvas);
  tr_SetColor(down,(char*)"#ffffff");
  tr_SetRound(down,3);
  tr_CanvasAddObjStatic(canvas,down);
  tr_SetAction(down,onDown,TR_ACTION_BUTTON);
  image = tr_ImageInitEx(NULL,&battr,(char*)"img/downarrow.jpg");
  tr_AddImage(down,image);
  tr_ImagePosCenter(image,down);
  
  return 0;
}

void flushList( )
{
  #ifdef _DEBUG
    printf("posapp mikaList flushList\n");
  #endif
  mikaListItem *current = menu->list->items;
  while(current)
  {
    tr_Object *button = current->button;
    
    struct tr_Attr attr;
    attr = tr_GetAttr(70,30,0,0);
    struct tr_Object *image = tr_ImageInitEx(NULL,&attr,(char*)"imgSushi/blank.jpg");
        
    tr_ChangeTextExPos(button,(char*)" ",150,50);
    tr_ChangeTextExPos(button,(char*)" ",150,70); 
    tr_AddImage(button,image);
    
    current = current->next;
  }
}

void *onListNodeEx( TiXmlElement *element, int position )
{
  if (!element || position<0)
  {
    return NULL;
  }
  #ifdef _DEBUG
    printf("\tposapp mikaList onListNodeEx position(%d)\n",position);
  #endif
  
  unsigned char text[128];
  int count = 0;
  
  mikaListItem *current = menu->list->items;

  while(current)
  {
    #ifdef _DEBUG
      printf("\tposapp mikaList onListNodeEx loop\n");
    #endif
    
    if (menu->list->nitems-position-1==count)
    {
      struct tr_Object *button = current->button;
      if (element->Attribute("image"))
      {
        #ifdef _DEBUG
          printf("\tposapp mikaList onListNodeEx reading image(%s)\n",(char*)element->Attribute("image"));
        #endif
        struct tr_Attr attr;
        attr = tr_GetAttr(150,70,0,0);
        struct tr_Object *image = tr_ImageInitEx(NULL,&attr,(char*)element->Attribute("image"));
        tr_AddImage(button,image);
      }
      if (element->Attribute("name"))
      {
        strcpy((char*)text,(char*)element->Attribute("name"));
        #ifdef _DEBUG
          printf("\tonListNode mikaList name(%s)\n",(char*)text);
        #endif
        tr_ChangeTextExPos(button,(char*)text,150,50);        
      }
      if (element->Attribute("price"))
      {
        strcpy((char*)text,(char*)element->Attribute("price"));
        #ifdef _DEBUG
          printf("\tonListNode mikaList price(%s)\n",(char*)text);
        #endif
        tr_ChangeTextExPos(button,(char*)text,150,70);      
      }
    }
    
    current = current->next;
    count++;
  }
  
  tr_WindowToTop(menu->wmain);
  tr_WindowExpose(menu->wmain);
 
  return NULL;
}

void *onSelection( int position )
{
  flushList();
  return NULL;
}

void *onLowest( void *sender, TiXmlElement *element, int position )
{
  if (!sender || !element)
    return NULL;
  
  #ifdef _DEBUG
    printf("posapp mikaList onLowest - show order form\n");
  #endif  
  
  menu->menuOrder->currentElement = element;
  menu->menuOrder->raise();
  
  return NULL;
}

int sushimenu::onListNode( TiXmlElement *element, int position )
{
  return 0;
}

sushimenu::sushimenu( )
{
  #ifdef _DEBUG
    printf("sushimenu::sushimenu\n");
  #endif
  
  mainAttr.x = 0;
  mainAttr.y = 0;
  mainAttr.w = _APP_SIZE_W;
  mainAttr.h = _APP_SIZE_H;
  
  menu = this;
  
  root    = tr_WindowInit(NULL,NULL,&mainAttr);
  wmain   = tr_WindowInit(NULL,root,&mainAttr);
  
  tr_WindowMap(wmain);
  tr_WindowToTop(wmain);
  cmain = tr_CanvasInit(NULL,&mainAttr,wmain);
  
  initCanvas(cmain);
  
  struct tr_Attr listAttr;
  listAttr.x = 0;
  listAttr.y = 50;
  listAttr.w = _APP_SIZE_W-100;
  listAttr.h = _APP_SIZE_H-listAttr.y-10;
  listdelegate.onNodeElement = onListNodeEx;
  listdelegate.onSelection = onSelection;
  listdelegate.onLowest = onLowest;
  list = new mikaList(cmain,&listAttr,4);
  list->delegate = &listdelegate;
  list->loadFile((unsigned char*)"sushi.xml");
  list->update();
  
  // init the ordering menu
  menuOrder = new sushiMenuOrder(root);
  menuBill = new sushiMenuBill(root);
  
  interfaceSetBackplaneSize(0,0);
  activitySetMovable(&bigInterface->activity,1,0);
  
  tr_WindowToTop(wmain);
  tr_WindowQueue(root);
}

sushimenu::~sushimenu( )
{
  #ifdef _DEBUG
    printf("sushimenu::~sushimenu\n");
  #endif
}

int sushimenu::raise( )
{
  tr_WindowToTop(wmain);
  tr_WindowExpose(wmain);
  list->data->delegateNode();
  return 0;
}

void *onOrderBack( void *data )
{
  #ifdef _DEBUG
    printf("onOrderBack\n");
  #endif
  
  menu->raise();
  
  return NULL;
}

void *onOrder( void *data )
{
  #ifdef _DEBUG
    printf("onOrder\n");
  #endif
  menu->menuBill->raise();
  //menu->raise();
}

void *onCancel( void *data )
{
  #ifdef _DEBUG
    printf("onCancel\n");
  #endif
  menu->raise();
}

sushiMenuOrder::sushiMenuOrder( struct tr_Window *parentWindow )
{
  #ifdef _DEBUG
    printf("sushimenuOrder::sushiMenuOrder\n");
  #endif
  
  attr = tr_GetAttr(_APP_SIZE_W,_APP_SIZE_H,0,0);
  currentElement = NULL;
    
  window = tr_WindowInit(NULL,parentWindow,&attr);
  tr_WindowMap(window);
  tr_WindowToTop(window);
  
  canvas = tr_CanvasInit(NULL,&attr,window);
  form_CanvasSetMov(canvas->widget.sub,CAN_MOV_NONE);
  tr_SetRound(canvas,2);
  tr_SetColor(canvas,(char*)"#ffffff");
  
  struct tr_Attr battr;
  battr = tr_GetAttr(200,50,10,0);
  struct tr_Object *title = tr_ButtonInitEx2(NULL,&battr,canvas);
  tr_SetColor(title,(char*)"#ffffff");
  tr_SetRound(title,2);
  tr_CanvasAddObjStatic(canvas,title);
  struct tr_Object *titleBg = tr_ImageInitEx(NULL,&battr,(char*)"imgSushi/logo.jpg");
  tr_AddImage(title,titleBg);
  tr_ImagePosCenter(titleBg,title);
  
  battr = tr_GetAttr(80,80,_APP_SIZE_W-100,_APP_SIZE_H-90);
  struct tr_Object *back = tr_ButtonInitEx2(NULL,&battr,canvas);
  tr_SetColor(back,(char*)"#ffffff");
  tr_SetRound(back,3);
  tr_CanvasAddObjStatic(canvas,back);
  tr_SetAction(back,onOrderBack,TR_ACTION_BUTTON);
  struct tr_Object *image = tr_ImageInitEx(NULL,&battr,(char*)"img/home_small_new2.jpg");
  tr_AddImage(back,image);
  tr_ImagePosCenter(image,back);
  
  battr = tr_GetAttr(250,90,50,160);
  order = tr_ButtonInitEx2(NULL,&battr,canvas);
  tr_SetColor(order,(char*)"#303030");
  tr_SetRound(order,3);
  tr_CanvasAddObjStatic(canvas,order);
  tr_SetAction(order,onOrder,TR_ACTION_BUTTON);
  tr_SetFontColorEx(order,(char*)"#ffffff");
  tr_SetFontSize(order,23);
  tr_ChangeTextExPos(order,(char*)"Order",20,50);
  
  battr.y += 150;
  cancel = tr_ButtonInitEx2(NULL,&battr,canvas);
  tr_SetColor(cancel,(char*)"#303030");
  tr_SetRound(cancel,3);
  tr_CanvasAddObjStatic(canvas,cancel);
  tr_SetAction(cancel,onCancel,TR_ACTION_BUTTON);
  tr_SetFontColorEx(cancel,(char*)"#ffffff");
  tr_SetFontSize(cancel,23);
  tr_ChangeTextExPos(cancel,(char*)"Cancel",20,50);

  battr = tr_GetAttr(300,100,50,30);
  infoText = tr_ButtonInitEx2(NULL,&battr,canvas);
  tr_SetColor(infoText,(char*)"#ffffff");
  tr_SetRound(infoText,3);
  tr_CanvasAddObjStatic(canvas,infoText);
  tr_SetFontColorEx(infoText,(char*)"#202020");
  tr_SetFontSize(infoText,18);
  tr_ChangeTextExPos(infoText,(char*)"Info Text",20,50);
}

sushiMenuOrder::~sushiMenuOrder( )
{
  
}

int sushiMenuOrder::raise( )
{
  #ifdef _DEBUG
    printf("sushiMenuOrder::raise mikaList\n");
  #endif
  tr_WindowToTop(window);
  if (currentElement)
  {
    
    if (currentElement->Attribute("name"))
    {
      tr_ChangeTextExPos(infoText,(char*)currentElement->Attribute("name"),20,50);
    }
    if (currentElement->Attribute("price"))
    {
      tr_ChangeTextExPos(infoText,(char*)currentElement->Attribute("price"),20,70);
    }
  }
  tr_WindowExpose(window);
  return 0;
}

void *onBillConfirm( void *data )
{
  #ifdef _DEBUG
    printf("onBillConfirm\n");
  #endif
}

sushiMenuBill::sushiMenuBill( struct tr_Window *parentWindow )
{
  attr = tr_GetAttr(_APP_SIZE_W,_APP_SIZE_H,0,0);
    
  window = tr_WindowInit(NULL,parentWindow,&attr);
  tr_WindowMap(window);
  tr_WindowToTop(window);
  
  canvas = tr_CanvasInit(NULL,&attr,window);
  form_CanvasSetMov(canvas->widget.sub,CAN_MOV_NONE);
  tr_SetRound(canvas,2);
  tr_SetColor(canvas,(char*)"#ffffff");

  struct tr_Attr battr = tr_GetAttr(_APP_SIZE_W-200,160,100,150);
  info = tr_ButtonInitEx2(NULL,&battr,canvas);
  tr_SetColor(info,(char*)"#7bc45e");
  tr_SetRound(info,3);
  tr_CanvasAddObjStatic(canvas,info);
  tr_SetFontColorEx(info,(char*)"#ffffff");
  tr_SetFontSize(info,18);
  tr_ChangeTextExPos(info,(char*)"Order Sent",50,90);
  tr_SetAction(info,onCancel,TR_ACTION_BUTTON);
}

int sushiMenuBill::raise( )
{
  tr_WindowToTop(window);
  tr_WindowExpose(window);
  return 0;
}

sushiMenuBill::~sushiMenuBill( )
{
}

int main( )
{
  interfaceSetName((unsigned char*)"SUSHIMENU");

  osInit();
  trop_Init();
  gui_Init();
  tr_WmInit();
  
  layout = NULL;
  rotation = ROT_NONE;
  
  menu = new sushimenu();
  
  return 0;
}
