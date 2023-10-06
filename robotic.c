extern "C" {
  #include "interface.h"
  #include "os.h"
  #include "system.h"
  #include "global.h"
  #include "wm.h"
  #include "mikaPosClient.h"
}
#include "mikaRoboticClient.h"

#ifndef _DEBUG
  #define _DEBUG
#endif

struct mikaRoboticClient *client = NULL;

struct tr_Window *root = NULL;
struct tr_Window *wmain = NULL;
struct tr_Object *cmain = NULL;

struct tr_Object *blogo = NULL;
struct tr_Object *blogoImage = NULL;
struct tr_Object *bcontrol = NULL;

struct tr_Object *bcommand1 = NULL;
struct tr_Object *bcommand2 = NULL;
struct tr_Object *bcommand3 = NULL;
struct tr_Object *bcommand4 = NULL;

void setInactive( )
{
  tr_SetColor(bcommand1,(char*)"#707070");
  tr_SetColor(bcommand2,(char*)"#707070");
  tr_SetColor(bcommand3,(char*)"#707070");
  tr_SetColor(bcommand4,(char*)"#707070");
  tr_SetFontColor(bcommand1,0xff,0xff,0xff,220);
  tr_SetFontColor(bcommand2,0xff,0xff,0xff,220);
  tr_SetFontColor(bcommand3,0xff,0xff,0xff,220);
  tr_SetFontColor(bcommand4,0xff,0xff,0xff,220);
}

void *oncommand1(void*data)
{
  #ifdef _DEBUG
    printf("oncommand1\n");
  #endif
  
  mikaRoboticClientSendCommand(client,(unsigned char*)"project_select:2O 3P");
  usleep(250000);
  mikaRoboticClientSendCommand(client,(unsigned char*)"sequence_list:play");
  
  setInactive();
  tr_SetColor(bcommand1,(char*)"#70df70");
  tr_SetFontColor(bcommand1,0x00,0x00,0x00,220);
  tr_WindowToTop(wmain);
  tr_WindowExpose(wmain);
  return NULL;
}

void *oncommand2(void*data)
{
  #ifdef _DEBUG
    printf("oncommand2\n");
  #endif
  
  mikaRoboticClientSendCommand(client,(unsigned char*)"project_select:2O 3P");
  usleep(250000);
  mikaRoboticClientSendCommand(client,(unsigned char*)"stop");
  
  setInactive();
  tr_SetColor(bcommand2,(char*)"#70df70");
  tr_SetFontColor(bcommand2,0x00,0x00,0x00,220);
  tr_WindowToTop(wmain);
  tr_WindowExpose(wmain);
  return NULL;
}

void *oncommand3(void*data)
{
  #ifdef _DEBUG
    printf("oncommand3\n");
  #endif
  
  mikaRoboticClientSendCommand(client,(unsigned char*)"project_select:2O 3P");
  usleep(250000);
  mikaRoboticClientSendCommand(client,(unsigned char*)"pause");
  
  setInactive();
  tr_SetColor(bcommand3,(char*)"#70df70");
  tr_SetFontColor(bcommand3,0x00,0x00,0x00,220);
  tr_WindowToTop(wmain);
  tr_WindowExpose(wmain);
  return NULL;
}

void *oncommand4(void*data)
{
  #ifdef _DEBUG
    printf("oncommand4\n");
  #endif
  
  mikaRoboticClientSendCommand(client,(unsigned char*)"project_select:2O 3P");
  usleep(250000);
  mikaRoboticClientSendCommand(client,(unsigned char*)"resume");
  
  setInactive();
  tr_SetColor(bcommand4,(char*)"#70df70");
  tr_SetFontColor(bcommand4,0x00,0x00,0x00,220);
  tr_WindowToTop(wmain);
  tr_WindowExpose(wmain);
  return NULL;
}

int main( )
{
  struct tr_Attr	mainAttr;
  struct tr_Attr	logoAttr;
  struct tr_Attr	controlAttr;
  struct tr_Attr	command1Attr;
  struct tr_Attr	command2Attr;
  struct tr_Attr	command3Attr;
  struct tr_Attr	command4Attr;
  char			*text = NULL;

  interfaceSetName((unsigned char*)"ROBOTIC");
  
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
  
  logoAttr.x = 0;
  logoAttr.y = 0;
  logoAttr.w = 140;
  logoAttr.h = 60;
  
  controlAttr.x = 10;
  controlAttr.y = 100;
  controlAttr.w = 340;
  controlAttr.h = 220;

  command1Attr.x = 20;
  command1Attr.y = 100;
  command1Attr.w = 155;
  command1Attr.h = 100;
  
  command2Attr.x = 185;
  command2Attr.y = 100;
  command2Attr.w = 155;
  command2Attr.h = 100;

  command3Attr.x = 20;
  command3Attr.y = 210;
  command3Attr.w = 155;
  command3Attr.h = 100;

  command4Attr.x = 185;
  command4Attr.y = 210;
  command4Attr.w = 155;
  command4Attr.h = 100;
  
  root = tr_WindowInit(NULL,root,&mainAttr);
  wmain = tr_WindowInit(NULL,root,&mainAttr);
  
  tr_WindowMap(wmain);
  tr_WindowToTop(wmain);
  
  cmain = tr_CanvasInit(NULL,&mainAttr,wmain);
  tr_SetRound(cmain,2);
  tr_SetColor(cmain,(char*)"#ffffff");
  form_CanvasSetMov(cmain->widget.sub,CAN_MOV_NONE);
  
  blogo = tr_ButtonInitEx2(NULL,&logoAttr,cmain);
  tr_SetRound(blogo,2);
  tr_SetColor(blogo,(char*)"#ffffff");
  tr_CanvasAddObj(cmain,blogo);
  blogoImage = tr_ImageInitEx(NULL,&logoAttr,(char*)"img/cngroup.jpg");
  tr_ImageSetRound(blogoImage,2);
  tr_AddImage(blogo,blogoImage);
  tr_ImagePosCenter(blogoImage,blogo);
  
  bcontrol = tr_ButtonInitEx2(NULL,&controlAttr,cmain);
  tr_SetRound(bcontrol,2);
  tr_SetColor(bcontrol,(char*)"#afafaf");
  //tr_CanvasAddObjStatic(cmain,bcontrol);
  
  bcommand1 = tr_ButtonInitEx2(NULL,&command1Attr,cmain);
  tr_SetRound(bcommand1,2);
  tr_SetColor(bcommand1,(char*)"#707070");
  tr_CanvasAddObjStatic(cmain,bcommand1);
  text = (char*)malloc(64);
  strcpy(text,"Start");
  tr_SetTextEx(bcommand1,text,PLACE_CENTER);
  tr_SetAction(bcommand1,oncommand1,TR_ACTION_BUTTON);
  
  bcommand2 = tr_ButtonInitEx2(NULL,&command2Attr,cmain);
  tr_SetRound(bcommand2,2);
  tr_SetColor(bcommand2,(char*)"#707070");
  tr_CanvasAddObjStatic(cmain,bcommand2);
  text = (char*)malloc(64);
  strcpy(text,"Stop");
  tr_SetTextEx(bcommand2,text,PLACE_CENTER);
  tr_SetAction(bcommand2,oncommand2,TR_ACTION_BUTTON);
  
  bcommand3 = tr_ButtonInitEx2(NULL,&command3Attr,cmain);
  tr_SetRound(bcommand3,2);
  tr_SetColor(bcommand3,(char*)"#707070");
  tr_CanvasAddObjStatic(cmain,bcommand3);
  text = (char*)malloc(64);
  strcpy(text,"Pause");
  tr_SetTextEx(bcommand3,text,PLACE_CENTER);
  tr_SetAction(bcommand3,oncommand3,TR_ACTION_BUTTON);
  
  bcommand4 = tr_ButtonInitEx2(NULL,&command4Attr,cmain);
  tr_SetRound(bcommand4,2);
  tr_SetColor(bcommand4,(char*)"#707070");
  tr_CanvasAddObjStatic(cmain,bcommand4);
  text = (char*)malloc(64);
  strcpy(text,"Resume");
  tr_SetTextEx(bcommand4,text,PLACE_CENTER);
  tr_SetAction(bcommand4,oncommand4,TR_ACTION_BUTTON);
  
  client = mikaRoboticClientNew();
  
  tr_WindowToTop(wmain);
  tr_WindowQueue(root);
  
  return 0;
}
