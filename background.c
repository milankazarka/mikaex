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

#define _PLAYER
#define _PLAYER_MOVIE	"/work/mikaex/microSignage/media/film.avi"
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

static struct ipc_module_t *ipcModule = NULL;
static struct ipc_module_t *ipcModuleServer = NULL;

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
  return NULL;
}

int registerBackgroundImage( unsigned char *filename )
{
  #ifdef _DEBUG
    printf("registerBackgroundImage background filename(%s)\n",filename);
  #endif
  
  if (!filename)
  {
    #ifdef _DEBUG
      printf("registerBackgroundImage background ERROR, invalid arguments\n");
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
  struct activityObject oOlympic;
  
  #ifdef _DEBUG
    printf("onButton background\n");
  #endif
  return NULL; // applications should get events from the interface delegate
}

void *onButtonMisc(void*data)
{
  #ifdef _DEBUG
    printf("onButtonMisc background\n");
  #endif
  //tr_SetColor(cmain,(char*)"#ffffff");
  //tr_WindowExpose(wmain);
  return NULL;
}

void *onButtonPanel(void*data)
{
  #ifdef _DEBUG
    printf("onButtonPanel background\n");
  #endif
  return NULL;
}

void *onButtonPanelStart(void*data)
{
  #ifdef _DEBUG
    printf("onButtonPanelStart background\n");
  #endif
  return NULL;
}

int main( )
{
  interfaceSetName((unsigned char*)"BACKGROUND");
  
  osInit();
  trop_Init();
  gui_Init();
  tr_WmInit();
  
  layout = NULL;
  rotation = ROT_NONE;
  
  mainAttr.x = 0;
  mainAttr.y = 0;
  mainAttr.w = 440;
  mainAttr.h = 440;
  
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
  infoLabelAttr.x = 10;
  infoLabelAttr.y = 340;
  infoLabelAttr.w = 130;
  infoLabelAttr.h = 130;
  infoLabel = tr_ButtonInitEx2(NULL,&infoLabelAttr,cmain);
  tr_SetColor(infoLabel,(char*)"#202020");
  tr_SetRound(infoLabel,2);
  bimage = tr_ImageInitEx(NULL,&infoLabelAttr,(char*)"img/mplayer.jpg");
  tr_AddImage(infoLabel,bimage);
  tr_ImagePosCenter(bimage,infoLabel);
  tr_CanvasAddObjStatic(cmain,infoLabel);
  //
  /**infoLabelAttr.x = 160;
  infoLabelAttr.y = 340;
  infoLabelAttr.w = 130;
  infoLabelAttr.h = 130;
  infoLabel = tr_ButtonInitEx2(NULL,&infoLabelAttr,cmain);
  tr_SetColor(infoLabel,(char*)"#202020");
  tr_SetRound(infoLabel,2);
  tr_CanvasAddObjStatic(cmain,infoLabel);*/
  //
  
  /**
  bimageholder = tr_ButtonInitEx2(NULL,&mainAttr,cmain);
  tr_SetAction(bimageholder,onButton,TR_ACTION_BUTTONDOWN);
  tr_SetColor(bimageholder,(char*)"#000000");
  tr_SetRound(bimageholder,2);
  tr_CanvasAddObjStatic(cmain,bimageholder);
  bimage = tr_ImageInitEx(NULL,&mainAttr,(char*)"casinoWallpaperSmall.jpg");
  tr_AddImage(bimageholder,bimage);
  tr_ImagePosCenter(bimage,bimageholder);
  
  activitySetMovable(&bigInterface->activity,0,1);
  
  setup = networkSetupInit(NULL);
  strcpy((char*)setup->server,"ftp://localhost");
  strcpy((char*)setup->path,"");
  strcpy((char*)setup->username,"anonymous");
  strcpy((char*)setup->password,"secret");
  strcpy((char*)setup->filename,"background.xml");
  setup->port = 21;
  networkSetupDownloadSetup(setup);
  
  parser = setupParserInit(NULL,setup);
  if (parser)
  {
    imageCount = setupParserGetElementCount(parser,setup,
      (unsigned char*)"filenames",(unsigned char*)"filename");
  
    if (imageCount==0)
    {
      #ifdef _DEBUG
        printf("background WARNING no images in setup\n");
      #endif
    }
    
    int n;
    for(n = 0; n < imageCount; n++)
    {
      TiXmlElement *filename = setupParserGetElementAtIndex(parser,setup,
        (unsigned char*)"filenames",(unsigned char*)"filename",n);
      if (filename)
      {
        #ifdef _DEBUG
          printf("background image(%s)\n",(char*)filename->GetText());
        #endif
        registerBackgroundImage((unsigned char*)filename->GetText());
      }
    }
  }
  else
  {
    #ifdef _DEBUG
      printf("background ERROR parser not created\n");
    #endif
  } 
 */
  
  // webkit client 
  /**
  interfaceRecordBackplaneAccess((unsigned char*)"Background");
  ipc = ipcInit(NULL);
  ipc->serverport = 46200;
  strcpy((char*)ipc->servername,"127.0.0.1");
  ipcModule = (struct ipc_module_t*)malloc(sizeof(struct ipc_module_t));
  strcpy((char*)ipcModule->keyword,"webkitcommand");
  ipcModuleRegister(ipc,ipcModule);
  // webkit listener / server
  ipc->listenport = 46201;
  ipcModuleServer = (struct ipc_module_t*)malloc(sizeof(struct ipc_module_t));
  strcpy((char*)ipcModuleServer->keyword,"background");
  ipcModuleServer->oncommand = onWebkitCommand;
  ipcModuleRegister(ipc,ipcModuleServer);
  ipcRun(ipc);
  */
 
  pthread_t bt;
  pthread_create(&bt,NULL,_background_thread,NULL);
  
  activityRegisterAccess(&bigInterface->activity,(unsigned char*)"mikaAccessBackground.log");
  interfaceRecordBackplaneAccess((unsigned char*)"Background");
  interfaceSetBackplaneSize(mainAttr.w,320);
  
  #ifdef _PLAYER
    player = mikaPlayerInit(NULL);
    sprintf((char*)player->filename,"%s",_PLAYER_MOVIE);
    player->customh = 320;
    mikaPlayerPlay(player);
  #endif
  
  /**
  cmain = tr_CanvasInit(NULL,&mainAttr,wmain);
  tr_SetRound(cmain,3);
  tr_SetColor(cmain,(char*)"#cacaca");
  form_CanvasSetMov(cmain->widget.sub,CAN_MOV_NONE);
  
  attr.x = 10;
  attr.y = 0;
  attr.w = 17;
  attr.h = 380;
  slidebar = tr_SlidebarInit(NULL,&attr,cmain);
  tr_SetColor(slidebar,(char*)"#303030");
  tr_CanvasAddObjStatic(cmain,slidebar);
  struct gui_Widget *wslidebar = slidebar->widget.sub;
  struct form_SlidebarEx *fslidebar = (struct form_SlidebarEx*)wslidebar->form;
  fslidebar->total 	= 100;
  fslidebar->position 	= 20;
  fslidebar->size 	= 60;
  
  attr.x = 100;
  attr.y = 400;
  attr.w = 400;
  attr.h = 80;
  panel = tr_ButtonInitEx2(NULL,&attr,cmain);
  tr_SetAction(panel,onButtonMisc,TR_ACTION_BUTTON);
  tr_SetColor(panel,(char*)"#303030");
  tr_SetRound(panel,3);
  tr_CanvasAddObjStatic(cmain,panel);
  
  attr.x = 100;
  attr.y = 100;
  attr.w = 280;
  attr.h = 280;
  overlay = tr_ButtonInitEx2(NULL,&attr,cmain);
  tr_SetAction(overlay,onButtonMisc,TR_ACTION_BUTTON);
  tr_SetColor(overlay,(char*)"#303030");
  tr_SetRound(overlay,3);
  tr_CanvasAddObjStatic(cmain,overlay);
  
  int nb;
  for(nb = 0; nb < 5; nb++)
  {
    attr.x = 0+(nb*80);
    attr.y = 0;
    attr.w = 80;
    attr.h = 80;
    panelButton = tr_ButtonInitEx2(NULL,&attr,cmain);
    tr_SetAction(panelButton,onButtonPanel,TR_ACTION_BUTTON);
    tr_SetAction(panelButton,onButtonPanelStart,TR_ACTION_BUTTONDOWN);
    tr_SetColor(panelButton,(char*)"#909090");
    tr_SetRound(panelButton,3);
    tr_PutObject(panel,panelButton);
  }
  */
  
  
  
  tr_WindowQueue(root);
  
  return 0;
}
