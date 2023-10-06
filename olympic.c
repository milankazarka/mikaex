extern "C" {
  #include "interface.h"
  #include "config.h"
  #include "os.h"
  #include "system.h"
  #include "global.h"
  #include "xml.h"
  #include "wm.h"
  #include "dom.h"
  #include "activityObject.h"
  #include "rootControllerSync.h"
  #include "mikaPosClient.h"
}

struct mikaPosClient client;
struct mikaPosOrder order;

struct tr_Window *root		= NULL;
struct tr_Window *wmain		= NULL;
struct tr_Window *wnealko	= NULL;
struct tr_Window *whot		= NULL;
struct tr_Window *worder	= NULL;

struct tr_Object *cmain		= NULL;
struct tr_Object *cnealko	= NULL;
struct tr_Object *chot		= NULL;
struct tr_Object *corder	= NULL;

struct tr_Object *bcustom	= NULL;
struct tr_Object *bitem		= NULL;
struct tr_Object *back		= NULL;
struct tr_Object *image		= NULL;
struct tr_Object *baccept	= NULL;
struct tr_Object *breject	= NULL;

struct tr_Attr	 customAttr;
struct tr_Attr	 backAttr;
struct tr_Attr	 mainAttr;
struct tr_Attr	 menuAttr;
struct tr_Attr	 sectionAttr;
struct tr_Attr	 acceptAttr;
struct tr_Attr	 rejectAttr;

typedef struct menuItem {
  unsigned char name[256];
  unsigned char price[256];
  struct menuItem *next;
} menuItem;

typedef struct menuSection {
  struct menuItem *items;
} menuSection;

void *onNealkoDown(void*data)
{
  tr_WindowExpose(wnealko);
}

void *onNealkoUp(void*data)
{
  tr_WindowToTop(wnealko);
  tr_WindowExpose(wnealko);
}

void *onHotDown(void*data)
{
  tr_WindowExpose(whot);
}

void *onHotUp(void*data)
{
  tr_WindowToTop(whot);
  tr_WindowExpose(whot);
}

void *onHome(void*data)
{
  tr_WindowToTop(wmain);
  tr_WindowExpose(wmain);
}

void *onNone(void*data)
{
}

void *onAccept(void*data)
{
  strcpy((char*)order.value,"objednavka, detail\n");
  order.table = 0;
  mikaPosClientCommandAddOrder(&client,&order);
  tr_WindowToTop(wmain);
  tr_WindowExpose(wmain);
}

void *onReject(void*data)
{
  tr_WindowToTop(wmain);
  tr_WindowExpose(wmain);
}

void *onOrder(void*data)
{
  strcpy((char*)order.value,"objednavka, detail\n");
  tr_WindowToTop(worder);
  tr_WindowExpose(worder);
}

int main( )
{
  osInit();
  trop_Init();
  gui_Init();
  tr_WmInit();
  
  layout = NULL;
  rotation = ROT_NONE;
  
  mainAttr.x = 0;
  mainAttr.y = 0;
  mainAttr.w = 440;
  mainAttr.h = 440;
  
  menuAttr.x = 10;
  menuAttr.y = 90;
  menuAttr.w = 420;
  menuAttr.h = 350;
  
  root = tr_WindowInit(NULL,root,&mainAttr);
  wmain = tr_WindowInit(NULL,root,&mainAttr);
  wnealko = tr_WindowInit(NULL,root,&menuAttr);
  whot = tr_WindowInit(NULL,root,&menuAttr);
  worder = tr_WindowInit(NULL,root,&menuAttr);
  
  tr_WindowMap(wmain);
  tr_WindowToTop(wmain);
  
  cmain = tr_CanvasInit(NULL,&mainAttr,wmain);
  tr_SetRound(cmain,3);
  tr_SetColor(cmain,(char*)"#000000");
  form_CanvasSetMov(cmain->widget.sub,CAN_MOV_NONE);
  
  customAttr.x = 5;
  customAttr.y = 5;
  customAttr.w = 120;
  customAttr.h = 55;
  bcustom = tr_ButtonInitEx2(NULL,&customAttr,cmain);
  tr_SetRound(bcustom,2);
  tr_SetColor(bcustom,(char*)"#000000");
  tr_CanvasAddObj(cmain,bcustom);
  image = tr_ImageInitEx(NULL,&customAttr,(char*)"olympicImages/logo.jpg");
  tr_ImageSetRound(image,2);
  tr_AddImage(bcustom,image);
  tr_ImagePosCenter(image,bcustom);
  
  customAttr.x = 20;
  customAttr.y = 90;
  customAttr.w = 120;
  customAttr.h = 120;
  
  bcustom = tr_ButtonInitEx2(NULL,&customAttr,cmain);
  tr_SetRound(bcustom,3);
  tr_SetColor(bcustom,(char*)"#dfdfdf");
  tr_SetAction(bcustom,onNealkoUp,TR_ACTION_BUTTON);
  tr_SetAction(bcustom,onNealkoDown,TR_ACTION_BUTTONDOWN);
  image = tr_ImageInitEx(NULL,&customAttr,(char*)"olympicImages/nealko.jpg");
  tr_AddImage(bcustom,image);
  tr_ImagePosCenter(image,bcustom);
  tr_CanvasAddObjStatic(cmain,bcustom);
  
  customAttr.x = 160;
  
  bcustom = tr_ButtonInitEx2(NULL,&customAttr,cmain);
  tr_SetRound(bcustom,3);
  tr_SetColor(bcustom,(char*)"#dfdfdf");
  tr_SetAction(bcustom,onHotUp,TR_ACTION_BUTTON);
  tr_SetAction(bcustom,onHotDown,TR_ACTION_BUTTONDOWN);
  image = tr_ImageInitEx(NULL,&customAttr,(char*)"olympicImages/hot.jpg");
  tr_AddImage(bcustom,image);
  tr_ImagePosCenter(image,bcustom);
  tr_CanvasAddObjStatic(cmain,bcustom);
  
  tr_WindowToTop(wnealko);
  cnealko = tr_CanvasInit(NULL,&menuAttr,wnealko);
  tr_SetRound(cnealko,2);
  tr_SetColor(cnealko,(char*)"#101010");
  form_CanvasSetMov(cnealko->widget.sub,CAN_MOV_SCROLL);
  
  backAttr.w = 60;
  backAttr.h = 60;
  backAttr.x = menuAttr.w-70;
  backAttr.y = menuAttr.h-70;
  back = tr_ButtonInitEx2(NULL,&backAttr,cnealko);
  tr_SetRound(back,2);
  tr_SetColor(back,(char*)"#ffffff");
  tr_SetAction(back,onHome,TR_ACTION_BUTTON);
  tr_CanvasAddObjStatic(cnealko,back);
  backAttr.x = 0;
  backAttr.y = 0;
  image = tr_ImageInitEx(NULL,&backAttr,(char*)"img/home_small_new2.jpg");
  tr_AddImage(back,image);
  tr_ImagePosCenter(image,back);
  
  int n;
  unsigned char *text;
  for(n = 0; n < 12; n++)
  {
    customAttr.x = 0;
    customAttr.y = 0;
    customAttr.w = 320;
    customAttr.h = 70;
    bitem = tr_ButtonInitEx2(NULL,&customAttr,cnealko);
    tr_SetAction(bitem,onOrder,TR_ACTION_BUTTON);
    tr_SetAction(bitem,onNone,TR_ACTION_BUTTONDOWN);
    tr_SetRound(bitem,2);
    tr_SetColor(bitem,(char*)"#000000");
    text = (unsigned char*)malloc(64);
    if (n==0)		strcpy((char*)text,(char*)"1 Coca-Cola");   //     \xE2\x82\xAC");
    else if (n==1)	strcpy((char*)text,(char*)"2 Coca-Cola Zero");//   \xE2\x82\xAC");
    else if (n==2)	strcpy((char*)text,(char*)"3 Fanta, Sprite");//    2,00 \xE2\x82\xAC");
    else if (n==3)	strcpy((char*)text,(char*)"4 Kinley Tonic");//     2,00 \xE2\x82\xAC");
    else if (n==4)	strcpy((char*)text,(char*)"5 Dzus Happy Day");//   2,00 \xE2\x82\xAC");
    else if (n==5)	strcpy((char*)text,(char*)"6 Rauch");//            2,00 \xE2\x82\xAC");
    else if (n==6)	strcpy((char*)text,(char*)"7 Red Bull");//         2,00 \xE2\x82\xAC");
    else if (n==7)	strcpy((char*)text,(char*)"8 Bonaqua");//          1,70 \xE2\x82\xAC");
    else if (n==8)	strcpy((char*)text,(char*)"9 Mineralna voda");//   1,70 \xE2\x82\xAC");
    else if (n==9)	strcpy((char*)text,(char*)"10 Vinea");//            2,00 \xE2\x82\xAC");
    else if (n==10)	strcpy((char*)text,(char*)"11 Ice tea");//          2,00 \xE2\x82\xAC");
    else if (n==11)	strcpy((char*)text,(char*)"12 Evian");//            2,40 \xE2\x82\xAC");
    tr_SetTextEx(bitem,(char*)text,PLACE_CENTER);
    tr_SetFontColor(bitem,0xcf,0xcf,0xcf,0xff);
    tr_CanvasAddObj(cnealko,bitem);
  }
  
  tr_WindowToTop(whot);
  chot = tr_CanvasInit(NULL,&menuAttr,whot);
  tr_SetRound(chot,2);
  tr_SetColor(chot,(char*)"#101010");
  form_CanvasSetMov(chot->widget.sub,CAN_MOV_SCROLL);
  
  backAttr.w = 60;
  backAttr.h = 60;
  backAttr.x = menuAttr.w-70;
  backAttr.y = menuAttr.h-70;
  back = tr_ButtonInitEx2(NULL,&backAttr,chot);
  tr_SetRound(back,2);
  tr_SetColor(back,(char*)"#ffffff");
  tr_SetAction(back,onHome,TR_ACTION_BUTTON);
  tr_CanvasAddObjStatic(chot,back);
  backAttr.x = 0;
  backAttr.y = 0;
  image = tr_ImageInitEx(NULL,&backAttr,(char*)"img/home_small_new2.jpg");
  tr_AddImage(back,image);
  tr_ImagePosCenter(image,back);
  
  for(n = 0; n < 6; n++)
  {
    customAttr.x = 0;
    customAttr.y = 0;
    customAttr.w = 320;
    customAttr.h = 70;
    bitem = tr_ButtonInitEx2(NULL,&customAttr,chot);
    tr_SetAction(bitem,onOrder,TR_ACTION_BUTTON);
    tr_SetAction(bitem,onNone,TR_ACTION_BUTTONDOWN);
    tr_SetRound(bitem,2);
    tr_SetColor(bitem,(char*)"#000000");
    text = (unsigned char*)malloc(64);
    if (n==0)		strcpy((char*)text,(char*)"1 Presso káva");   //     \xE2\x82\xAC");
    else if (n==1)	strcpy((char*)text,(char*)"2 Melange");//   \xE2\x82\xAC");
    else if (n==2)	strcpy((char*)text,(char*)"3 Cappuchino");//    2,00 \xE2\x82\xAC");
    else if (n==3)	strcpy((char*)text,(char*)"4 Viedenska káva");//     2,00 \xE2\x82\xAC");
    else if (n==4)	strcpy((char*)text,(char*)"5 Latte");//   2,00 \xE2\x82\xAC");
    else if (n==5)	strcpy((char*)text,(char*)"6 Caj");//            2,00 \xE2\x82\xAC");
    else if (n==6)	strcpy((char*)text,(char*)"7 Cokolada black, white");//         2,00 \xE2\x82\xAC");
    tr_SetTextEx(bitem,(char*)text,PLACE_CENTER);
    tr_SetFontColor(bitem,0xcf,0xcf,0xcf,0xff);
    tr_CanvasAddObj(chot,bitem);
  }
  
  tr_WindowToTop(worder);
  corder = tr_CanvasInit(NULL,&menuAttr,worder);
  tr_SetRound(corder,3);
  tr_SetColor(corder,(char*)"#303030");
  form_CanvasSetMov(corder->widget.sub,CAN_MOV_NONE);
  backAttr.w = 60;
  backAttr.h = 60;
  backAttr.x = menuAttr.w-70;
  backAttr.y = menuAttr.h-70;
  back = tr_ButtonInitEx2(NULL,&backAttr,corder);
  tr_SetRound(back,2);
  tr_SetColor(back,(char*)"#ffffff");
  tr_SetAction(back,onHome,TR_ACTION_BUTTON);
  tr_CanvasAddObjStatic(corder,back);
  backAttr.x = 0;
  backAttr.y = 0;
  image = tr_ImageInitEx(NULL,&backAttr,(char*)"img/home_small_new2.jpg");
  tr_AddImage(back,image);
  tr_ImagePosCenter(image,back);
  
  acceptAttr.w = 130;
  acceptAttr.h = 100;
  acceptAttr.x = 20;
  acceptAttr.y = 150;
  baccept = tr_ButtonInitEx2(NULL,&acceptAttr,corder);
  tr_SetRound(baccept,2);
  tr_SetColor(baccept,(char*)"#aaffaa");
  tr_SetAction(baccept,onAccept,TR_ACTION_BUTTON);
  text = (unsigned char*)malloc(64);
  strcpy((char*)text,"Objednat");
  tr_SetTextEx(baccept,(char*)text,PLACE_CENTER);
  tr_SetFontColor(baccept,0x00,0xff,0x00,0xff);
  tr_CanvasAddObjStatic(corder,baccept);
  
  rejectAttr.w = 130;
  rejectAttr.h = 100;
  rejectAttr.x = 240;
  rejectAttr.y = 150;
  breject = tr_ButtonInitEx2(NULL,&rejectAttr,corder);
  tr_SetRound(breject,2);
  tr_SetColor(breject,(char*)"#ffaaaa");
  tr_SetAction(breject,onReject,TR_ACTION_BUTTON);
  text = (unsigned char*)malloc(64);
  strcpy((char*)text,"Zamietnut");
  tr_SetTextEx(breject,(char*)text,PLACE_CENTER);
  tr_SetFontColor(breject,0xff,0x00,0x00,0xff);
  tr_CanvasAddObjStatic(corder,breject);
  
  tr_WindowToTop(wmain);
  tr_WindowQueue(root);
  
  return 0;
}
