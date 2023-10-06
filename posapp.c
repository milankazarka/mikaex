extern "C" {
  #include "mikaPosClient.h"
}
#include "mikaListbox.h"
#include "mikaList.h"
#include "mikaListData.h"
#include "posapp.h"
#include "posSetup.h"

#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"

#define _APP_SIZE 	380
#define _MENU_SIZE 	320
#define _APP_SERVER 	"127.0.0.1"
#define _APP_PORT 	21000
#define _APP_GUI_CORNER	3
#define _SINGLE_TOPLEVEL_NO // if this app is to be the only app movable

extern "C" {
  #include "global.h"
}

struct tr_Window 	*root		= NULL;
struct tr_Window 	*wmain		= NULL;
struct tr_Window	*worder		= NULL;
struct tr_Window	*winfo		= NULL;
struct tr_Window	*worders	= NULL;

struct tr_Object 	*cmain		= NULL;
struct tr_Object	*corder		= NULL;
struct tr_Object	*cinfo		= NULL;
struct tr_Object	*corders	= NULL;

struct tr_Object	*bup		= NULL;
struct tr_Object	*bdown		= NULL;
struct tr_Object	*borders	= NULL;
struct tr_Object	*binfo		= NULL;
struct tr_Object	*bback		= NULL;
struct tr_Object	*image		= NULL;
struct tr_Object	*border		= NULL;
struct tr_Object	*baccept	= NULL;
struct tr_Object	*bdecline	= NULL;
struct tr_Object	*bordersinfo	= NULL;
mikaListbox 		*dlist		= NULL;

mikaListData		*dlistdata	= NULL;
mikaList		*dlistex	= NULL;

struct tr_Attr		mainAttr;
struct tr_Attr		listAttr;
struct tr_Attr		attr;

struct tr_Window	*current	= NULL;
struct mikaListboxDelegate 		delegate;

struct posSetup		*pos		= NULL;
struct mikaPosClient	client;
struct mikaPosOrder	order;

typedef struct posapp_order
{
  struct posapp_order *next;
} posapp_order;

struct posapp_order *orders = NULL;

typedef struct posapp_t
{
} posapp_t;

void flushList( )
{
  #ifdef _DEBUG
    printf("flushList\n");
  #endif
  
  mikaListItem *current = dlistex->items;
  while(current)
  {
    tr_Object *button = current->button;
    
    struct tr_Attr imageAttr;
    imageAttr.x = 0;
    imageAttr.y = 0;
    imageAttr.w = 70;
    imageAttr.h = 30;
    struct tr_Object *image = tr_ImageInitEx(NULL,&attr,(char*)"imgSushi/blank.jpg");
        
    tr_ChangeTextExPos(button,(char*)" ",30,15);
    tr_AddImage(button,image);
    
    current = current->next;
  }
}

void *onBup(void*data)
{
  #ifdef _DEBUG
    printf("onBup\n");
  #endif
  flushList();
  if (dlist)
    dlist->onListUp();
  if (dlistex)
    dlistex->moveUp();
}

void *onBdown(void*data)
{
  #ifdef _DEBUG
    printf("onBdown\n");
  #endif
  flushList();
  if (dlist)
    dlist->onListDown();
  if (dlistex)
    dlistex->moveDown();
}

void *onBorders(void*data)
{
  #ifdef _DEBUG
    printf("onBorders\n");
  #endif
  current = worders;
  tr_WindowToTop(worders);
  tr_WindowExpose(worders);
}

void *onInfo(void*data)
{
  #ifdef _DEBUG
    printf("onInfo\n");
  #endif
  current = winfo;
  tr_WindowToTop(winfo);
  tr_WindowExpose(winfo);
}

void *onBack(void*data)
{
  #ifdef _DEBUG
    printf("onBack (%p)\n",data);
  #endif
  flushList();
  if (
    current==winfo ||
    current==worders ||
    current==worder
    )
  {
    current = wmain;
    tr_WindowToTop(wmain);
    tr_WindowExpose(wmain);
  }
  else if (current==wmain)
  {
    //dlist->onListBack();
    dlistex->moveBack();
  }
}

int generatePanelForCanvas( struct tr_Object *canvas )
{
  #ifdef _DEBUG
    printf("generatePanelForCanvas\n");
  #endif
  
  // bottom panel orders
  attr.x = 1;
  attr.y = _MENU_SIZE-70;//_APP_SIZE-70;
  attr.w = 107;
  attr.h = 70;
  borders = tr_ButtonInitEx2(NULL,&attr,canvas);
  tr_SetAction(borders,onBorders,TR_ACTION_BUTTON);
  #ifndef _USE_SKIN
    tr_SetColor(borders,(char*)"#303030");
  #endif
  tr_SetRound(borders,3);
  tr_CanvasAddObjStatic(canvas,borders);
  image = tr_ImageInitEx(NULL,&attr,(char*)"posappImages/my_orders_butt.jpg");
  tr_AddImage(borders,image);
  tr_ImagePosCenter(image,borders);
  // button panel info
  attr.x += attr.w;
  binfo = tr_ButtonInitEx2(NULL,&attr,canvas);
  tr_SetAction(binfo,onInfo,TR_ACTION_BUTTON);
  #ifndef _USE_SKIN
    tr_SetColor(binfo,(char*)"#303030");
  #endif
  tr_SetRound(binfo,3);
  tr_CanvasAddObjStatic(canvas,binfo);
  image = tr_ImageInitEx(NULL,&attr,(char*)"posappImages/info_butt.jpg");
  tr_AddImage(binfo,image);
  tr_ImagePosCenter(image,binfo);
  // button panel back
  attr.x += attr.w;
  attr.w = 106;
  bback = tr_ButtonInitEx2(NULL,&attr,canvas);
  tr_SetAction(bback,onBack,TR_ACTION_BUTTON);
  #ifndef _USE_SKIN
    tr_SetColor(bback,(char*)"#303030");
  #endif
  tr_SetRound(bback,3);
  tr_CanvasAddObjStatic(canvas,bback);
  image = tr_ImageInitEx(NULL,&attr,(char*)"posappImages/back_butt.jpg");
  tr_AddImage(bback,image);
  tr_ImagePosCenter(image,bback);
  
  return 0;
}

static struct mikaListboxNode *selectedNode = NULL;
void *onOrderItem(struct mikaListboxNode*node)
{
  #ifdef _DEBUG
    printf("onOrderItem\n");
  #endif
  
  selectedNode = node;
  
  current = worder;
  tr_WindowToTop(worder);
  tr_WindowExpose(worder);
  
  return NULL;
}

void *onOrderAccept(void*data)
{
  if (selectedNode)
  { 
    char *text = (char*)malloc(32);
    strcpy(text,(char*)selectedNode->name);
    tr_ChangeTextExPos(bordersinfo,text,10,40);
  
    sprintf((char*)order.value,"T1:%s:new",
      (char*)selectedNode->name);
    mikaPosClientCommandAddOrder(&client,&order);
  }
  
  current = wmain;
  tr_WindowToTop(wmain);
  tr_WindowExpose(wmain);
}

void *onOrderDecline(void*data)
{
  current = wmain;
  tr_WindowToTop(wmain);
  tr_WindowExpose(wmain);
}

void *onListNode( TiXmlElement *element, int position )
{
  if (!element)
  {
    return NULL;
  }
  #ifdef _DEBUG
    printf("\tposapp mikaList onListNode position(%d)\n",position);
  #endif
  
  unsigned char text[128];
  int count = 0;
  mikaListItem *current = dlistex->items;
  while(current)
  {
    if (dlistex->nitems-position-1==count)
    {
      if (element->Attribute("name"))
      {
        strcpy((char*)text,(char*)element->Attribute("name"));
        #ifdef _DEBUG
          printf("\tonListNode mikaList name(%s)\n",(char*)text);
        #endif
        
        struct tr_Attr imageAttr;
        imageAttr.x = 0;
        imageAttr.y = 0;
        imageAttr.w = 70;
        imageAttr.h = 30;
        struct tr_Object *image = tr_ImageInitEx(NULL,&attr,(char*)"imgSushi/sushiSake002small.jpg");
        
        struct tr_Object *button = current->button;
        tr_ChangeTextExPos(button,(char*)text,30,15);
        tr_AddImage(button,image);
      }
    }
    current = current->next;
    count++;
  }
  
  tr_WindowToTop(wmain);
  tr_WindowExpose(wmain);
  
  return NULL;
}

void *onSelection( int position )
{
  flushList();
  return NULL;
}

int main( )
{
  //
  /**
  mikaListDataDelegate *lbd =
    (struct mikaListDataDelegate*)malloc(sizeof(struct mikaListDataDelegate));
  lbd->onNodeElement = onListNode;
  mikaListData *ld = new mikaListData((unsigned char*)"pos.xml");
  ld->delegate = lbd;
  ld->moveToElementAtPosition(2);
  ld->back();
  exit(2);
  */
  char *text = NULL;
  
  osInit();
  trop_Init();
  gui_Init();
  tr_WmInit();
  
  layout = NULL;
  rotation = ROT_NONE;
  
  mainAttr.x = 0;
  mainAttr.y = 0;
  mainAttr.w = _APP_SIZE;
  mainAttr.h = _APP_SIZE;
  
  root 		= tr_WindowInit(NULL,root,&mainAttr);
  wmain 	= tr_WindowInit(NULL,root,&mainAttr);
  worder 	= tr_WindowInit(NULL,root,&mainAttr);
  winfo 	= tr_WindowInit(NULL,root,&mainAttr);
  worders 	= tr_WindowInit(NULL,root,&mainAttr);
  
  tr_WindowMap(wmain);
  tr_WindowToTop(wmain);
  
  cmain = tr_CanvasInit(NULL,&mainAttr,wmain);
  form_CanvasSetMov(cmain->widget.sub,CAN_MOV_NONE);
  tr_SetRound(cmain,_APP_GUI_CORNER);
  #ifndef _USE_SKIN
    tr_SetColor(cmain,(char*)"#000000");
  #endif
  
  listAttr.x = 1;
  listAttr.y = 1;//_APP_SIZE-_MENU_SIZE;
  listAttr.w = 250;
  listAttr.h = 250;
  /**
  dlist = new mikaListbox(cmain,&listAttr);
  dlist->window = wmain;
  dlist->loadNodeFile((unsigned char*)"mikaListbox.xml");
  dlist->listNodes(NULL);
  dlist->delegate = &delegate;
  delegate.onOrder = onOrderItem;
  */
  struct mikaListDelegate *dlistexdelegate =
    (struct mikaListDelegate*)malloc(sizeof(struct mikaListDelegate));
  dlistexdelegate->onNodeElement = onListNode;
  dlistexdelegate->onSelection = onSelection;
  dlistex = new mikaList(cmain,&listAttr,4);
  dlistex->delegate = dlistexdelegate;
  dlistex->loadFile((unsigned char*)"pos.xml");
  dlistex->update();
  
  // slidebar buttons
  attr.x = 251;
  attr.y = 1; //_APP_SIZE-_MENU_SIZE;
  attr.w = 70;
  attr.h = 125;
  bup = tr_ButtonInitEx2(NULL,&attr,cmain);
  tr_SetAction(bup,onBup,TR_ACTION_BUTTON);
  tr_SetColor(bup,(char*)"#303030");
  tr_SetRound(bup,3);
  tr_CanvasAddObjStatic(cmain,bup);
  image = tr_ImageInitEx(NULL,&attr,(char*)"posappImages/up_butt.jpg");
  tr_AddImage(bup,image);
  tr_ImagePosCenter(image,bup);
  //
  attr.x = 251;
  attr.y = 125;//+(_APP_SIZE-_MENU_SIZE);
  bdown = tr_ButtonInitEx2(NULL,&attr,cmain);
  tr_SetAction(bup,onBdown,TR_ACTION_BUTTON);
  tr_SetColor(bdown,(char*)"#303030");
  tr_SetRound(bdown,3);
  tr_CanvasAddObjStatic(cmain,bdown);
  image = tr_ImageInitEx(NULL,&attr,(char*)"posappImages/down_butt.jpg");
  tr_AddImage(bdown,image);
  tr_ImagePosCenter(image,bdown);
  
  generatePanelForCanvas(cmain);
  
  tr_WindowToTop(worder);
  corder = tr_CanvasInit(NULL,&mainAttr,worder);
  form_CanvasSetMov(corder->widget.sub,CAN_MOV_NONE);
  tr_SetRound(corder,1);
  #ifndef _USE_SKIN
    tr_SetColor(corder,(char*)"#505050");
  #endif
  generatePanelForCanvas(corder);
  attr.x = 1;
  attr.y = 1;
  attr.w = 320;
  attr.h = 250;
  border = tr_ButtonInitEx2(NULL,&attr,corder);
  tr_SetColor(border,(char*)"#303030");
  tr_SetRound(border,_APP_GUI_CORNER);
  //tr_CanvasAddObjStatic(corder,border);
  attr.x = 10;
  attr.y = 10;
  attr.w = 300;
  attr.h = 80;
  baccept = tr_ButtonInitEx2(NULL,&attr,corder);
  tr_SetColor(baccept,(char*)"#d0d0d0");
  tr_SetFontColor(baccept,0x20,0x20,0x20,240);
  tr_SetRound(baccept,_APP_GUI_CORNER);
  tr_SetFontSize(baccept,20);
  text = (char*)malloc(32);
  strcpy(text,"Accept");
  tr_ChangeTextExPos(baccept,text,10,40);
  tr_SetAction(baccept,onOrderAccept,TR_ACTION_BUTTON);
  tr_CanvasAddObjStatic(corder,baccept);
  attr.x = 10;
  attr.y = 120;
  attr.w = 300;
  attr.h = 80;
  bdecline = tr_ButtonInitEx2(NULL,&attr,corder);
  tr_SetColor(bdecline,(char*)"#d0d0d0");
  tr_SetFontColor(bdecline,0x20,0x20,0x20,240);
  tr_SetRound(bdecline,3);
  tr_SetFontSize(bdecline,20);
  text = (char*)malloc(32);
  strcpy(text,"Decline");
  tr_ChangeTextExPos(bdecline,text,10,40);
  tr_SetAction(bdecline,onOrderDecline,TR_ACTION_BUTTON);
  tr_CanvasAddObjStatic(corder,bdecline);

  tr_WindowToTop(winfo);
  cinfo = tr_CanvasInit(NULL,&mainAttr,winfo);
  form_CanvasSetMov(cinfo->widget.sub,CAN_MOV_NONE);
  tr_SetRound(cinfo,1);
  #ifndef _USE_SKIN
    tr_SetColor(cinfo,(char*)"#505050");
  #endif
  generatePanelForCanvas(cinfo);
  
  tr_WindowToTop(worders);
  corders = tr_CanvasInit(NULL,&mainAttr,worders);
  form_CanvasSetMov(corders->widget.sub,CAN_MOV_NONE);
  tr_SetRound(corders,1);
  #ifndef _USE_SKIN
    tr_SetColor(corders,(char*)"#505050");
  #endif
  generatePanelForCanvas(corders);
  attr.x = 0;
  attr.y = 0;
  attr.w = 320;
  attr.h = 250;
  bordersinfo = tr_ButtonInitEx2(NULL,&attr,corders);
  #ifndef _USE_SKIN
    tr_SetColor(bordersinfo,(char*)"#303030");
  #endif
  tr_SetFontColor(bordersinfo,0xfa,0xfa,0xfa,240);
  tr_SetRound(bordersinfo,_APP_GUI_CORNER);
  tr_SetFontSize(bordersinfo,20);
  text = (char*)malloc(32);
  strcpy(text," Orders Info");
  tr_ChangeTextExPos(bordersinfo,text,10,40);
  tr_CanvasAddObjStatic(corders,bordersinfo);

  current = wmain;
  tr_WindowToTop(wmain);
  
  pos = posSetupInit(NULL);
  strcpy((char*)pos->server,_APP_SERVER);
  pos->port = _APP_PORT;
  
  mikaPosClientInit(&client);
  strcpy((char*)client.server,(char*)pos->server);
  client.port = pos->port;
  
  #ifdef _SINGLE_TOPLEVEL
    activitySetToplevel(&bigInterface->activity,1,1);
  #endif
  
  interfaceSetBackplaneSize(0,0);
  
  tr_WindowQueue(root);
  
  return 0;
}
