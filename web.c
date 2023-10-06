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
#include "setupParser.h"

#define _STATIC		// static on the background

struct networkSetup *setup 	= NULL;
struct setupParser  *parser 	= NULL;
int		    imageCount	= 0;

struct tr_Window *root		= NULL;
struct tr_Window *wmain		= NULL;
struct tr_Object *cmain		= NULL;
struct tr_Object *bimageholder	= NULL;
struct tr_Object *bimage	= NULL;
struct tr_Object *slidebar	= NULL;

struct tr_Object *panel		= NULL;
struct tr_Object *panelButton	= NULL;
struct tr_Object *overlay	= NULL;

struct tr_Attr	 mainAttr;
struct tr_Attr	 attr;

struct mikaPlayer *player	= NULL;
static struct ipc_t *ipc	= NULL;

static struct ipc_module_t *ipcModule 		= NULL; // expose
static struct ipc_module_t *ipcModuleTouch 	= NULL; // button

static struct ipc_module_t *ipcModuleServer 	= NULL; // listening

typedef struct backgroundImage
{
  unsigned char 		filename[255];
  struct tr_Object 		*bimage;
  struct backgroundImage	*next;
} backgroundImage;

struct backgroundImage *backgroundImages = NULL;

#ifdef _STRESS
  #define _BACKGROUND_TIK 100
#else
  #define _BACKGROUND_TIK 10000*7 // second*num_seconds
#endif
void *_background_thread(void*data)
{
  static int index = 0;
  long long count = 0;
  while(1)
  {
    #ifdef _DEBUG
      printf("_background_thread count(%d)\n",count);
    #endif
    binarySemaphoreWait(bigInterface->activity.sem->id);
    
    int n;
    struct backgroundImage *image = backgroundImages;
    for(n = 0; n < index; n++)
      image = image->next;
    
    if (image)
    {
      tr_AddImage(bimageholder,image->bimage);
      tr_ImagePosCenter(image->bimage,bimageholder);
    }
    
    binarySemaphorePost(bigInterface->activity.sem->id);
    
    tr_WindowExpose(wmain);
    
    ipcSendMessage(ipc,ipcModule,(unsigned char*)"hello");
    
    if (index<imageCount-1)
      index++;
    else
      index = 0;
      
    usleep(_BACKGROUND_TIK);
    count++;
  }
}

/** is triggered after webkit has repainted our backplane
 */
void *onWebkitCommand( void *ipc, unsigned char *command )
{
  #ifdef _DEBUG
    printf("onWebkitCommand web\n");
  #endif
  return NULL;
}

int registerBackgroundImage( unsigned char *filename )
{
  #ifdef _DEBUG
    printf("registerBackgroundImage web filename(%s)\n",filename);
  #endif
  
  if (!filename)
  {
    #ifdef _DEBUG
      printf("registerBackgroundImage web ERROR, invalid arguments\n");
    #endif
    return 1;
  }
  
  struct backgroundImage *image = (struct backgroundImage*)malloc(sizeof(struct backgroundImage));
  image->next = backgroundImages;
  backgroundImages = image;
  
  sprintf((char*)image->filename,"setup/%s",(char*)filename);
  image->bimage = tr_ImageInitEx(NULL,&mainAttr,(char*)image->filename);
  
  return 0;
}

void *onButton(void*data)
{  
  #ifdef _DEBUG
    printf("onButton web\n");
  #endif
  
  struct gui_Event *event = (struct gui_Event*)data;
  struct gui_Widget *widget = event->widget;  
  
  unsigned char tmp[64];
  sprintf((char*)tmp,"touch,x%d,y%d,",event->x,event->y);
  ipcSendMessage(ipc,ipcModule,(unsigned char*)tmp);
  
  return NULL; // applications should get events from the interface delegate
}

void *onButtonMisc(void*data)
{
  #ifdef _DEBUG
    printf("onButtonMisc web\n");
  #endif
  return NULL;
}

void *onButtonPanel(void*data)
{
  #ifdef _DEBUG
    printf("onButtonPanel web\n");
  #endif
  return NULL;
}

void *onButtonPanelStart(void*data)
{
  #ifdef _DEBUG
    printf("onButtonPanelStart web\n");
  #endif
  return NULL;
}

int main( )
{
  interfaceSetName((unsigned char*)"WEB");
  
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
  
  root = tr_WindowInit(NULL,root,&mainAttr);
  wmain = tr_WindowInit(NULL,root,&mainAttr);
  
  tr_WindowMap(wmain);
  tr_WindowToTop(wmain);
  
  cmain = tr_CanvasInit(NULL,&mainAttr,wmain);
  tr_SetRound(cmain,3);
  #ifndef _USE_SKIN
    tr_SetColor(cmain,(char*)"#000000");
  #endif
  form_CanvasSetMov(cmain->widget.sub,CAN_MOV_NONE);
  
  //
  struct tr_Object *infoLabel = NULL;
  struct tr_Attr infoLabelAttr;
  infoLabelAttr.x = 0;
  infoLabelAttr.y = 0;
  infoLabelAttr.w = 480;
  infoLabelAttr.h = 480;
  infoLabel = tr_ButtonInitEx2(NULL,&infoLabelAttr,cmain);
  tr_SetAction(infoLabel,onButton,TR_ACTION_BUTTON);
  tr_SetColor(cmain,(char*)"#ffffff");
  tr_CanvasAddObjStatic(cmain,infoLabel);
  
  // webkit client 
  interfaceRecordBackplaneAccess((unsigned char*)"Web");
  //
  ipc = ipcInit(NULL);
  ipc->serverport = 46200;
  strcpy((char*)ipc->servername,"127.0.0.1");
  ipcModule = (struct ipc_module_t*)malloc(sizeof(struct ipc_module_t));
  strcpy((char*)ipcModule->keyword,"webkitcommand");
  ipcModuleRegister(ipc,ipcModule);
  // server
  ipc->listenport = 46201;
  ipcModuleServer = (struct ipc_module_t*)malloc(sizeof(struct ipc_module_t));
  strcpy((char*)ipcModuleServer->keyword,"background");
  ipcModuleServer->oncommand = onWebkitCommand;
  ipcModuleRegister(ipc,ipcModuleServer);
  ipcRun(ipc);
  interfaceSetBackplaneSize(mainAttr.w,mainAttr.h);
 
  pthread_t bt;
  pthread_create(&bt,NULL,_background_thread,NULL);
  
  tr_WindowQueue(root);
  
  return 0;
}
