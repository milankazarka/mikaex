extern "C" {
  #include "interface.h"
  #include "common.h"
  #include "config.h"
  #include "os.h"
  #include "system.h"
  #include "global.h"
  #include "xml.h"
  #include "wm.h"
  #include "dom.h"
  #include "activityObject.h"
}
#include "museum.h"

struct tr_Window *root		= NULL;
struct tr_Window *wmain		= NULL;

struct tr_Object *cmain		= NULL;
struct tr_Object *button	= NULL;
struct tr_Object *buttonPrev	= NULL;
struct tr_Object *buttonNext	= NULL;
struct tr_Object *buttonHome	= NULL;
struct tr_Object *poster	= NULL;
struct tr_Object *slideshow	= NULL;
struct tr_Attr			mainAttr;
struct tr_Attr			buttonAttr;
struct tr_Attr			posterAttr;
struct tr_Attr			homeAttr;
struct tr_Attr			slideshowAttr;

typedef struct mImage
{
  unsigned char name[256];
  struct mImage *next;
} mImage;

struct mImage *images = NULL;

void *onPrev(void*data)
{
  #ifdef _DEBUG
    printf("onPrev\n");
  #endif
}

void *onNext(void*data)
{
  #ifdef _DEBUG
    printf("onNext\n");
  #endif
}

void *onHome(void*data)
{
  #ifdef _DEBUG
    printf("onHome\n");
  #endif
}

struct mImage *initImage( )
{
  struct mImage *image = NULL;
  image = (struct mImage*)malloc(sizeof(struct mImage));
  image->next = images;
  images = image;
}

int initImages( )
{
  return 0;
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
  mainAttr.w = 800;
  mainAttr.h = 600;

  root = tr_WindowInit(NULL,root,&mainAttr);
  wmain = tr_WindowInit(NULL,root,&mainAttr);

  tr_WindowMap(wmain);
  activitySetMovable(&bigInterface->activity,0,1);

  tr_WindowToTop(wmain);
  cmain = tr_CanvasInit(NULL,&mainAttr,wmain);
  tr_SetRound(cmain,3);
  tr_SetColor(cmain,(char*)"#f0f0f0");
  
  buttonAttr.x = mainAttr.w*0.05;
  buttonAttr.w = mainAttr.w*0.25;
  buttonAttr.h = mainAttr.h*0.15;
  buttonAttr.y = mainAttr.h*0.80;
  buttonPrev = tr_ButtonInitEx2(NULL,&buttonAttr,cmain);
  tr_SetColor(buttonPrev,(char*)"#101010");
  tr_SetRound(buttonPrev,3);
  tr_CanvasAddObjStatic(cmain,buttonPrev);
  tr_SetAction(buttonPrev,onPrev,TR_ACTION_BUTTON);
  tr_SetFontSize(buttonPrev,26);
  tr_SetFontColor(buttonPrev,0xff,0xff,0xff,240);
  tr_ChangeTextExPos(buttonPrev,(char*)"Previous",20,buttonAttr.h/2);
  
  buttonAttr.x = mainAttr.w*0.70;
  buttonNext = tr_ButtonInitEx2(NULL,&buttonAttr,cmain);
  tr_SetColor(buttonNext,(char*)"#101010");
  tr_SetRound(buttonNext,3);
  tr_CanvasAddObjStatic(cmain,buttonNext);
  tr_SetAction(buttonNext,onNext,TR_ACTION_BUTTON);
  tr_SetFontSize(buttonNext,26);
  tr_SetFontColor(buttonNext,0xff,0xff,0xff,240);
  tr_ChangeTextExPos(buttonNext,(char*)"Next",20,buttonAttr.h/2);
  
  homeAttr.h = buttonAttr.h;
  homeAttr.w = buttonAttr.w;
  homeAttr.y = buttonAttr.y;
  homeAttr.x = (mainAttr.w/2)-(homeAttr.w/2);
  buttonHome = tr_ButtonInitEx2(NULL,&homeAttr,cmain);
  tr_SetColor(buttonHome,(char*)"#101010");
  tr_SetRound(buttonHome,3);
  tr_CanvasAddObjStatic(cmain,buttonHome);
  tr_SetAction(buttonHome,onHome,TR_ACTION_BUTTON);
  tr_SetFontSize(buttonHome,26);
  tr_SetFontColor(buttonHome,0xff,0xff,0xff,240);
  tr_ChangeTextExPos(buttonHome,(char*)"Home",20,buttonAttr.h/2);
  
  posterAttr.x = mainAttr.w*0.05;
  posterAttr.y = mainAttr.h*0.05;
  posterAttr.w = mainAttr.w*0.9;
  posterAttr.h = mainAttr.h*0.6;
  poster = tr_ButtonInitEx2(NULL,&posterAttr,cmain);
  tr_SetColor(poster,(char*)"#303030");
  tr_SetRound(poster,2);
  tr_CanvasAddObjStatic(cmain,poster);
  
  slideshowAttr.x = mainAttr.w*0.05;
  slideshowAttr.y = mainAttr.h*0.65;
  slideshowAttr.w = mainAttr.w*0.9;
  slideshowAttr.h = mainAttr.h*0.1;
  slideshow = tr_ButtonInitEx2(NULL,&slideshowAttr,cmain);
  tr_SetColor(slideshow,(char*)"#505050");
  tr_SetRound(slideshow,2);
  tr_CanvasAddObjStatic(cmain,slideshow);
  
  tr_WindowToTop(wmain);
  tr_WindowQueue(root);

  return 0;
}
