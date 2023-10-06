#include "shop.h"
extern "C" {
  #include "interface.h"
  #include "common.h"
  #include "config.h"
  #include "os.h"
  #include "system.h"
  #include "systemip.h"
  #include "xml.h"
  #include "wm.h"
  #include "dom.h"
  #include "activityObject.h"
  #include "rootControllerSync.h"
  #include "basic.h"
  #include "mikaPosClient.h"
  #include "global.h"
}
#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"
#include "mikaPanel.h"

#define _APP_SIZE_W 700
#define _APP_SIZE_H 500

struct tr_Window	*root		= NULL;
struct tr_Window	*wmain		= NULL;
struct tr_Window	*wdetail	= NULL;
struct tr_Window	*witems		= NULL;
struct tr_Window	*wstore		= NULL;
struct tr_Window	*wspecials	= NULL;

struct tr_Object	*cmain		= NULL;
struct tr_Object	*cdetail	= NULL;
struct tr_Object	*citems		= NULL;
struct tr_Object	*cstore		= NULL;
struct tr_Object	*cspecials	= NULL;

struct tr_Object	*imageLogo	= NULL;
struct tr_Object	*blogo		= NULL;

struct tr_Attr		mainAttr;
struct tr_Attr		blogoAttr;
struct tr_Attr		attr;

mikaPanel		*panel		= NULL;
mikaPanelItem		*itemStore	= NULL;
mikaPanelItem		*itemItems	= NULL;

class appCanvasItemDetails {
  public:
    struct tr_Attr	detailsAttr;
    struct tr_Object	*bprice;
    struct tr_Object	*bname;
    
    appCanvasItemDetails( struct tr_Object *canvas, struct tr_Object *widget );
    ~appCanvasItemDetails( );
    
    int setPrice( unsigned char *price );
    int setName( unsigned char *name );
};

class appCanvasDetail {
  public:
    struct tr_Attr	previewAttr;
    struct tr_Attr	detailAttr;
    struct tr_Object	*bpreview;
    struct tr_Object	*bpreviewImage;  
    struct tr_Object	*bdetail;  
    
    appCanvasItemDetails *appDetails;
      
    appCanvasDetail( struct tr_Window *window, struct tr_Object *canvas );
    ~appCanvasDetail( );
    
    int setImage( unsigned char *filename );
};

class appCanvasItems {
  public:
    struct tr_Attr	itemsAttr;
    struct tr_Object	*bwomanImage;
    struct tr_Object	*bmanImage;
    struct tr_Object	*bwoman;
    struct tr_Object	*bman;
  
    appCanvasItems( struct tr_Window *window, struct tr_Object *canvas );
    ~appCanvasItems( );
};

class appCanvasStore {
  public:
    struct tr_Attr	storeAttr;
    struct tr_Object	*bposterImage;
    struct tr_Object	*bposter;
    
    appCanvasStore( struct tr_Window *window, struct tr_Object *canvas );
    ~appCanvasStore( );
};

appCanvasDetail 	*appCDetail	= NULL;
appCanvasItems		*appCItems	= NULL;
appCanvasStore		*appCStore	= NULL;
//

void *onButton( void *data )
{
  static int run = 0;
  if (run==0)
  {
    tr_WindowToTop(wdetail);
    tr_WindowExpose(wdetail);
    run++;
  }
  else
  {
    tr_WindowToTop(witems);
    tr_WindowExpose(witems);
    run--;
  }
}

void *onButtonDetails( void *data )
{
  static int run = 1;
  #ifdef _DEBUG
    printf("onButtonDetails\n");
  #endif
  
  if (run==5)
  {
    run = 1;
  }
  else
  {
    run++;
  }
  
  unsigned char tmp[128];
  sprintf((char*)tmp,"FilesShop/womanGeneral00%d.jpg",run);
  appCDetail->setImage((unsigned char*)tmp);
  
  unsigned char *txt = (unsigned char*)malloc(128);
  sprintf((char*)txt,"%d00.0 euro",run);
  appCDetail->appDetails->setPrice((unsigned char*)txt);
  txt = (unsigned char*)malloc(128);
  sprintf((char*)txt,"dress %d",run);
  appCDetail->appDetails->setName((unsigned char*)txt);
  
  tr_WindowToTop(wdetail);
  tr_WindowExpose(wdetail);
  return NULL;
}

void *onButtonItems( void *data )
{
  #ifdef _DEBUG
    printf("onButtonItems\n");
  #endif
  tr_WindowToTop(witems);
  tr_WindowExpose(witems);
  return NULL;
}

void *onButtonStore( void *data )
{
  #ifdef _DEBUG
    printf("onButtonStore\n");
  #endif
  tr_WindowToTop(wstore);
  tr_WindowExpose(wstore);
  return NULL;
}

void *onButtonSpecials( void *data )
{
  #ifdef _DEBUG
    printf("onButtonSpecials\n");
  #endif
  tr_WindowToTop(wspecials);
  tr_WindowExpose(wspecials);
  return NULL;
}

appCanvasItemDetails::appCanvasItemDetails( struct tr_Object *canvas, struct tr_Object *widget )
{
  detailsAttr.x = 0;
  detailsAttr.y = 0;
  detailsAttr.w = 200;
  detailsAttr.h = 40;
  bprice = tr_ButtonInitEx2(NULL,&detailsAttr,canvas);
  tr_PutObject(widget,bprice);
  tr_SetFontColor(bprice,0x30,0x30,0x30,240);
  tr_SetFontSize(bprice,20);  
  detailsAttr.y = 40;
  detailsAttr.h = 40;
  bname = tr_ButtonInitEx2(NULL,&detailsAttr,canvas);
  tr_SetFontColor(bname,0x30,0x30,0x30,240);
  tr_SetFontSize(bname,20);
  tr_PutObject(widget,bname);
}

appCanvasItemDetails::~appCanvasItemDetails( )
{
}

int appCanvasItemDetails::setPrice( unsigned char *price )
{
  if (!price)
  {
    return 1;
  }
  
  char *txt = (char*)malloc(strlen((char*)price)+1);
  strcpy((char*)txt,(char*)price);
  tr_ChangeTextEx(bprice,(char*)txt,PLACE_CENTER);
  
  return 0;
}

int appCanvasItemDetails::setName( unsigned char *name )
{
  if (!name)
  {
    return 1;
  }
  
  char *txt = (char*)malloc(strlen((char*)name)+1);
  strcpy((char*)txt,(char*)name);
  tr_ChangeTextEx(bname,(char*)txt,PLACE_CENTER);
  
  return 0;
}

appCanvasDetail::appCanvasDetail( struct tr_Window *window, struct tr_Object *canvas )
{
  #ifdef _DEBUG
    printf("appCanvasDetail::appCanvasDetail\n");
  #endif
  previewAttr.x = 100;
  previewAttr.y = 100;
  previewAttr.w = 300;
  previewAttr.h = 300;
  bpreview = tr_ButtonInitEx2(NULL,&previewAttr,canvas);
  tr_SetColor(bpreview,(char*)"#303030");
  tr_CanvasAddObjStatic(canvas,bpreview);
  previewAttr.x = 0;
  previewAttr.y = 0;
  bpreviewImage = tr_ImageInitEx(NULL,&attr,(char*)"FilesShop/womanGeneral001.jpg");
  tr_AddImage(bpreview,bpreviewImage);
  tr_ImagePosCenter(bpreviewImage,bpreview);
  
  detailAttr.x = 400;
  detailAttr.y = 100;
  detailAttr.w = 200;
  detailAttr.h = 300;
  bdetail = tr_ButtonInitEx2(NULL,&detailAttr,canvas);
  tr_SetColor(bdetail,(char*)"#ffffff");
  tr_CanvasAddObjStatic(canvas,bdetail);
  
  appDetails = new appCanvasItemDetails(canvas,bdetail);
  appDetails->setPrice((unsigned char*)"price 100.0");
  appDetails->setName((unsigned char*)"dress 1");
}

appCanvasDetail::~appCanvasDetail( )
{
}

int appCanvasDetail::setImage( unsigned char *filename )
{
  if (!filename)
    return 1;
      
  bpreviewImage = tr_ImageInitEx(NULL,&attr,(char*)filename);
  tr_AddImage(bpreview,bpreviewImage);
  tr_ImagePosCenter(bpreviewImage,bpreview);
  
  return 0;
}

appCanvasItems::appCanvasItems( struct tr_Window *window, struct tr_Object *canvas )
{
  itemsAttr.x = 100;
  itemsAttr.y = 100;
  itemsAttr.w = 150;
  itemsAttr.h = 300;
  
  bwoman = tr_ButtonInitEx2(NULL,&itemsAttr,canvas);
  tr_CanvasAddObjStatic(canvas,bwoman);
  itemsAttr.x = 0;
  itemsAttr.y = 0;
  bwomanImage = tr_ImageInitEx(NULL,&itemsAttr,(char*)"FilesShop/iconWoman.jpg");
  tr_AddImage(bwoman,bwomanImage);
  tr_ImagePosCenter(bwomanImage,bwoman);
  char *txt = (char*)malloc(32);
  strcpy((char*)txt,"woman");
  tr_SetFontColor(bwoman,0x30,0x30,0x30,240);
  tr_SetFontSize(bwoman,20);
  tr_ChangeTextExPos(bwoman,(char*)txt,0,260);
  
  itemsAttr.x = 300;
  itemsAttr.y = 100;
  itemsAttr.w = 150;
  itemsAttr.h = 300;
  bman = tr_ButtonInitEx2(NULL,&itemsAttr,canvas);
  tr_CanvasAddObjStatic(canvas,bman);
  itemsAttr.x = 0;
  itemsAttr.y = 0;
  bmanImage = tr_ImageInitEx(NULL,&itemsAttr,(char*)"FilesShop/iconMan.jpg");
  tr_AddImage(bman,bmanImage);
  tr_ImagePosCenter(bmanImage,bman);
  txt = (char*)malloc(32);
  strcpy((char*)txt,"man");
  tr_SetFontColor(bman,0x30,0x30,0x30,240);
  tr_SetFontSize(bman,20);
  tr_ChangeTextExPos(bman,(char*)txt,0,260);
}

appCanvasItems::~appCanvasItems( )
{
}

appCanvasStore::appCanvasStore( struct tr_Window *window, struct tr_Object *canvas )
{
  storeAttr.x = 100;
  storeAttr.y = 100;
  storeAttr.w = 400;
  storeAttr.h = 300;
  
  bposter = tr_ButtonInitEx2(NULL,&storeAttr,canvas);
  tr_CanvasAddObjStatic(canvas,bposter);
  storeAttr.x = 0;
  storeAttr.y = 0;
  bposterImage = tr_ImageInitEx(NULL,&storeAttr,(char*)"FilesShop/poster.jpg");
  tr_AddImage(bposter,bposterImage);
  tr_ImagePosCenter(bposterImage,bposter);
}

appCanvasStore::~appCanvasStore( )
{
}

mikaPanel *generatePanelForCanvas( struct tr_Object *canvas )
{
  mikaPanel *customPanel 		= NULL;
  mikaPanelItem *customItemStore	= NULL;
  mikaPanelItem *customItemItems	= NULL;
  mikaPanelItem *customItemSpecials	= NULL;
  mikaPanelItem *customItemScanned	= NULL;
  
  if (!canvas)
  {
    #ifdef _DEBUG
      printf("generatePanelForCanvas error, invalid arguments\n");
    #endif
    return NULL;
  }
  
  attr.x = 100;
  attr.y = 420;
  attr.w = 510;
  attr.h = 80;
  customPanel = new mikaPanel(canvas,&attr);
  
  attr.x = 5;
  attr.y = 5;
  attr.w = 100;
  attr.h = 70;
  customItemScanned = new mikaPanelItem(canvas,&attr);
  customItemScanned->setName((unsigned char*)"scanned");
  tr_SetAction(customItemScanned->widget,onButtonDetails,TR_ACTION_BUTTON);
  customPanel->addItem(customItemScanned);
  attr.x += 132;
  customItemItems = new mikaPanelItem(canvas,&attr);
  customItemItems->setName((unsigned char*)"items");
  tr_SetAction(customItemItems->widget,onButtonItems,TR_ACTION_BUTTON);
  customPanel->addItem(customItemItems);
  attr.x += 132;
  customItemSpecials = new mikaPanelItem(cmain,&attr);
  customItemSpecials->setName((unsigned char*)"specials");
  tr_SetAction(customItemSpecials->widget,onButtonSpecials,TR_ACTION_BUTTON);
  customPanel->addItem(customItemSpecials);
  attr.x += 132;
  customItemStore = new mikaPanelItem(cmain,&attr);
  customItemStore->setName((unsigned char*)"store info");
  tr_SetAction(customItemStore->widget,onButtonStore,TR_ACTION_BUTTON);
  customPanel->addItem(customItemStore);
  
  return customPanel;
}

struct tr_Object *setupCanvas( struct tr_Window *window )
{
  if (!window)
  {
    #ifdef _DEBUG
      printf("setupCanvas error, invalid arguments\n");
    #endif
    return NULL;
  }
  
  struct tr_Object *canvas = tr_CanvasInit(NULL,&mainAttr,window);

  form_CanvasSetMov(canvas->widget.sub,CAN_MOV_NONE);
  tr_SetRound(canvas,2);
  tr_SetColor(canvas,(char*)"#ffffff");

  attr.x = 5;
  attr.y = 5;
  attr.w = 200;
  attr.h = 50;
  blogo = tr_ButtonInitEx2(NULL,&attr,canvas);
  tr_SetColor(blogo,(char*)"#ffffff");
  tr_CanvasAddObjStatic(canvas,blogo);
  attr.x = 0;
  attr.y = 0;
  imageLogo = tr_ImageInitEx(NULL,&attr,(char*)"FilesShop/topshopLogoSmall.jpg");
  tr_AddImage(blogo,imageLogo);
  tr_ImagePosCenter(imageLogo,blogo);

  return canvas;
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
  mainAttr.w = _APP_SIZE_W;
  mainAttr.h = _APP_SIZE_H;
  
  root		= tr_WindowInit(NULL,root,&mainAttr);
  wmain		= tr_WindowInit(NULL,root,&mainAttr);
  wdetail	= tr_WindowInit(NULL,root,&mainAttr);
  witems	= tr_WindowInit(NULL,root,&mainAttr);
  wspecials	= tr_WindowInit(NULL,root,&mainAttr);
  wstore	= tr_WindowInit(NULL,root,&mainAttr);
  
  tr_WindowMap(wmain);
  tr_WindowToTop(wmain);
  cmain = setupCanvas(wmain);
  generatePanelForCanvas(cmain);
  
  tr_WindowMap(wdetail);
  tr_WindowToTop(wdetail);
  cdetail = setupCanvas(wdetail);
  generatePanelForCanvas(cdetail);
  appCDetail = new appCanvasDetail(wdetail,cdetail);
  
  tr_WindowMap(witems);
  tr_WindowToTop(witems);
  citems = setupCanvas(witems);
  generatePanelForCanvas(citems);
  appCItems = new appCanvasItems(witems,citems);
  
  tr_WindowMap(wspecials);
  tr_WindowToTop(wspecials);
  cspecials = setupCanvas(wspecials);
  generatePanelForCanvas(cspecials);
  
  tr_WindowMap(wstore);
  tr_WindowToTop(wstore);
  cstore = setupCanvas(wstore);
  generatePanelForCanvas(cstore);
  appCStore = new appCanvasStore(wstore,cstore);
  
  interfaceSetBackplaneSize(0,0);
  activitySetMovable(&bigInterface->activity,0,0);
  
  tr_WindowToTop(wdetail);
  tr_WindowQueue(root);
  
  return 0;
}
