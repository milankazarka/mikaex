extern "C" {
  #include "interface.h"
  #include "os.h"
  #include "system.h"
  #include "global.h"
  #include "xml.h"
  #include "wm.h"
  #include "dom.h"
  #include "mikaRss.h"
}

struct tr_Window *root = NULL;
struct tr_Window *wmain = NULL;
struct tr_Window *wsport = NULL;
struct tr_Window *wpolitics = NULL;
struct tr_Window *wstars = NULL;

struct tr_Object *cmain = NULL;
struct tr_Object *csport = NULL;
struct tr_Object *cpolitics = NULL;
struct tr_Object *cstars = NULL;

void *onButtonSport(void*data)
{
  tr_WindowToTop(wsport);
  tr_WindowExpose(wsport);
  return NULL;
}

void *onButtonPolitics(void*data)
{
  tr_WindowToTop(wsport);
  tr_WindowExpose(wsport);
  return NULL;
}

void *onButtonStars(void*data)
{
  tr_WindowToTop(wsport);
  tr_WindowExpose(wsport);
  return NULL;
}

void *onButtonHome(void*data)
{
  tr_WindowToTop(wmain);
  tr_WindowExpose(wmain);
  return NULL;
}

int main( )
{
  struct tr_Attr	mainAttr;
  struct tr_Attr	buttonAttr;
  struct tr_Attr	imageAttr;
  struct tr_Attr	feedAttr;
  struct tr_Attr	attrHome;

  struct tr_Object	*bSport;
  struct tr_Object	*bPolitics;
  struct tr_Object	*bStars;
  struct tr_Object	*bimage;
  struct tr_Object	*bhome;
  struct tr_Object	*ihome;
  char			*text;

  struct tr_Object	*bSportFeed;

  struct tr_Object	*isport;
  struct tr_Object	*ipolitics;
  struct tr_Object	*istars;

  interfaceSetName((unsigned char*)"NEWS");

  osInit();
  trop_Init();
  gui_Init();
  tr_WmInit();
  
  layout = NULL;
  rotation = ROT_NONE;
  
  mainAttr.x = 0;
  mainAttr.y = 0;
  mainAttr.w = 360;
  mainAttr.h = 360;

  attrHome.x = mainAttr.w-70;
  attrHome.y = mainAttr.h-70;
  attrHome.w = 60;
  attrHome.h = 60;

  feedAttr.x = 0;
  feedAttr.y = 0;
  feedAttr.w = 260;
  feedAttr.h = 300;

  buttonAttr.x = 0;
  buttonAttr.y = 0;
  buttonAttr.w = 290;
  buttonAttr.h = 90;

  imageAttr.x = 0;
  imageAttr.y = 0;
  imageAttr.w = 260;
  imageAttr.h = 50;
  
  root = tr_WindowInit(NULL,root,&mainAttr);
  wmain = tr_WindowInit(NULL,root,&mainAttr);
  wsport = tr_WindowInit(NULL,root,&mainAttr);
  wpolitics = tr_WindowInit(NULL,root,&mainAttr);
  wstars = tr_WindowInit(NULL,root,&mainAttr);
  
  tr_WindowMap(wmain);
  
  tr_WindowToTop(wmain);
  cmain = tr_CanvasInit(NULL,&mainAttr,wmain);
  tr_SetRound(cmain,3);
  tr_SetColor(cmain,(char*)"#dfdfdf");
  form_CanvasSetMov(cmain->widget.sub,CAN_MOV_NONE);
  
  tr_WindowToTop(wsport);
  csport = tr_CanvasInit(NULL,&mainAttr,wsport);
  tr_SetRound(csport,3);
  tr_SetColor(csport,(char*)"#101010");
  form_CanvasSetMov(csport->widget.sub,CAN_MOV_SCROLL);
  
  tr_WindowToTop(wpolitics);
  cpolitics = tr_CanvasInit(NULL,&mainAttr,wpolitics);
  tr_SetRound(cpolitics,3);
  tr_SetColor(cpolitics,(char*)"#101010");
  form_CanvasSetMov(cpolitics->widget.sub,CAN_MOV_SCROLL);
  
  tr_WindowToTop(wstars);
  cstars = tr_CanvasInit(NULL,&mainAttr,wstars);
  tr_SetRound(cstars,3);
  tr_SetColor(cstars,(char*)"#101010");
  form_CanvasSetMov(cstars->widget.sub,CAN_MOV_SCROLL);
  
  /** create the buttons of the main canvas
   */
  bSport = tr_ButtonInitEx2(NULL,&buttonAttr,cmain);
  tr_SetRound(bSport,4);
  tr_SetColor(bSport,(char*)"#ffffff");
  tr_SetAction(bSport,onButtonSport,TR_ACTION_BUTTON);
  //tr_CanvasAddObj(cmain,bSport);
  text = (char*)malloc(32);
  sprintf((char*)text,"Sport");
  tr_SetTextEx(bSport,text,PLACE_CENTER);
  bimage = tr_ImageInitEx(NULL,&imageAttr,(char*)"img/bannerSportSmall.jpg");
  tr_ImageSetRound(bimage,2);
  tr_AddImage(bSport,bimage);
  tr_ImagePosCenter(bimage,bSport);
  
  bPolitics = tr_ButtonInitEx2(NULL,&buttonAttr,cmain);
  tr_SetRound(bPolitics,4);
  tr_SetColor(bPolitics,(char*)"#ffffff");
  tr_CanvasAddObj(cmain,bPolitics);
  tr_SetAction(bPolitics,onButtonSport,TR_ACTION_BUTTON);
  text = (char*)malloc(32);
  sprintf((char*)text,"Politics");
  tr_SetTextEx(bPolitics,text,PLACE_CENTER);
  bimage = tr_ImageInitEx(NULL,&imageAttr,(char*)"img/bannerPoliticsSmall.jpg");
  tr_ImageSetRound(bimage,2);
  tr_AddImage(bPolitics,bimage);
  tr_ImagePosCenter(bimage,bPolitics);
  
  bStars = tr_ButtonInitEx2(NULL,&buttonAttr,cmain);
  tr_SetRound(bStars,4);
  tr_SetColor(bStars,"#ffffff");
  //tr_CanvasAddObj(cmain,bStars);
  text = (char*)malloc(32);
  sprintf((char*)text,"Stars");
  tr_SetTextEx(bStars,text,PLACE_CENTER);
  bimage = tr_ImageInitEx(NULL,&imageAttr,(char*)"img/bannerStarsSmall.jpg");
  tr_ImageSetRound(bimage,2);
  tr_AddImage(bStars,bimage);
  tr_ImagePosCenter(bimage,bStars);
  
  tr_WindowToTop(wsport);
  bSportFeed = tr_ButtonInitEx2(NULL,&feedAttr,cmain);
  tr_SetColor(bSportFeed,"#ffffff");
  tr_CanvasAddObj(csport,bSportFeed);
  isport = tr_ImageInitEx(NULL,&imageAttr,(char*)"img/newsPoliticsSmall.jpg");
  tr_ImageSetRound(isport,2);
  tr_AddImage(bSportFeed,isport);
  tr_ImagePosCenter(isport,bSportFeed);
  ihome = tr_ImageInitEx(NULL,&attrHome,(char*)"img/home_small_new.jpg");
  tr_ImageSetRound(ihome,2);
  bhome = tr_ButtonInitEx2(NULL,&attrHome,csport);
  tr_SetRound(bhome,3);
  tr_SetColor(bhome,(char*)"#ffffff");
  tr_AddImage(bhome,ihome);
  tr_SetAction(bhome,onButtonHome,TR_ACTION_BUTTON);
  tr_ImagePosCenter(ihome,bhome);
  tr_CanvasAddObjStatic(csport,bhome);
  
  tr_WindowToTop(wmain);
  tr_WindowQueue(root);
  
  return 0;
}
