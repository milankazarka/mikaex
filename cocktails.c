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
  #include "xml.h"
  #include "wm.h"
  #include "dom.h"
  #include "mikaRss.h"
}

#include "mikaConfig.h"

#define _COCKTAIL_INFO_NO
#define _LOCAL_DEBUG

struct tr_Window *root 		= NULL;
struct tr_Window *wmain 	= NULL;

struct tr_Object *cmain 	= NULL;
struct tr_Object *cchampagne 	= NULL;

struct tr_Attr	mainAttr;
struct tr_Attr	buttonAttr;
struct tr_Attr	imageAttr;
struct tr_Attr	cocktailAttr;
struct tr_Attr	infoAttr;
struct tr_Attr	prevAttr;
struct tr_Attr	nextAttr;

struct tr_Object	*bPoster;
struct tr_Object	*bimage;
struct tr_Object	*binfo;
struct tr_Object	*bprev;
struct tr_Object	*bnext;
char			*text;

int item = 0;

pthread_mutex_t		mutex;

void setCocktail()
{
  switch(item)
  {
    case 0:
      bimage = tr_ImageInitEx(NULL,&imageAttr,(char*)"cocktailImages/cocktailChampagne.jpg");
      break;
    case 1:
      bimage = tr_ImageInitEx(NULL,&imageAttr,(char*)"cocktailImages/cocktailMargarita.jpg");
      break;
    case 2:
      bimage = tr_ImageInitEx(NULL,&imageAttr,(char*)"cocktailImages/cocktailMojito.jpg");
      break;
    case 3:
      bimage = tr_ImageInitEx(NULL,&imageAttr,(char*)"cocktailImages/cocktailPunch.jpg");
      break;
  }
  tr_ImageSetRound(bimage,2);
  tr_AddImage(bPoster,bimage);
  tr_ImagePosCenter(bimage,bPoster);
  tr_WindowExpose(wmain);
}

void *onPrevDown(void*data)
{
  pthread_mutex_lock(&mutex);
  tr_SetColor(bprev,(char*)"#404040");
  tr_SetFontColor(bprev,0xff,0xff,0xff,0xff);
  tr_WindowExpose(wmain);
  #ifdef _LOCAL_DEBUG
    printf("onPrevDown\n");
  #endif
  pthread_mutex_unlock(&mutex);
}

void *onNextDown(void*data)
{
  pthread_mutex_lock(&mutex);
  tr_SetColor(bnext,(char*)"#404040");
  tr_SetFontColor(bnext,0xff,0xff,0xff,0xff);
  tr_WindowExpose(wmain);
  #ifdef _LOCAL_DEBUG
    printf("onNextDown\n");
  #endif
  pthread_mutex_unlock(&mutex);
}

void *onPrev(void*data)
{
  pthread_mutex_lock(&mutex);
  tr_SetColor(bprev,(char*)"#ffffff");
  tr_SetFontColor(bprev,0x00,0x00,0x00,0xff);
  if (item!=0)
    item--;
  else
    item = 3;
  setCocktail();
  pthread_mutex_unlock(&mutex);
}

void *onNext(void*data)
{
  pthread_mutex_lock(&mutex);
  tr_SetColor(bnext,(char*)"#ffffff");
  tr_SetFontColor(bnext,0x00,0x00,0x00,0xff);
  if (item==3)
    item=0;
  else
    item++;
  setCocktail();
  pthread_mutex_unlock(&mutex);
}

int main( )
{
  pthread_mutex_init(&mutex,NULL);
  interfaceSetName((unsigned char*)"COCKTAILS");

  osInit();
  trop_Init();
  gui_Init();
  tr_WmInit();
  
  mikaConfigGet((unsigned char*)"menu");
  
  layout = NULL;
  rotation = ROT_NONE;
  
  mainAttr.x = 0;
  mainAttr.y = 0;
  mainAttr.w = 360;
  mainAttr.h = 360;

  cocktailAttr.x = 0;
  cocktailAttr.y = 0;
  cocktailAttr.w = 340;
  cocktailAttr.h = 240;

  infoAttr.x = 0;
  infoAttr.y = 200;
  infoAttr.w = 340;
  infoAttr.h = 50;

  prevAttr.x = 5;
  prevAttr.y = 200;
  prevAttr.w = 100;
  prevAttr.h = 80;

  nextAttr.x = 230;
  nextAttr.y = 200;
  nextAttr.w = 100;
  nextAttr.h = 80;

  buttonAttr.x = 0;
  buttonAttr.y = 0;
  buttonAttr.w = 340;
  buttonAttr.h = 340;

  imageAttr.x = 0;
  imageAttr.y = 0;
  imageAttr.w = 340;
  imageAttr.h = 200;
  
  root = tr_WindowInit(NULL,root,&mainAttr);
  wmain = tr_WindowInit(NULL,root,&mainAttr);
  
  tr_WindowMap(wmain);
  tr_WindowToTop(wmain);
  
  cmain = tr_CanvasInit(NULL,&mainAttr,wmain);
  tr_SetRound(cmain,3);
  tr_SetColor(cmain,(char*)"#cecece");
  form_CanvasSetMov(cmain->widget.sub,CAN_MOV_NONE);
  
  bPoster = tr_ButtonInitEx2(NULL,&imageAttr,cmain);
  tr_SetRound(bPoster,4);
  tr_SetColor(bPoster,(char*)"#dfdfdf");
  tr_CanvasAddObj(cmain,bPoster);
  bimage = tr_ImageInitEx(NULL,&imageAttr,(char*)"cocktailImages/cocktailChampagne.jpg");
  tr_ImageSetRound(bimage,2);
  tr_AddImage(bPoster,bimage);
  tr_ImagePosCenter(bimage,bPoster);
  
  binfo = tr_ButtonInitEx2(NULL,&infoAttr,cmain);
  tr_SetRound(binfo,4);
  tr_SetColor(binfo,(char*)"#dedede");
  text = (char*)malloc(64);
  strcpy((char*)text,"Cocktail Information");
  tr_SetTextEx(binfo,text,PLACE_CENTER);
  #ifdef _COCKTAIL_INFO
    tr_CanvasAddObj(cmain,binfo);
  #endif

  bprev = tr_ButtonInitEx2(NULL,&prevAttr,cmain);
  tr_SetRound(bprev,4);
  tr_SetColor(bprev,(char*)"#ffffff");
  text = (char*)malloc(64);
  strcpy((char*)text,"Previous");
  tr_SetTextEx(bprev,text,PLACE_CENTER);
  tr_CanvasAddObj(cmain,bprev);
  tr_SetAction(bprev,onPrev,TR_ACTION_BUTTON);
  tr_SetAction(bprev,onPrevDown,TR_ACTION_BUTTONDOWN);
  tr_SetFontColor(bprev,0x30,0x30,0x30,220);
  
  bnext = tr_ButtonInitEx2(NULL,&nextAttr,cmain);
  tr_SetRound(bnext,4);
  tr_SetColor(bnext,(char*)"#ffffff");
  text = (char*)malloc(64);
  strcpy((char*)text,"Next");
  tr_SetTextEx(bnext,text,PLACE_CENTER);
  tr_CanvasAddObj(cmain,bnext);
  tr_SetAction(bnext,onNext,TR_ACTION_BUTTON);
  tr_SetAction(bnext,onNextDown,TR_ACTION_BUTTONDOWN);
  tr_SetFontColor(bnext,0x30,0x30,0x30,220);
  
  tr_WindowToTop(wmain);
  tr_WindowQueue(root);
  
  return 0;
}
