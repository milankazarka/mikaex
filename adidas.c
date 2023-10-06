extern "C" {
  #include "interface.h"
  #include "activity.h"
  #include "config.h"
  #include "os.h"
  #include "system.h"
  #include "global.h"
  #include "xml.h"
  #include "wm.h"
  #include "dom.h"
  #include "activityObject.h"
  #include "rootControllerSync.h"
  #include "networkSetup.h"
  #include "player.h"
  #include "xipc.h"
}

struct tr_Window *root		= NULL;
struct tr_Window *wmain		= NULL;
struct tr_Window *wshoes        = NULL;
struct tr_Window *wclothes      = NULL;
struct tr_Window *wbags         = NULL;

struct tr_Object *cmain		= NULL;
struct tr_Object *cshoes        = NULL;
struct tr_Object *cclothes      = NULL;
struct tr_Object *cbags         = NULL;

struct tr_Object *button        = NULL;
struct tr_Object *image         = NULL;

struct tr_Object *bshoes        = NULL;
struct tr_Object *bclothes      = NULL;
struct tr_Object *bbags         = NULL;

struct tr_Object *bimage        = NULL;

struct tr_Attr	 mainAttr;
struct tr_Attr	 attr;

void *onButton(void*data)
{
  #ifdef _DEBUG
    printf("adidas onButton\n");
  #endif
  
  struct gui_Event *event = (struct gui_Event*)data;
  struct gui_Widget *widget = event->widget;
  
  if (widget==bshoes->widget.sub)
  {
    #ifdef _DEBUG
      printf("adidas onButton bshoes\n");
    #endif
    tr_WindowToTop(wshoes);
    tr_WindowExpose(wshoes);
  }
  else if (widget==bclothes->widget.sub)
  {
    #ifdef _DEBUG
      printf("adidas onButton bclothes\n");
    #endif
    tr_WindowToTop(wclothes);
    tr_WindowExpose(wclothes);
  }
  else if (widget==bbags->widget.sub)
  {
    #ifdef _DEBUG
      printf("adidas onButton bbags\n");
    #endif
    tr_WindowToTop(wbags);
    tr_WindowExpose(wbags);
  }
  
  return NULL;
}

void *onBack(void*data)
{
  #ifdef _DEBUG
    printf("onBack\n");
  #endif
  tr_WindowToTop(wmain);
  tr_WindowExpose(wmain);
}

int main(void)
{
    interfaceSetName((unsigned char*)"ADIDAS");
  
    osInit();
    trop_Init();
    gui_Init();
    tr_WmInit();
  
    layout = NULL;
    rotation = ROT_NONE;
  
    mainAttr.x = 0;
    mainAttr.y = 0;
    mainAttr.w = 480;
    mainAttr.h = 480;
  
    root        = tr_WindowInit(NULL,root,&mainAttr);
    wmain       = tr_WindowInit(NULL,root,&mainAttr);
    wshoes      = tr_WindowInit(NULL,root,&mainAttr);
    wclothes    = tr_WindowInit(NULL,root,&mainAttr);
    wbags       = tr_WindowInit(NULL,root,&mainAttr);
  
    tr_WindowMap(wmain);
    tr_WindowToTop(wmain);
    
    cmain = tr_CanvasInit(NULL,&mainAttr,wmain);
    tr_SetColor(cmain,(char*)"#ffffff");
    
    /**
    attr.x = 10;
    attr.y = 10;
    attr.w = 150;
    attr.h = 50;
    button = tr_ButtonInitEx2(NULL,&attr,cmain);
    tr_SetColor(button,(char*)"#ffffff");
    tr_CanvasAddObjStatic(cmain,button);
    image = tr_ImageInitEx(NULL,&attr,(char*)"adidasFiles/logo.jpg");
    tr_AddImage(button,image);
    tr_ImagePosCenter(image,button);
    
    attr.x = 0;
    attr.y = 327;
    attr.w = 480;
    attr.h = 153;
    button = tr_ButtonInitEx2(NULL,&attr,cmain);
    tr_SetColor(button,(char*)"#ffffff");
    tr_CanvasAddObjStatic(cmain,button);
    image = tr_ImageInitEx(NULL,&attr,(char*)"adidasFiles/bottom.jpg");
    tr_AddImage(button,image);
    tr_ImagePosCenter(image,button);
    
    attr.x = 10;
    attr.y = 80;
    attr.w = 146;
    attr.h = 230;
    bshoes = tr_ButtonInitEx2(NULL,&attr,cmain);
    tr_SetAction(bshoes,onButton,TR_ACTION_BUTTON);
    tr_SetColor(bshoes,(char*)"#ffffff");
    tr_CanvasAddObjStatic(cmain,bshoes);
    bimage = tr_ImageInitEx(NULL,&attr,(char*)"adidasFiles/iconShoes.jpg");
    tr_AddImage(bshoes,bimage);
    tr_ImagePosCenter(bimage,bshoes);
    
    attr.x = 170;
    attr.y = 80;
    attr.w = 146;
    attr.h = 230;
    bclothes = tr_ButtonInitEx2(NULL,&attr,cmain);
    tr_SetAction(bclothes,onButton,TR_ACTION_BUTTON);
    tr_SetColor(bclothes,(char*)"#ffffff");
    tr_CanvasAddObjStatic(cmain,bclothes);
    bimage = tr_ImageInitEx(NULL,&attr,(char*)"adidasFiles/iconClothing.jpg");
    tr_AddImage(bclothes,bimage);
    tr_ImagePosCenter(bimage,bclothes);
    
    attr.x = 326;
    attr.y = 80;
    attr.w = 146;
    attr.h = 230;
    bbags = tr_ButtonInitEx2(NULL,&attr,cmain);
    tr_SetAction(bbags,onButton,TR_ACTION_BUTTON);
    tr_SetColor(bbags,(char*)"#ffffff");
    tr_CanvasAddObjStatic(cmain,bbags);
    bimage = tr_ImageInitEx(NULL,&attr,(char*)"adidasFiles/iconBags.jpg");
    tr_AddImage(bbags,bimage);
    tr_ImagePosCenter(bimage,bbags);
    
    tr_WindowToTop(wshoes);
    cshoes = tr_CanvasInit(NULL,&mainAttr,wshoes);
    tr_SetColor(cshoes,(char*)"#ffffff");
    attr.x = 10;
    attr.y = 10;
    attr.w = 150;
    attr.h = 50;
    button = tr_ButtonInitEx2(NULL,&attr,cshoes);
    tr_SetColor(button,(char*)"#ffffff");
    tr_SetAction(button,onBack,TR_ACTION_BUTTON);
    tr_CanvasAddObjStatic(cshoes,button);
    image = tr_ImageInitEx(NULL,&attr,(char*)"adidasFiles/logo.jpg");
    tr_AddImage(button,image);
    tr_ImagePosCenter(image,button);
    
    tr_WindowToTop(wclothes);
    cclothes = tr_CanvasInit(NULL,&mainAttr,wclothes);
    tr_SetColor(cclothes,(char*)"#ffffff");
    attr.x = 10;
    attr.y = 10;
    attr.w = 150;
    attr.h = 50;
    button = tr_ButtonInitEx2(NULL,&attr,cclothes);
    tr_SetColor(button,(char*)"#ffffff");
    tr_SetAction(button,onBack,TR_ACTION_BUTTON);
    tr_CanvasAddObjStatic(cclothes,button);
    image = tr_ImageInitEx(NULL,&attr,(char*)"adidasFiles/logo.jpg");
    tr_AddImage(button,image);
    tr_ImagePosCenter(image,button);
    
    tr_WindowToTop(wbags);
    cbags = tr_CanvasInit(NULL,&mainAttr,wbags);
    tr_SetColor(cbags,(char*)"#ffffff");
    attr.x = 10;
    attr.y = 10;
    attr.w = 150;
    attr.h = 50;
    button = tr_ButtonInitEx2(NULL,&attr,cbags);
    tr_SetColor(button,(char*)"#ffffff");
    tr_SetAction(button,onBack,TR_ACTION_BUTTON);
    tr_CanvasAddObjStatic(cbags,button);
    image = tr_ImageInitEx(NULL,&attr,(char*)"adidasFiles/logo.jpg");
    tr_AddImage(button,image);
    tr_ImagePosCenter(image,button);
    */
    tr_WindowToTop(wmain);
    
    interfaceSetBackplaneSize(0,0);
    
    tr_WindowQueue(root);
    
    return 0;
}
