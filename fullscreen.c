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
}

#define _CONNECTED_MO

struct tr_Window *root		= NULL;
struct tr_Window *wmain		= NULL;
struct tr_Object *cmain		= NULL;
struct tr_Object *bposter	= NULL;
struct tr_Object *bclose	= NULL;
struct tr_Object *bimage	= NULL;

struct tr_Attr	mainAttr;
struct tr_Attr	closeAttr;

#ifndef _ACTIVITY_OBJECT_RELAY_CLOCK
  #define _ACTIVITY_OBJECT_RELAY_CLOCK	250000
#endif
void *_activity_object_relay(void*data)
{
  static int run = 0;
  struct activityObject oFullscreen;
  actObjects = activityObjectsClone();
  while(1)
  {
    if ( activityObjectClone(actObjects,(unsigned char*)"fullscreenApp",&oFullscreen) == 0 )
    {
      if (oFullscreen.fastData[0]!=0x00)
      {
        #ifdef _DEBUG
          printf("fullscreenApp load(%s)\n",(char*)oFullscreen.fastData);
        #endif
        
        struct activityObject oAngle;
        if ( activityObjectClone(actObjects,(unsigned char*)"anglesushi",&oAngle) == 0 )
        {
          int angle;
          if (oAngle.fastData[0]!=0x00)
            angle = atoi((char*)oAngle.fastData);
          activitySetForceangle(&bigInterface->activity,angle,1);
        }
        
        binarySemaphoreWait(bigInterface->activity.sem->id);
        if (run==0)
        {
          bimage = tr_ImageInitEx(NULL,&mainAttr,(char*)"flower01.jpg");
          tr_AddImage(bposter,bimage);
          tr_ImagePosCenter(bimage,bposter);
          tr_SetColor(cmain,(char*)"#808080");
          run++;
        }
        else
        {
          bimage = tr_ImageInitEx(NULL,&mainAttr,(char*)"flower02.jpg");
          tr_AddImage(bposter,bimage);
          tr_ImagePosCenter(bimage,bposter);
          tr_SetColor(cmain,(char*)"#afafaf");
          run--;
        }
        
        activitySetVisible(&bigInterface->activity,1,1);
        binarySemaphorePost(bigInterface->activity.sem->id);
        
        tr_WindowExpose(wmain);
        usleep(350000);
        
        oFullscreen.fastData[0] = 0x00;
        activityObjectSet(actObjects,(unsigned char*)"fullscreenApp",&oFullscreen);
      }
    }
    else
    {
      #ifdef _DEBUG
        printf("fullscreenApp could not find activityObject(fullscreenApp)\n");
      #endif
    }
    usleep(_ACTIVITY_OBJECT_RELAY_CLOCK);
  }
}

void *onCloseDown(void*data)
{
  return 0; // 
  
  #ifdef _DEBUG
    printf("onCloseDown\n");
  #endif
  tr_SetColor(bposter,(char*)"#e0e0e0");
  tr_SetFontColor(bclose,0x20,0x20,0x20,0xff);
  tr_WindowExpose(wmain);
}

void *_on_closeup_thread(void*data)
{
  #ifdef _DEBUG
    printf("fullscreen _on_closeup_thread\n");
  #endif
  struct activityObject oSushi;
  activitySetVisible(&bigInterface->activity,0,1);
  tr_WindowExpose(wmain);
  
  usleep(300000);
  
  if ( activityObjectClone(actObjects,(unsigned char*)"sushi",&oSushi) == 0 )
  {
    oSushi.fastData[0] = 1;
    activityObjectSet(actObjects,(unsigned char*)"sushi",&oSushi);
  }
}

void *onCloseUp(void*data)
{
  static struct activityObject oSushi;

  #ifdef _DEBUG
    printf("onCloseUp\n");
  #endif
  
  #ifndef _CONNECTED
    pthread_t t;
    pthread_create(&t,NULL,_on_closeup_thread,NULL);
  #endif
  
  tr_SetColor(bclose,(char*)"trans");
  tr_SetFontColor(bclose,0xff,0xff,0xff,0xff);
  
  activitySetVisible(&bigInterface->activity,0,1);
  if ( activityObjectClone(actObjects,(unsigned char*)"sushi",&oSushi) == 0 )
  {
    oSushi.fastData[0] = 1;
    activityObjectSet(actObjects,(unsigned char*)"sushi",&oSushi);
  }
  
  tr_WindowExpose(wmain);
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
  mainAttr.w = 600;
  mainAttr.h = 600;
  
  closeAttr.x = 20;
  closeAttr.y = 20;
  closeAttr.w = 80;
  closeAttr.h = 80;
  
  root = tr_WindowInit(NULL,root,&mainAttr);
  wmain = tr_WindowInit(NULL,root,&mainAttr);
  
  tr_WindowMap(wmain);
  tr_WindowToTop(wmain);
  
  cmain = tr_CanvasInit(NULL,&mainAttr,wmain);
  tr_SetRound(cmain,3);
  tr_SetColor(cmain,(char*)"#808080");
  form_CanvasSetMov(cmain->widget.sub,CAN_MOV_NONE);
  
  actObjects = activityObjectsClone();
  if (actObjects)
  {
    if ( activityObjectRegister( actObjects,(unsigned char*)"fullscreenApp" ) != 0 )
    {
      #ifdef _DEBUG
        printf("activity object could not be created named(fullscreenApp)\n");
      #endif
    }
    else
    {
      //activitySetMovable(&bigInterface->activity,0,1);
      //activitySetVisible(&bigInterface->activity,0,1);
    }
  }
  
  bposter = tr_ButtonInitEx2(NULL,&mainAttr,cmain);
  tr_SetColor(bposter,(char*)"#dedede");
  tr_SetRound(bposter,3);
  tr_SetAction(bposter,onCloseDown,TR_ACTION_BUTTONDOWN);
  tr_SetAction(bposter,onCloseUp,TR_ACTION_BUTTON);
  tr_CanvasAddObjStatic(cmain,bposter);
  
  bclose = tr_ButtonInitEx2(NULL,&closeAttr,cmain);
  tr_SetColor(bclose,(char*)"trans");
  tr_SetRound(bclose,7);
  tr_SetAction(bclose,onCloseDown,TR_ACTION_BUTTONDOWN);
  tr_SetAction(bclose,onCloseUp,TR_ACTION_BUTTON);
  char *text;
  text = (char*)malloc(32);
  strcpy(text,"Hide");
  tr_SetTextEx(bclose,text,PLACE_CENTER);
  tr_SetFontColor(bclose,0xff,0xff,0xff,0xff);
  //tr_CanvasAddObjStatic(cmain,bclose);
  
  #ifndef _CONNECTED
    pthread_t t;
    pthread_create(&t,NULL,_activity_object_relay,NULL);
  #endif
  
  tr_WindowToTop(wmain);
  tr_WindowQueue(root);
  
  return 0;
}
