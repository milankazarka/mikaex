/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

extern "C" {
  #include "interface.h"
  #include "os.h"
  #include "system.h"
  #include "global.h"
  #include "wm.h"
}

#ifndef _DEBUG
  #define _DEBUG
#endif

struct tr_Window	*root	= NULL;
struct tr_Window	*wmain	= NULL;
struct tr_Window	*wabout	= NULL;
struct tr_Window	*wplatform = NULL;

struct tr_Object	*cmain	= NULL;
struct tr_Object	*cabout	= NULL;
struct tr_Object	*cplatform = NULL;

struct tr_Object	*bmisc	= NULL;
struct tr_Object	*bimage	= NULL;

struct tr_Attr		mainAttr;
struct tr_Attr		miscAttr;

char			*text	= NULL;

void *onButtonPlatform(void*data)
{
  tr_WindowToTop(wplatform);
  tr_WindowExpose(wplatform);
  return NULL;
}

void *onButtonAbout(void*data)
{
  tr_WindowToTop(wabout);
  tr_WindowExpose(wabout);
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
  wabout = tr_WindowInit(NULL,root,&mainAttr);
  wplatform = tr_WindowInit(NULL,root,&mainAttr);
  
  tr_WindowMap(wmain);
  tr_WindowToTop(wmain);
  
  cmain = tr_CanvasInit(NULL,&mainAttr,wmain);
  tr_SetRound(cmain,3);
  tr_SetColor(cmain,(char*)"#505050");
  form_CanvasSetMov(cmain->widget.sub,CAN_MOV_NONE);
  
  miscAttr.x = 10;
  miscAttr.y = 10;
  miscAttr.w = 340;
  miscAttr.h = 90;
  
  bmisc = tr_ButtonInitEx2(NULL,&miscAttr,cmain);
  tr_SetRound(bmisc,3);
  tr_SetColor(bmisc,(char*)"#ffffff");
  bimage = tr_ImageInitEx(NULL,&miscAttr,(char*)"img/mikaCnTitle.jpg");
  tr_AddImage(bmisc,bimage);
  tr_CanvasAddObj(cmain,bmisc);
  
  miscAttr.h = 70;
  bmisc = tr_ButtonInitEx2(NULL,&miscAttr,cmain);
  tr_SetRound(bmisc,3);
  tr_SetColor(bmisc,(char*)"#de3232");
  text = (char*)malloc(20);
  sprintf((char*)text,"Platform");
  tr_SetTextEx(bmisc,text,PLACE_CENTER);
  tr_SetFontColor(bmisc,0x00,0x00,0x00,220);
  tr_CanvasAddObj(cmain,bmisc);
  tr_SetAction(bmisc,onButtonPlatform,TR_ACTION_BUTTON);
  
  bmisc = tr_ButtonInitEx2(NULL,&miscAttr,cmain);
  tr_SetRound(bmisc,3);
  tr_SetColor(bmisc,(char*)"#de3232");
  text = (char*)malloc(20);
  sprintf((char*)text,"About");
  tr_SetTextEx(bmisc,text,PLACE_CENTER);
  tr_SetFontColor(bmisc,0x00,0x00,0x00,220);
  tr_CanvasAddObj(cmain,bmisc);
  tr_SetAction(bmisc,onButtonAbout,TR_ACTION_BUTTON);
  
  // platform canvas
  
  tr_WindowToTop(wplatform);
  
  cplatform = tr_CanvasInit(NULL,&mainAttr,wplatform);
  tr_SetRound(cplatform,3);
  tr_SetColor(cplatform,(char*)"#505050");
  form_CanvasSetMov(cplatform->widget.sub,CAN_MOV_SCROLL);
  
  miscAttr.x = mainAttr.w-70;
  miscAttr.y = mainAttr.h-70;
  miscAttr.w = 60;
  miscAttr.h = 60;
  
  bmisc = tr_ButtonInitEx2(NULL,&miscAttr,cplatform);
  tr_SetRound(bmisc,3);
  tr_SetColor(bmisc,(char*)"#f0f0f0");
  tr_CanvasAddObjStatic(cplatform,bmisc);
  text = (char*)malloc(12);
  sprintf((char*)text,"Home");
  tr_SetTextEx(bmisc,text,PLACE_CENTER);
  tr_SetFontColor(bmisc,0x00,0x00,0x00,220);
  tr_SetAction(bmisc,onButtonHome,TR_ACTION_BUTTON);
  
  miscAttr.x = 10;
  miscAttr.y = 10;
  miscAttr.w = 260;
  miscAttr.h = 132;
  bmisc = tr_ButtonInitEx2(NULL,&miscAttr,cplatform);
  tr_SetRound(bmisc,3);
  tr_SetColor(bmisc,(char*)"#f0f0f0");
  bimage = tr_ImageInitEx(NULL,&miscAttr,(char*)"img/schemaMikaOperatingSystemComplete.jpg");
  tr_AddImage(bmisc,bimage);
  tr_CanvasAddObj(cplatform,bmisc);
  
  miscAttr.x = 10;
  miscAttr.y = 10;
  miscAttr.w = 260;
  miscAttr.h = 180;
  bmisc = tr_ButtonInitEx2(NULL,&miscAttr,cplatform);
  tr_SetRound(bmisc,3);
  tr_SetColor(bmisc,(char*)"#f0f0f0");
  bimage = tr_ImageInitEx(NULL,&miscAttr,(char*)"img/schemaMikaFrameworkComplete.jpg");
  tr_AddImage(bmisc,bimage);
  tr_CanvasAddObj(cplatform,bmisc);
  
  // about canvas
  
  tr_WindowToTop(wabout);
  
  cabout = tr_CanvasInit(NULL,&mainAttr,wabout);
  tr_SetRound(cabout,3);
  tr_SetColor(cabout,(char*)"#505050");
  form_CanvasSetMov(cabout->widget.sub,CAN_MOV_SCROLL);
  
  miscAttr.x = mainAttr.w-70;
  miscAttr.y = mainAttr.h-70;
  miscAttr.w = 60;
  miscAttr.h = 60;
  
  bmisc = tr_ButtonInitEx2(NULL,&miscAttr,cabout);
  tr_SetRound(bmisc,3);
  tr_SetColor(bmisc,(char*)"#f0f0f0");
  tr_CanvasAddObjStatic(cabout,bmisc);
  text = (char*)malloc(12);
  sprintf((char*)text,"Home");
  tr_SetTextEx(bmisc,text,PLACE_CENTER);
  tr_SetFontColor(bmisc,0x00,0x00,0x00,220);
  tr_SetAction(bmisc,onButtonHome,TR_ACTION_BUTTON);
  
  miscAttr.x = 10;
  miscAttr.y = 10;
  miscAttr.w = 270;
  miscAttr.h = 280;
  bmisc = tr_ButtonInitEx2(NULL,&miscAttr,cabout);
  tr_SetRound(bmisc,3);
  tr_SetColor(bmisc,(char*)"#202020");
  bimage = tr_ImageInitEx(NULL,&miscAttr,(char*)"img/mikaCnAbout.jpg");
  tr_AddImage(bmisc,bimage);
  tr_CanvasAddObj(cabout,bmisc);
  
  tr_WindowToTop(wmain);
  tr_WindowQueue(root);
  
  return 0;
}
