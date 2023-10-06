extern "C" {
  #include "interface.h"
  #include "os.h"
  #include "system.h"
  #include "global.h"
  #include "wm.h"
  #include "mikaPosClient.h"
}

#ifndef _DEBUG
  #define _DEBUG
#endif

struct tr_Window 	*root 	= NULL;
struct tr_Window 	*wmain 	= NULL;
struct tr_Window	*wref 	= NULL;
struct tr_Window	*wtech	= NULL;

struct tr_Object 	*cmain 	= NULL;
struct tr_Object	*cref 	= NULL;
struct tr_Object	*ctech	= NULL;

struct tr_Object	*bmisc 	= NULL;
struct tr_Object	*bimage = NULL;

struct tr_Attr		mainAttr;
struct tr_Attr		miscAttr;

char			*text = NULL;

void *onButtonHome(void*data)
{
  tr_WindowToTop(wmain);
  tr_WindowExpose(wmain);
  return NULL;
}

void *onServices(void*data)
{
  tr_WindowToTop(wref);
  tr_WindowExpose(wref);
  return NULL;
}

void *onTechnologies(void*data)
{
  tr_WindowToTop(wtech);
  tr_WindowExpose(wtech);
  return NULL;
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
  mainAttr.w = 360;
  mainAttr.h = 360;

  root = tr_WindowInit(NULL,root,&mainAttr);
  wmain = tr_WindowInit(NULL,root,&mainAttr);
  wref = tr_WindowInit(NULL,root,&mainAttr);
  wtech = tr_WindowInit(NULL,root,&mainAttr);

  tr_WindowMap(wmain);

  tr_WindowToTop(wmain);
  cmain = tr_CanvasInit(NULL,&mainAttr,wmain);
  tr_SetRound(cmain,3);
  tr_SetColor(cmain,(char*)"#dfdfdf");
  form_CanvasSetMov(cmain->widget.sub,CAN_MOV_NONE);

  miscAttr.x = 0;
  miscAttr.y = 0;
  miscAttr.w = 140;
  miscAttr.h = 70;

  bmisc = tr_ButtonInitEx2(NULL,&miscAttr,cmain);
  tr_SetRound(bmisc,2);
  tr_SetColor(bmisc,(char*)"#ffffff");
  tr_CanvasAddObj(cmain,bmisc);
  bimage = tr_ImageInitEx(NULL,&miscAttr,(char*)"img/cngroup.jpg");
  tr_ImageSetRound(bimage,2);
  tr_AddImage(bmisc,bimage);
  tr_ImagePosCenter(bimage,bmisc);

  miscAttr.x = 10;
  miscAttr.y = 10;
  miscAttr.w = 300;
  miscAttr.h = 80;

  bmisc = tr_ButtonInitEx2(NULL,&miscAttr,cmain);
  tr_SetRound(bmisc,3);
  tr_SetColor(bmisc,(char*)"#de3232");
  text = (char*)malloc(12);
  sprintf((char*)text,"References");
  tr_SetTextEx(bmisc,text,PLACE_CENTER);
  tr_SetFontColor(bmisc,0x00,0x00,0x00,220);
  tr_CanvasAddObj(cmain,bmisc);
  tr_SetAction(bmisc,onServices,TR_ACTION_BUTTON);

  bmisc = tr_ButtonInitEx2(NULL,&miscAttr,cmain);
  tr_SetRound(bmisc,3);
  tr_SetColor(bmisc,(char*)"#de3232");
  text = (char*)malloc(17);
  sprintf((char*)text,"Technologies");
  tr_SetTextEx(bmisc,text,PLACE_CENTER);
  tr_SetFontColor(bmisc,0x00,0x00,0x00,220);
  tr_CanvasAddObj(cmain,bmisc);
  tr_SetAction(bmisc,onTechnologies,TR_ACTION_BUTTON);

  tr_WindowToTop(wref);
  
  cref = tr_CanvasInit(NULL,&mainAttr,wref);
  tr_SetRound(cref,3);
  tr_SetColor(cref,(char*)"#dfdfdf");
  form_CanvasSetMov(cref->widget.sub,CAN_MOV_SCROLL);

  miscAttr.x = 10;
  miscAttr.y = 10;
  miscAttr.w = 245;
  miscAttr.h = 166;

  bmisc = tr_ButtonInitEx2(NULL,&miscAttr,cref);
  tr_SetRound(bmisc,3);
  tr_SetColor(bmisc,(char*)"#ffffff");
  bimage = tr_ImageInitEx(NULL,&miscAttr,(char*)"img/cnRefAviation.jpg");
  tr_AddImage(bmisc,bimage);
  tr_CanvasAddObj(cref,bmisc);

  miscAttr.h = 110;
  bmisc = tr_ButtonInitEx2(NULL,&miscAttr,cref);
  tr_SetRound(bmisc,3);
  tr_SetColor(bmisc,(char*)"#ffffff");
  bimage = tr_ImageInitEx(NULL,&miscAttr,(char*)"img/cnRefEmbedded.jpg");
  tr_AddImage(bmisc,bimage);
  tr_CanvasAddObj(cref,bmisc);

  miscAttr.h = 205;
  bmisc = tr_ButtonInitEx2(NULL,&miscAttr,cref);
  tr_SetRound(bmisc,3);
  tr_SetColor(bmisc,(char*)"#ffffff");
  bimage = tr_ImageInitEx(NULL,&miscAttr,(char*)"img/cnRefTelecom.jpg");
  tr_AddImage(bmisc,bimage);
  tr_CanvasAddObj(cref,bmisc);

  miscAttr.h = 170;
  bmisc = tr_ButtonInitEx2(NULL,&miscAttr,cref);
  tr_SetRound(bmisc,3);
  tr_SetColor(bmisc,(char*)"#ffffff");
  bimage = tr_ImageInitEx(NULL,&miscAttr,(char*)"img/cnRefFinance.jpg");
  tr_AddImage(bmisc,bimage);
  tr_CanvasAddObj(cref,bmisc);

  miscAttr.h = 164;
  bmisc = tr_ButtonInitEx2(NULL,&miscAttr,cref);
  tr_SetRound(bmisc,3);
  tr_SetColor(bmisc,(char*)"#ffffff");
  bimage = tr_ImageInitEx(NULL,&miscAttr,(char*)"img/cnRefRetail.jpg");
  tr_AddImage(bmisc,bimage);
  tr_CanvasAddObj(cref,bmisc);

  miscAttr.x = mainAttr.w-70;
  miscAttr.y = mainAttr.h-70;
  miscAttr.w = 60;
  miscAttr.h = 60;

  bmisc = tr_ButtonInitEx2(NULL,&miscAttr,cref);
  tr_SetRound(bmisc,3);
  tr_SetColor(bmisc,(char*)"#de3232");
  tr_CanvasAddObjStatic(cref,bmisc);
  text = (char*)malloc(12);
  sprintf((char*)text,"Home");
  tr_SetTextEx(bmisc,text,PLACE_CENTER);
  tr_SetFontColor(bmisc,0x00,0x00,0x00,220);
  tr_SetAction(bmisc,onButtonHome,TR_ACTION_BUTTON);

  tr_WindowToTop(wtech);
  ctech = tr_CanvasInit(NULL,&mainAttr,wtech);
  tr_SetRound(ctech,3);
  tr_SetColor(ctech,(char*)"#dfdfdf");
  form_CanvasSetMov(ctech->widget.sub,CAN_MOV_SCROLL);

  miscAttr.x = mainAttr.w-70;
  miscAttr.y = mainAttr.h-70;
  miscAttr.w = 60;
  miscAttr.h = 60;

  bmisc = tr_ButtonInitEx2(NULL,&miscAttr,ctech);
  tr_SetRound(bmisc,3);
  tr_SetColor(bmisc,(char*)"#de3232");
  tr_CanvasAddObjStatic(ctech,bmisc);
  text = (char*)malloc(12);
  sprintf((char*)text,"Home");
  tr_SetTextEx(bmisc,text,PLACE_CENTER);
  tr_SetFontColor(bmisc,0x00,0x00,0x00,220);
  tr_SetAction(bmisc,onButtonHome,TR_ACTION_BUTTON);

  miscAttr.x = 10;
  miscAttr.y = 10;
  miscAttr.w = 245;
  miscAttr.h = 122;

  bmisc = tr_ButtonInitEx2(NULL,&miscAttr,ctech);
  tr_SetRound(bmisc,3);
  tr_SetColor(bmisc,(char*)"#ffffff");
  bimage = tr_ImageInitEx(NULL,&miscAttr,(char*)"img/cnTechLanguages.jpg");
  tr_AddImage(bmisc,bimage);
  tr_CanvasAddObj(ctech,bmisc);

  miscAttr.h = 88;
  bmisc = tr_ButtonInitEx2(NULL,&miscAttr,ctech);
  tr_SetRound(bmisc,3);
  tr_SetColor(bmisc,(char*)"#ffffff");
  bimage = tr_ImageInitEx(NULL,&miscAttr,(char*)"img/cnTechOs.jpg");
  tr_AddImage(bmisc,bimage);
  tr_CanvasAddObj(ctech,bmisc);

  miscAttr.h = 126;
  bmisc = tr_ButtonInitEx2(NULL,&miscAttr,ctech);
  tr_SetRound(bmisc,3);
  tr_SetColor(bmisc,(char*)"#ffffff");
  bimage = tr_ImageInitEx(NULL,&miscAttr,(char*)"img/cnTechTech.jpg");
  tr_AddImage(bmisc,bimage);
  tr_CanvasAddObj(ctech,bmisc);

  tr_WindowToTop(wmain);
  tr_WindowQueue(root);

  return 0;
}
