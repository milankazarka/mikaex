/** copyright Milan Kazarka, 2011
 *  M.R.Stefanika 44/29, Ziar nad Hronom 96501, Slovakia
 */
extern "C" {
  #include "interface.h"
  #include "common.h"
  #include "config.h"
  #include "os.h"
  #include "system.h"
  #include "systemip.h"
  #include "global.h"
  #include "xml.h"
  #include "wm.h"
  #include "dom.h"
  #include "activityObject.h"
  #include "rootControllerSync.h"
  #include "connectionWatchdog.h"
  #include "networkSetup.h"
  #include "mikaPosClient.h"
  #include "basic.h"
}
#include "setupParser.h"
#include "posSetup.h"
#include "langSetup.h"

#define _CONNECTED_NO
#define _SUSHI_CONNECTION_WATCHDOG_NO
#define _SUSHI_INACTIVITY
#define _WINDOW_SIZE 425

struct tr_Window *root		= NULL;
struct tr_Window *wmain		= NULL;
struct tr_Window *wmainmenu	= NULL;
struct tr_Window *wordermenu	= NULL;
struct tr_Window *wconfirm	= NULL;
struct tr_Window *wmika	= NULL;

struct tr_Object *cmain		= NULL;
struct tr_Object *cmainmenu	= NULL;
struct tr_Object *cordermenu	= NULL;
struct tr_Object *ctitlecanvas	= NULL;
struct tr_Object *cconfirm	= NULL;
struct tr_Object *cmika	= NULL;

struct tr_Object *status	= NULL;

struct tr_Object *menuImage	= NULL;

struct tr_Object *listbox	= NULL;

struct tr_Object *buttons[4]	= { 0x00, 0x00, 0x00, 0x00 };
struct tr_Object *poster	= NULL;

struct tr_Attr	 mainAttr;
struct tr_Attr	 menuAttr;
struct tr_Attr	 titleAttr;
struct tr_Attr	 buttonAttr;
struct tr_Attr	 previewAttr;
struct tr_Attr	 statusAttr;

struct interface_delegate interfaceDelegate;
struct networkSetup *setup		= NULL;
struct setupParser *parser		= NULL;
struct langSetup *lang			= NULL;
struct itemSetup *currentOrderItem	= NULL; // for which item we are displaying an order menu
struct posSetup *pos			= NULL;
struct mikaPosClient client;	// pos terminal
struct basic *basicConfig		= NULL;

int		 active = 1; // shown on the screen
int		 activityTrigger = 0;

/** our own timestamp retrieval
 */
unsigned long _schedullerTime( )
{
  static int usetimer = 0;
  static unsigned long long inittime;
  struct tms cputime;
  
  if (usetimer == 0)
  {
    inittime = (unsigned long long)times(&cputime);
    usetimer = 1;
  }
  
  return (unsigned long)((times(&cputime) - inittime)*1000UL/sysconf(_SC_CLK_TCK));
}

/** automatic menu setup objects
 */
typedef struct itemSetup
{
  TiXmlElement		*xmlObject;
  struct tr_Attr	attr;
  struct tr_Object	*object;
  struct itemSetup	*next;
} itemSetup;

typedef struct windowSetup
{
  TiXmlElement		*referenceElement;
  /** hack - should be done the other way
   */
  struct gui_Widget	*trigger;
  
  struct tr_Attr	attr;
  struct tr_Window	*window;
  struct tr_Object	*canvas;
  struct tr_Object	*back;
  struct itemSetup	*items;
  struct windowSetup 	*next;
  
  struct windowSetup	*screens; // subscreens (when dynamic multi-level setup)
  struct windowSetup	*parent;
} windowSetup;

typedef struct menuSetup
{
  struct networkSetup *setup;
  struct setupParser *parser;
} menuSetup;

struct menuSetup *globalms = NULL;

/** automatically setup the menu from the configuration files
 */
struct menuSetup *menuSetupInit( struct menuSetup *ms );
int menuSetupCreateSubMenus( struct menuSeup *ms );
struct windowSetup *generateScreens( TiXmlElement *root, windowSetup *rootws );
struct windowSetup *windowSetupOfButtonInHierarchy(
  struct windowSetup *ws, struct gui_Widget *widget );

static struct windowSetup *windows = NULL;
static struct windowSetup *windowHierarchy = NULL;
static struct windowSetup *windowCurrent = NULL;

/** initialize Window configuration
 */
struct windowSetup *windowSetupInit( struct windowSetup *ws )
{
  if (!ws)
    ws = (struct windowSetup*)malloc(sizeof(struct windowSetup));
  
  ws->referenceElement = NULL;
  ws->back = NULL;
  ws->trigger = NULL; // object which triggers tris window
  ws->window = NULL;
  ws->canvas = NULL;
  ws->items = NULL;
  ws->next = NULL;
  ws->screens = NULL;
  ws->parent = NULL;
  
  return ws;
}

/** create new window and add it to the list of windows
 */
struct windowSetup *windowSetupNewWindow( )
{
  struct windowSetup *ws = windowSetupInit(NULL);
  ws->next = windows;
  windows = ws;
  return ws;
}

/** add a subscreen to a screen setup
 */
int windowSetupAddScreen( struct windowSetup *ws, struct windowSetup *child )
{
  #ifdef _DEBUG
    printf("windowSetupAddScreen\n");
  #endif
  
  if (!ws || !child)
  {
    #ifdef _DEBUG
      printf("windowSetupAddScreen ERROR invalid arguments\n");
    #endif
    return 1;
  }
  
  child->next = NULL;
  child->parent = ws;
  struct windowSetup *current = ws->screens;
  if (!current)
  {
    ws->screens = child;
  }
  else
  {
    while(current)
    {
      if (!current->next)
      {
        current->next = child;
        break;
      }
      current = current->next;
    }
  }
  
  return 0;
}

struct itemSetup *itemSetupInit( struct itemSetup *item, struct tr_Object *object )
{
  if (!item)
    item = (struct itemSetup*)malloc(sizeof(struct itemSetup));
  
  item->object = object;
  
  return item;
}

/** retrieve a windowSetup which holds a gui object (object)
 */
struct windowSetup *getWindowSetupByObject( struct gui_Widget *widget )
{
  unsigned long start = _schedullerTime();
  #ifdef _DEBUG
    printf("getWindowSetupByObject start(%d)\n",start);
  #endif
  struct tr_Object *object;
  struct itemSetup *item = NULL;
  struct windowSetup *ws = NULL;
  struct windowSetup *window = windows;
  struct gui_Widget *currentWidget = NULL;
  while(window)
  {
    item = window->items;
    while(item)
    {
      object = item->object;
      currentWidget = object->widget.sub;
      if (currentWidget==widget)
      {
        #ifdef _DEBUG
          printf("getWindowSetupByObject end(%d)\n",_schedullerTime()-start);
        #endif
        return window;
      }
      item = item->next;
    }
    window = window->next;
  }
  return NULL;
}

struct itemSetup *getItemSetupByObject( struct windowSetup *window, struct gui_Widget *widget )
{
  if (!window || !widget)
    return NULL;

  struct itemSetup *item = window->items;
  while(item)
  {
    if (item->object)
    {
      if (item->object->widget.sub==widget)
        return item;
    }
    item = item->next;
  }

  return NULL;
}

struct windowSetup *getWindowSetupByTrigger( struct gui_Widget *widget )
{
  struct windowSetup *window = windows;
  while(window)
  {
    if (window->trigger==widget)
      return window;
    window = window->next;
  }
  return NULL;
}

/** add a new itemSetup into the windowSetup
 */
struct itemSetup *windowSetupAddItem( struct windowSetup *ws, struct tr_Object *object )
{
  if (!ws || !object)
  {
    #ifdef _DEBUG
      printf("windowSetupAddItem\n");
    #endif
    return NULL;
  }
  
  struct itemSetup *is = itemSetupInit(NULL,object);
  if (is)
  {
    is->next = ws->items;
    ws->items = is;
  }
  else
  {
    #ifdef _DEBUG
      printf("windowSetupAddItem ERROR couldn't ainitialize item\n");
    #endif
    return NULL;
  }
  
  return is;
}
 
#define _MS_SERVER	"ftp://localhost"
#define _MS_PATH	""
#define _MS_USERNAME	"anonymous"
#define _MS_PASSWORD	"secret"
#define _MS_PORT	21
struct menuSetup *menuSetupInit( struct menuSetup *ms )
{
  if (!ms)
    ms = (struct menuSetup*)malloc(sizeof(struct menuSetup));
  
  ms->setup = networkSetupInit(NULL);
  if (ms->setup)
  {
    strcpy((char*)ms->setup->server,_MS_SERVER);
    strcpy((char*)ms->setup->path,_MS_PATH);
    strcpy((char*)ms->setup->username,_MS_USERNAME);
    strcpy((char*)ms->setup->password,_MS_PASSWORD);
    ms->setup->port = _MS_PORT;
    networkSetupDownloadSetup(ms->setup);
    
    ms->parser = setupParserInit(NULL,ms->setup);
    if (ms->parser)
    {
      
    }
    else
    {
      #ifdef _DEBUG
        printf("menuSetupInit ERROR couldn't initialize setup parser\n");
      #endif
    }
  }
  else
  {
    #ifdef _DEBUG
      printf("menuSetupInit ERROR couldn't initialize network setup\n");
    #endif
  }
  
  return ms;
}

#define _POLL_VISIBILITY_NO
#define _POLL_VISIBILITY_INTERVAL	200000
void *_poll_visibility(void*data) // deprecated, remove
{
  #ifndef _POLL_VISIBILITY
    return NULL;
  #endif
  static struct activityObject oSushi;
  static int visibility = 1;
  if (actObjects==NULL)
    actObjects = activityObjectsClone();
  while(1)
  {
    if ( activityObjectClone(actObjects,(unsigned char*)"rootActivity",&oSushi) == 0 )
    {
      if (visibility!=oSushi.fastData[0])
      {
        if (oSushi.fastData[0]==0)
          activitySetVisible(&bigInterface->activity,0,0); // SYNCED?
        else
          activitySetVisible(&bigInterface->activity,1,0);
        
        tr_WindowExpose(wmain);
        visibility = oSushi.fastData[0];
      }
    }
    usleep(_POLL_VISIBILITY_INTERVAL);
  }
}

void *_on_connection_status( void *watchdogptr, int alive )
{
  #ifdef _DEBUG
    printf("_on_connection_status\n");
  #endif
  if (!watchdogptr)
  {
    #ifdef _DEBUG
      printf("_on_connection_status ERROR invalid arguments\n");
    #endif
    return NULL;
  }
  
  #ifdef _DEBUG
    printf("_on_connection_status alive(%d)\n",alive);
  #endif
  
  static unsigned char *current_status_text = NULL;
  int expose = 0;
  
  unsigned char ip[32];
  strcpy((char*)ip,"");
  systemipGet((unsigned char*)ip);
  
  binarySemaphoreWait(bigInterface->activity.sem->id);
  char *statusText = (char*)malloc(32);
  if (alive==0)
    sprintf(statusText,"%s connection OK",(char*)ip);
  else
    sprintf(statusText,"%s connection ERROR",(char*)ip);
  
  if (current_status_text==NULL)
  {
    current_status_text = (unsigned char*)malloc(64);
    strcpy((char*)current_status_text,(char*)statusText);
    expose = 1;
  }
  else
  {
    if (strcmp((char*)current_status_text,(char*)statusText)!=0)
    {
      strcpy((char*)current_status_text,(char*)statusText);
      expose = 1;
    }
    else
      expose = 0;
  }
  
  if (expose==1)
    tr_ChangeTextExPos(status,statusText,5,10);
  binarySemaphorePost(bigInterface->activity.sem->id);
  
  if (active==1)
  {
    activitySetToplevel(&bigInterface->activity,1,1);
    if (expose==1)
      tr_WindowExpose(wmain);
  }
}

#ifdef _STRESS
  #define _INACTIVITY_TIMEOUT	5
  #define _INACTIVITY_TIK	200000
#else
  #define _INACTIVITY_TIMEOUT 	20
  #define _INACTIVITY_TIK	200000
#endif
void *_inactivity_thread_ex(void*data)
{
  struct activityObjects *myActObjects;
  struct activityObject oOlympic;
  int n;
  
  if (myActObjects==NULL)
  {
    myActObjects = activityObjectsClone();
    if (!myActObjects)
    {
      #ifdef _DEBUG
        printf("_inactivity_thread ERROR activity objects could not be cloned\n");
      #endif
    }
  }
  
  n = 0;
  while(1)
  {
    if ( activityObjectClone(myActObjects,(unsigned char*)"rootActivity",&oOlympic) == 0 )
        activityTrigger = oOlympic.fastData[0];
    
    if (activityTrigger==1)
    {
      #ifdef _DEBUG
        printf("_inactivity_thread_ex activityTrigger=1\n");
      #endif
      oOlympic.fastData[0] = 0;
      activityObjectSet(myActObjects,(unsigned char*)"rootActivity",&oOlympic);
      if (active==0)
      {
        active = 1;
        activitySetVisible(&bigInterface->activity,1,1); // synced
        tr_WindowExpose(wmain);
      }
      n = 0;
    }
    
    if (n==_INACTIVITY_TIMEOUT*1000000 && active==1)
    {
      #ifdef _DEBUG
        printf("_inactivity_thread_ex go to background\n");
      #endif
      active = 0;
      oOlympic.fastData[0] = 0;
      activityObjectSet(myActObjects,(unsigned char*)"rootActivity",&oOlympic);
      activitySetVisible(&bigInterface->activity,0,1); // synced
      tr_WindowExpose(wmain);
      n = 0;
    }
    usleep(_INACTIVITY_TIK);
    n+=_INACTIVITY_TIK;
  }
}

/** the interface can delegate events to this application
 */
void *interfaceOnEvent(void*event)
{
  #ifdef _DEBUG
    printf("interfaceOnEvent\n");
  #endif
}

void *postinitSetup(void*data)
{
  sleep(2);
  activitySetToplevel(&bigInterface->activity,1,1);
  tr_WindowExpose(wmain);
}

void *onMainButton(void*data)
{
  struct gui_Event *event = (struct gui_Event*)data;
  struct gui_Widget *widget = event->widget;
  #ifdef _DEBUG
    printf("onMainButton\n");
  #endif
}

/** triggered by a button on the main menu
 */
void *onMainmenu(void*data)
{
  struct gui_Event *event = (struct gui_Event*)data;
  struct gui_Widget *widget = event->widget;
  #ifdef _DEBUG
    printf("onMainmenu (%p)\n",widget);
  #endif
  
  tr_WindowToTop(wmainmenu);
  tr_WindowExpose(wmainmenu);
}

void *onMainmenuItem(void*data)
{
  struct gui_Event *event = (struct gui_Event*)data;
  struct gui_Widget *widget = event->widget;
  #ifdef _DEBUG
    printf("onMainmenu (%p)\n",widget);
  #endif
  
  struct windowSetup *ws = getWindowSetupByTrigger(widget);
  #ifdef _DEBUG
    printf("onMainmenu ws(%p)\n",ws);
  #endif
  if (ws)
  {
    tr_WindowToTop(ws->window);
    tr_WindowExpose(ws->window);
  }
}

void *onMain(void*data)
{
  printf("onMain\n");
  tr_WindowToTop(wmain);
  tr_WindowExpose(wmain);
}

int showOrderMenuForItem( struct itemSetup *is )
{
  #ifdef _DEBUG
    printf("showOrderMenuForItem\n");
  #endif
  if (!is)
  {
    #ifdef _DEBUG
      printf("showOrderMenuForItem\n");
    #endif
    return 1;
  }
  
  currentOrderItem = is;
  tr_WindowToTop(wordermenu);
  tr_WindowExpose(wordermenu);
  
  return 0;
}

void *onMenuItem(void*data)
{
  struct gui_Event *event = (struct gui_Event*)data;
  struct gui_Widget *widget = event->widget;
  #ifdef _DEBUG
    printf("onMenuItem (%p)\n",widget);
  #endif
  
  struct windowSetup *ws = getWindowSetupByObject(widget);
  #ifdef _DEBUG
    printf("onMenuItem ws(%p)\n",ws);
  #endif
  if (ws)
  {
    struct itemSetup *is = getItemSetupByObject(ws,widget);
    #ifdef _DEBUG
      printf("onMenuItem is(%p)\n",is);
    #endif
    if (is)
    {
      showOrderMenuForItem(is);
    }
  }
}

struct mikaPosOrder order;
void *onOrder(void*data)
{
  #ifdef _DEBUG
    printf("onOrder item(%p)\n",currentOrderItem);
  #endif
  
  sprintf((char*)order.value,"T1:%s",
    currentOrderItem->xmlObject->Attribute("name"));
  mikaPosClientCommandAddOrder(&client,&order);
  
  tr_WindowToTop(wmainmenu);
  tr_WindowExpose(wmainmenu);
}

void *onCancel(void*data)
{
  #ifdef _DEBUG
    printf("onCancel item(%p)\n",currentOrderItem);
  #endif
  
  tr_WindowToTop(wmainmenu);
  tr_WindowExpose(wmainmenu);
}

int windowSetupForBackButtonInHierarchy( 
  struct gui_Widget *widget, struct windowSetup *currentws )
{
  #ifdef _DEBUG
    printf("windowSetupForBackButtonInHierarchy\n");  
  #endif
  struct windowSetup *current = currentws;
  while(current)
  {
    if (current->back)
    {
      if (current->back->widget.sub==widget)
      {
        windowCurrent = current;
      }
    }
    windowSetupForBackButtonInHierarchy(widget,current->screens);
    current = current->next;
  }
  #ifdef _DEBUG
    printf("windowSetupForBackButtonInHierarchy finish\n");
  #endif
  return 0;
}

/** onBack in the tree structure of windows
 */
void *onBack(void*data)
{
  #ifdef _DEBUG
    printf("onBack\n");
  #endif
  struct gui_Event *event = (struct gui_Event*)data;
  struct gui_Widget *widget = event->widget;
  windowCurrent = NULL;
  windowSetupForBackButtonInHierarchy(widget,windowHierarchy);
  
  if (windowCurrent)
  {
    #ifdef _DEBUG
      printf("onBack setup\n");
    #endif
    if (windowCurrent->parent)
    {
      #ifdef _DEBUG
        printf("onBack setup->parent %p\n",windowCurrent->parent->window);
      #endif
      tr_WindowToTop(windowCurrent->parent->window);
      tr_WindowExpose(windowCurrent->parent->window);
    }
  }
}

/** determine the windowSetup in windowSetup hierarchy to which
 *  the button belongs through windowSetup->trigger
 */
static struct windowSetup *windowToButton = NULL; // dirty hack
struct windowSetup *windowSetupOfButtonInHierarchy(
  struct windowSetup *ws,
  struct gui_Widget *widget
  )
{
  if (ws->trigger==widget)
  {
    windowToButton = ws;
  }
  else
  {
    struct windowSetup *current = ws->screens;
    while(current)
    {
      if (current->trigger==widget)
      {
        windowToButton = current;
        break;
      }
      else
      {
        windowSetupOfButtonInHierarchy(current,widget);
      }
      current = current->next;
    }
  }
  if (!windowToButton)
  {
    #ifdef _DEBUG
      printf("windowSetupOfButtonInHierarchy FAIL no windowSetup found\n");
    #endif
  }
  return windowToButton;
}

/** on Button in window Hierarchy
 */
void *onHierarchyButton(void*data)
{
  #ifdef _DEBUG
    printf("onHierarchyButton\n");
  #endif
  struct gui_Event *event = (struct gui_Event*)data;
  struct gui_Widget *widget = event->widget;
  
  windowToButton = NULL;
  struct windowSetup *setup = windowSetupOfButtonInHierarchy(windowHierarchy,widget);
  if (setup)
  {
    #ifdef _DEBUG
      printf("onHierarchyButton go to screen\n");
    #endif
    windowCurrent = setup;
    tr_WindowToTop(setup->window);
    tr_WindowExpose(setup->window);
    //tr_WindowToTop(wmainmenu);
    //tr_WindowExpose(wmainmenu);
  }
  else
  {
    #ifdef _DEBUG
      printf("onHierarchyButton go to Item menu\n");
    #endif
  }
}

void *onMika(void*data)
{
  #ifdef _DEBUG
    printf("onMika\n");
  #endif
  tr_WindowToTop(wmika);
  tr_WindowExpose(wmika);
}

/** initialize a GUI screen from an XML element and work on it's setup
 */
struct windowSetup *initScreenFromElement( TiXmlElement *screen )
{
  #ifdef _DEBUG
    printf("initScreenFromElement\n");
  #endif
  struct tr_Object *button = NULL;
  struct tr_Object *image = NULL;
  struct windowSetup *ws = NULL;
  if (!screen)
  {
    #ifdef _DEBUG
      printf("initScreenFromElement\n");
    #endif
    return NULL;
  }
  
  ws = windowSetupInit(NULL);

  tr_Window *window = NULL;
  if (windowHierarchy)
    window = tr_WindowInit(NULL,root,&menuAttr);
  else
    window = wmain;
  tr_WindowToTop(window);
  if (window==wmain)
  {
    /**
    ctitlecanvas = tr_CanvasInit(NULL,&titleAttr,window);
    tr_SetRound(ctitlecanvas,3);
    tr_SetColor(ctitlecanvas,(char*)"#050505");
    form_CanvasSetMov(ctitlecanvas->widget.sub,CAN_MOV_NONE);
    //form_CanvasSetMovestep(ctitlecanvas->widget.sub,70);
    //
    buttonAttr.x = 0;
    buttonAttr.y = 0;
    buttonAttr.w = 320;
    buttonAttr.h = 55;
    button = tr_ButtonInitEx2(NULL,&buttonAttr,cmain);
    tr_SetRound(button,2);
    tr_SetColor(button,(char*)"#000000");
    tr_CanvasAddObj(ctitlecanvas,button);
    image = tr_ImageInitEx(NULL,&buttonAttr,(char*)"olympicImages/logo.jpg");
    tr_ImageSetRound(image,2);
    tr_AddImage(button,image);
    tr_ImagePosCenter(image,button);
    */
  }
  tr_Object *canvas = tr_CanvasInit(NULL,&menuAttr,window);
  ws->window = window;
  ws->canvas = canvas;
  // trigger - is set outside, where we have created the button cycling 
  //           through the child elements
  tr_SetRound(canvas,3);
  tr_SetColor(canvas,(char*)"050505");
  form_CanvasSetMov(canvas->widget.sub,CAN_MOV_SCROLL);
  //form_CanvasSetMovestep(canvas->widget.sub,70);
  // home button, moving back
  if (window!=wmain)
  {
    buttonAttr.x = menuAttr.w-90;
    buttonAttr.y = menuAttr.h-90;
    buttonAttr.w = 60;
    buttonAttr.h = 60;
    button = tr_ButtonInitEx2(NULL,&buttonAttr,canvas);
    ws->back = button;
    tr_SetRound(button,2);
    tr_SetColor(button,(char*)"#fafafa");
    tr_SetAction(button,onBack,TR_ACTION_BUTTON);
    tr_CanvasAddObjStatic(canvas,button);
    image = tr_ImageInitEx(NULL,&buttonAttr,(char*)"img/back.jpg");
    tr_AddImage(button,image);
    tr_ImagePosCenter(image,button);
  }
  
  buttonAttr.x = menuAttr.w-121; // about Mika button
  buttonAttr.y = 5;
  buttonAttr.w = 121;
  buttonAttr.h = 102;
  button = tr_ButtonInitEx2(NULL,&buttonAttr,canvas);
  tr_SetRound(button,2);
  tr_SetColor(button,(char*)"#313131");
  tr_CanvasAddObjStatic(canvas,button);
  tr_SetAction(button,onMika,TR_ACTION_BUTTON);
  //
  image = tr_ImageInitEx(NULL,&buttonAttr,(char*)"img/mikaLogo.jpg");
  tr_ImageSetRound(image,2);
  tr_AddImage(button,image);
  tr_ImagePosCenter(image,button);
  
  #ifdef _DEBUG
    printf("initScreenFromElement finish\n");
  #endif
  
  if (screen->Attribute("name"))
  {
    //if (strlen((const char*)screen->Attribute("name"))==strlen((const char*)"main"))
      if (strcmp(screen->Attribute("name"),"main")!=0)
        generateScreens(screen,ws);
  }
  
  return ws;
}

/** setup button from XML element
 */
#define _BUTTON_LINE 38
int setupButton( struct tr_Object *button, TiXmlElement *item )
{
  struct tr_Object *image = NULL;
  char *tmp = NULL;
  char *text = NULL;
  char *text2 = NULL;
    
  tr_SetRound(button,2);
  tr_SetColor(button,(char*)"#303030");
  tr_SetFontColor(button,0xfa,0xfa,0xfa,240);
  tr_SetFontSize(button,15);
  tr_SetAction(button,onHierarchyButton,TR_ACTION_BUTTON);
  if (item->Attribute("name"))
  {
    text = (char*)malloc(256);
    text2 = (char*)malloc(256);
    tmp = (char*)malloc(512);
    sprintf((char*)tmp,"%s",item->Attribute("name"));
    if (strlen((char*)tmp)<_BUTTON_LINE+1)
    {
      if (item->Attribute("image"))
        tr_ChangeTextExPos(button,tmp,90,40);
      else
        tr_ChangeTextExPos(button,tmp,0,40);
    }
    else
    {
      
      strncpy((char*)text,(char*)tmp,_BUTTON_LINE);
      text[_BUTTON_LINE] = 0x00;
      strncpy((char*)text2,(char*)tmp+_BUTTON_LINE,strlen((char*)tmp)-_BUTTON_LINE);
      text2[strlen((char*)tmp)-_BUTTON_LINE] = 0x00;
      
      if (item->Attribute("image"))
      {
        tr_ChangeTextExPos(button,text,90,30);
        tr_ChangeTextExPos(button,text2,90,48);
      }
      else
      {
        tr_ChangeTextExPos(button,text,-5,30);
        tr_ChangeTextExPos(button,text2,-5,48);
      }
    }
  }
  
  if (item->Attribute("price"))
  {
    text = (char*)malloc(256);
    sprintf((char*)text,"%s",item->Attribute("price"));
    struct gui_Widget *w = button->widget.sub;
    tr_ChangeTextExPos(button,text,w->attr.w-60,40); // 310,40
  }
  
  if (item->Attribute("image"))
  {
    image = tr_ImageInitEx(NULL,&buttonAttr,(char*)item->Attribute("image"));
    if (image)
    {
      tr_ImageSetRound(image,2);
      tr_AddImage(button,image);
      tr_ImagePosLeft(image,button);
    }
  }
  
  return 0;
}

int listScreenHierarchy( struct windowSetup *ws )
{
  if (!ws)
  {
    #ifdef _DEBUG
      printf("listScreenHierarchy ERROR invalid arguments\n");
    #endif
    return 1;
  } 
  return 0;
}

int setWindowSetupByWidget( struct gui_Widget *widget, struct windowSetup *ws )
{
  struct gui_Widget *canvasWidget = NULL;
  struct windowSetup *current = ws;
  while(current)
  {
    if (current->canvas)
    {
      canvasWidget = current->canvas->widget.sub;
      #ifdef _DEBUG
        printf("setWindowSetupByWidget compare(%p/%p)\n",canvasWidget,widget->parent);
      #endif
      if (canvasWidget==widget->parent)
      {
        #ifdef _DEBUG
          printf("setWindowSetupByWidget found parent\n");
        #endif
        windowCurrent = current;
      }
    }
    setWindowSetupByWidget(widget,current->screens);
    current = current->next;
  }
  return 0;
}

/** confirmation of order
 */
void *onConfirmBack(void*data)
{
  #ifdef _DEBUG
    printf("onConfirmBack\n");
  #endif
  tr_WindowToTop(windowCurrent->window);
  tr_WindowExpose(windowCurrent->window);
}

void *onConfirmOrder(void*back)
{
  if (windowCurrent)
  {
    #ifdef _DEBUG
      printf("onHierarchyOrder construct order\n");
    #endif
    if (windowCurrent->referenceElement)
    {
      #ifdef _DEBUG
        printf("onHierarchyOrder send order\n");
      #endif
      TiXmlElement *element = windowCurrent->referenceElement;
      sprintf((char*)order.value,"%s:%s",
        (char*)basicGet(basicConfig,(unsigned char*)"id"),
        element->Attribute("name"));
      mikaPosClientCommandAddOrder(&client,&order);
    }
  }
}

void doOrder( )
{
  if (windowCurrent)
  {
    #ifdef _DEBUG
      printf("showConfirmation construct order\n");
    #endif
    if (windowCurrent->referenceElement)
    {
      #ifdef _DEBUG
        printf("showConfirmation send order id(%s)\n",
          (char*)basicGet(basicConfig,(unsigned char*)"id"));
      #endif
      TiXmlElement *element = windowCurrent->referenceElement;
      if (element->Attribute("id"))
        sprintf((char*)order.value,"%s:%s",
          basicGet(basicConfig,(unsigned char*)"id"),
          element->Attribute("id"));
      else
        sprintf((char*)order.value,"%s:%s",
          basicGet(basicConfig,(unsigned char*)"id"),
          element->Attribute("name"));
      mikaPosClientCommandAddOrder(&client,&order);
    }
  }
}

void showConfirmation( )
{
  doOrder();
  tr_WindowToTop(wconfirm);
  tr_WindowExpose(wconfirm);
}

/** order the item and show the confirmation screen
 */
#define _ORDER_TO_MAIN
void *onHierarchyOrder(void*data)
{
  struct gui_Event *event = (struct gui_Event*)data;
  struct gui_Widget *widget = event->widget;
  struct gui_Widget *canvasWidget = NULL;
  
  #ifdef _DEBUG
    printf("onHierarchyOrder\n");
  #endif
  
  /** determine which windowSEtup is the parent of the trigger button
   */
  windowCurrent = NULL; 
  struct windowSetup *current = windowHierarchy;
  setWindowSetupByWidget(widget,current);
  //
  // place where to order if we would be skipping confirmation
  //showConfirmation();
  doOrder();
  #ifdef _ORDER_TO_MAIN
    onMain(NULL);
  #else
    if (windowCurrent)
    {
      if (windowCurrent->referenceElement)
      {
        tr_WindowToTop(windowCurrent->parent->window);
        tr_WindowExpose(windowCurrent->parent->window);
      }
    }
  #endif
}

void *onHierarchyCancel(void*data)
{
  struct gui_Event *event = (struct gui_Event*)data;
  struct gui_Widget *widget = event->widget;
  struct gui_Widget *canvasWidget = NULL;
  
  #ifdef _DEBUG
    printf("onHierarchyCancel\n");
  #endif
  
  windowCurrent = NULL;
  struct windowSetup *current = windowHierarchy;
  setWindowSetupByWidget(widget,current);
  if (windowCurrent)
  {
    if (windowCurrent->referenceElement)
    {
      #ifdef _DEBUG
        printf("onHierarchCancel\n");
      #endif
      tr_WindowToTop(windowCurrent->parent->window);
      tr_WindowExpose(windowCurrent->parent->window);
    }
  }
}

/** setup a screen by it's XML object
 */
int setupItemScreen( struct windowSetup *ws )
{
  #ifdef _DEBUG
    printf("setupItemScreen\n");
  #endif
  tr_WindowToTop(ws->window);
  // the TiXmlElement is saved in ws->referenceElement, so that we can
  // access misc attributes of UI objects in real time
  //
  if (!ws->referenceElement)
  {
    #ifdef _DEBUG
      printf("setupItemScreen ERROR referenceObject missing\n");
    #endif
    return 1;
  }
  
  TiXmlElement *menu = ws->referenceElement->FirstChildElement("menu");
  if (menu)
  {
    #ifdef _DEBUG
      printf("setupItemScreen menu setup\n");
    #endif
  }
  
  TiXmlElement *ordermenu = ws->referenceElement->FirstChildElement("ordermenu");
  if (ordermenu)
  {
    form_CanvasSetMov(ws->canvas->widget.sub,CAN_MOV_SCROLL);
    // order, decline buttons
    char *text = NULL;
    char tmp[256];
    tr_Object *button = NULL;
    //
    
    buttonAttr.x = 5;
    buttonAttr.y = 5;
    buttonAttr.w = 320;
    buttonAttr.h = 90;
    text = (char*)malloc(128);
    button = tr_ButtonInitEx2(NULL,&buttonAttr,ws->canvas);
    sprintf((char*)text,(char*)langGet(lang,(unsigned char*)"Do you want to order:"));
    tr_SetFontColor(button,0xfa,0xfa,0xfa,240);
    tr_SetFontSize(button,17);
    tr_ChangeTextExPos(button,text,5,20);
    text = (char*)malloc(128);
    if (
      ws->referenceElement->Attribute("name")
      )
    {
      sprintf((char*)text,"%s",
        (char*)langGet(lang,(unsigned char*)ws->referenceElement->Attribute("name"))
        );
      
      // multiline text
      char *textEx 	= NULL;
      char *text2Ex 	= NULL;
      char *tmpEx	= NULL;
      textEx = (char*)malloc(128);
      text2Ex = (char*)malloc(128);
      tmpEx = (char*)malloc(256);
      sprintf((char*)tmpEx,"%s",ws->referenceElement->Attribute("name"));
      if (strlen((char*)tmpEx)<(_BUTTON_LINE)+1)
      {
        tr_ChangeTextExPos(button,tmpEx,0,45);
      }
      else
      {  
        strncpy((char*)textEx,(char*)tmpEx,(_BUTTON_LINE));
        textEx[_BUTTON_LINE] = 0x00;
        strncpy((char*)text2Ex,(char*)tmpEx+_BUTTON_LINE,strlen((char*)tmpEx)-(_BUTTON_LINE));
        text2Ex[strlen((char*)tmpEx)-(_BUTTON_LINE)] = 0x00;
      
        {
          tr_ChangeTextExPos(button,textEx,0,45);
          tr_ChangeTextExPos(button,text2Ex,0,68);
        }
      }
    }
    //tr_ChangeTextExPos(button,text,5,40);
    
    text = (char*)malloc(128);
    if (
      ws->referenceElement->Attribute("price")
      )
    {
      sprintf((char*)text,"%s",
        (char*)langGet(lang,(unsigned char*)ws->referenceElement->Attribute("price"))
        );
    }
    struct gui_Widget *w = button->widget.sub;
    tr_ChangeTextExPos(button,text,w->attr.w-60,25); // 250,25
    tr_CanvasAddObjStatic(ws->canvas,button);
    //
    buttonAttr.x = 30;
    buttonAttr.y = 120;
    buttonAttr.w = 230;
    buttonAttr.h = 70;
    button = tr_ButtonInitEx2(NULL,&buttonAttr,ws->canvas);
    tr_SetRound(button,2);
    tr_SetColor(button,(char*)"#20fa20");
    text = (char*)malloc(256);
    sprintf((char*)text,"%s",(char*)langGet(lang,(unsigned char*)"Order"));
    tr_SetFontColor(button,0xfa,0xfa,0xfa,240);
    tr_SetFontSize(button,22);
    tr_ChangeTextExPos(button,text,75,35);
    tr_SetAction(button,onHierarchyOrder,TR_ACTION_BUTTON);
    tr_CanvasAddObjStatic(ws->canvas,button);
    buttonAttr.x = 30;
    buttonAttr.y = 260;
    buttonAttr.w = 230;
    buttonAttr.h = 70;
    button = tr_ButtonInitEx2(NULL,&buttonAttr,ws->canvas);
    tr_SetRound(button,2);
    tr_SetColor(button,(char*)"#fa2020");
    text = (char*)malloc(256);
    sprintf((char*)text,"%s",(char*)langGet(lang,(unsigned char*)"Cancel"));
    tr_SetFontColor(button,0xfa,0xfa,0xfa,240);
    tr_SetFontSize(button,22);
    tr_ChangeTextExPos(button,text,75,35);
    tr_SetAction(button,onHierarchyCancel,TR_ACTION_BUTTON);
    tr_CanvasAddObjStatic(ws->canvas,button);
  }
  
  TiXmlElement *mika = ws->referenceElement->FirstChildElement("mika");
  if (mika)
  {
    struct tr_Object *button = NULL;
    struct tr_Object *image = NULL;
    buttonAttr.y = menuAttr.h-70;
    buttonAttr.w = 60;
    buttonAttr.h = 60;
    button = tr_ButtonInitEx2(NULL,&buttonAttr,ws->canvas);
    tr_SetRound(button,2);
    tr_SetColor(button,(char*)"#fafafa");
    //tr_SetAction(button,onHierarchyBack,TR_ACTION_BUTTON);
    tr_CanvasAddObjStatic(ws->canvas,button);
    image = tr_ImageInitEx(NULL,&buttonAttr,(char*)"img/back.jpg");
    tr_AddImage(button,image);
    tr_ImagePosCenter(image,button);
  }
  
  return 0;
}

/** generates a screen hierarchy (canvas hierarchy)
 */
struct windowSetup *generateScreens( TiXmlElement *root, windowSetup *rootws )
{
  #ifdef _DEBUG
    printf("generateScreens\n");
  #endif
  
  struct windowSetup *sub = NULL;
  struct tr_Object *button = NULL;
  char *text = NULL;
  
  if (!root && !rootws)
  {
    #ifdef _DEBUG
      printf("generateScreens !root & !rootws\n");
    #endif
    root = setupParserGetScreenNamed(parser,setup,(unsigned char*)"main");
    if (root)
    {
      tr_WindowToTop(wmain);
      rootws = initScreenFromElement(root);
      rootws->referenceElement = root;
      rootws->parent = NULL;
      windowHierarchy = rootws;
      #ifdef _DEBUG
        printf("generateScreens !root setting windowHierarchy ROOT\n");
      #endif
    }
    else
    {
      #ifdef _DEBUG
        printf("generateScreens ERROR main screen not present\n");
      #endif
      return NULL;
    }
  }
  
  int itemCount = setupParserScreenGetItemCount(parser,setup,root);
  #ifdef _DEBUG
    printf("generateScreens items(%d)\n",itemCount);
  #endif
  
  if (itemCount==0)
  {
    #ifdef _DEBUG
      printf("generateScreens generate Item menu\n");
    #endif
    rootws->referenceElement = root;
    setupItemScreen(rootws);
  }
  else
  {
    int n;
    TiXmlElement *item = NULL;  
    for(n = 0; n < itemCount; n++)
    {
      tr_WindowToTop(rootws->window);
    
      item = setupParserScreenGetItemAtIndex(parser,setup,root,n);
      if (item->Attribute("name"))
      {
        #ifdef _DEBUG
          printf("generateScreens screen named(%s)\n",(char*)item->Attribute("name"));
        #endif
      }
      // generate the button trigger for the subscreen
      buttonAttr.x = 0;
      buttonAttr.y = 0;
      buttonAttr.w = mainAttr.w-125; // 375
      buttonAttr.h = 70;
      button = tr_ButtonInitEx2(NULL,&buttonAttr,rootws->canvas); 
      tr_SetColor(button,(char*)"#292929");
      tr_SetFontColor(button,0xfa,0xfa,0xfa,240);
      tr_CanvasAddObj(rootws->canvas,button);
      setupButton(button,item);
      
      sub = initScreenFromElement(item);
      #ifdef _DEBUG
        printf("generateScreens after initScreenFromElements\n");
      #endif
      sub->referenceElement = item;
      sub->trigger = button->widget.sub;
      sub->parent = rootws;
      #ifdef _DEBUG
        printf("generateScreens after initScreenFromElements02\n");
      #endif
      windowSetupAddScreen(rootws,sub);
    }
    #ifdef _DEBUG
      printf("generateScreens after processing items\n");
    #endif
  }
  
  tr_WindowToTop(rootws->window);
    
  return rootws;
}

/** generate the main menu and the setup objects
 */
int generateMainMenu( )
{
  struct tr_Object *button = NULL;
  struct tr_Object *image = NULL;
  struct tr_Attr buttonAttr;
  char *text;
  
  tr_WindowToTop(wmain);
  buttonAttr.x = 0;
  buttonAttr.y = 0;
  buttonAttr.w = 320;
  buttonAttr.h = 55;
  button = tr_ButtonInitEx2(NULL,&buttonAttr,cmain);
  tr_SetRound(button,2);
  tr_SetColor(button,(char*)"#000000");
  tr_CanvasAddObj(cmain,button);
  image = tr_ImageInitEx(NULL,&buttonAttr,(char*)"olympicImages/logo.jpg");
  tr_ImageSetRound(image,2);
  tr_AddImage(button,image);
  tr_ImagePosCenter(image,button);
  
  buttonAttr.x = 0;
  buttonAttr.y = 0;
  buttonAttr.w = 320;
  buttonAttr.h = 80;
  button = tr_ButtonInitEx2(NULL,&buttonAttr,cmain);
  tr_SetRound(button,2);
  tr_SetColor(button,(char*)"#404040");
  tr_CanvasAddObj(cmain,button);
  tr_SetAction(button,onMainmenu,TR_ACTION_BUTTON);
  tr_SetFontSize(button,17);
  text = (char*)malloc(256);
  sprintf((char*)text,"Drink Menu");
  tr_ChangeTextExPos(button,text,120,35);
  
  tr_WindowToTop(wmainmenu);
  TiXmlElement *screen = setupParserGetScreenNamed(parser,setup,(unsigned char*)"main");
  if (screen)
  {
    int itemCount = setupParserScreenGetItemCount(parser,setup,screen);
    int n;
    TiXmlElement *item = NULL;
    
    for(n = 0; n < itemCount; n++)
    {
      buttonAttr.x = 0;
      buttonAttr.y = 0;
      buttonAttr.w = 320;
      buttonAttr.h = 100;
      tr_WindowToTop(wmainmenu);
      item = setupParserScreenGetItemAtIndex(parser,setup,screen,n);
      if (!item)
        continue;
      button = tr_ButtonInitEx2(NULL,&buttonAttr,cmainmenu);
      tr_SetRound(button,2);
      tr_SetColor(button,(char*)"#303030");
      tr_SetFontColor(button,0xfa,0xfa,0xfa,240);
      tr_SetFontSize(button,17);
      tr_SetAction(button,onMainmenuItem,TR_ACTION_BUTTON);
      tr_CanvasAddObj(cmainmenu,button);
      text = (char*)malloc(256);
      sprintf((char*)text,"%s",item->Attribute("name"));
      tr_ChangeTextExPos(button,text,120,60);
      if (item->Attribute("image"))
      {
        image = tr_ImageInitEx(NULL,&buttonAttr,(char*)item->Attribute("image"));
        if (image)
        {
          tr_ImageSetRound(image,2);
          tr_AddImage(button,image);
          tr_ImagePosLeft(image,button);
        }
      }
      
      // create a setup for the screen itself
      struct windowSetup *ws = windowSetupNewWindow();
      tr_Window *window = tr_WindowInit(NULL,root,&menuAttr);
      tr_WindowToTop(window);
      tr_Object *canvas = tr_CanvasInit(NULL,&menuAttr,window);
      ws->window = window;
      ws->canvas = canvas;
      ws->trigger = button->widget.sub;
      tr_SetRound(canvas,3);
      tr_SetColor(canvas,(char*)"#000000");
      buttonAttr.x = menuAttr.w-100;
      form_CanvasSetMov(canvas->widget.sub,CAN_MOV_SCROLL);
      buttonAttr.y = menuAttr.h-100;
      buttonAttr.w = 60;
      buttonAttr.h = 60;
      button = tr_ButtonInitEx2(NULL,&buttonAttr,canvas);
      tr_SetRound(button,2);
      tr_SetColor(button,(char*)"#fafafa");
      tr_SetAction(button,onMainmenu,TR_ACTION_BUTTON);
      tr_CanvasAddObjStatic(canvas,button);
      image = tr_ImageInitEx(NULL,&buttonAttr,(char*)"img/back.jpg");
      tr_AddImage(button,image);
      tr_ImagePosCenter(image,button);
      // create dynamic items of this screen
      TiXmlElement *subscreen = setupParserGetScreenNamed(parser,setup,
        (unsigned char*)item->Attribute((char*)"name"));
      #ifdef _DEBUG
        printf("generateMainMenu fetching menu(%s)\n",item->Attribute((char*)"name"));
      #endif
      if (subscreen)
      {
        int subitemCount = setupParserScreenGetItemCount(parser,setup,subscreen);
        int subn;
        TiXmlElement *subitem = NULL;
        for(subn = 0; subn < subitemCount; subn++)
        {
          subitem = setupParserScreenGetItemAtIndex(parser,setup,subscreen,subn);
          #ifdef _DEBUG
            printf("generateMainMenu submenu(%s) item\n",
              subitem->Attribute("name"));
          #endif
          buttonAttr.x = 0;
          buttonAttr.y = 0;
          buttonAttr.w = 320;
          buttonAttr.h = 100;
          
          button = tr_ButtonInitEx2(NULL,&buttonAttr,canvas);
          tr_SetRound(button,2);
          tr_SetColor(button,(char*)"#303030");
          tr_SetFontColor(button,0xfa,0xfa,0xfa,240);
          tr_SetFontSize(button,17);
          tr_SetAction(button,onMenuItem,TR_ACTION_BUTTON);
          
          tr_CanvasAddObj(canvas,button);
          text = (char*)malloc(256);
          sprintf((char*)text,"%s",subitem->Attribute("name"));
          tr_ChangeTextExPos(button,text,50,60);
          text = (char*)malloc(256);
          sprintf((char*)text,"%s",subitem->Attribute("price"));
          tr_ChangeTextExPos(button,text,260,60);
        
          struct itemSetup *is = windowSetupAddItem(ws,button);
          if (!is)
          {
            #ifdef _DEBUG
              printf("generateMainMenu WARNING couldn't setup submenu item\n");
            #endif
          }
          else
          {
            is->xmlObject = subitem;
          }
        }
      }
      else
      {
        #ifdef _DEBUG
          printf("generateMainMenu no subscreen (%s) found\n",
            item->Attribute("name"));
        #endif
      }
    }
    tr_WindowToTop(wmainmenu);
    buttonAttr.x = menuAttr.w-90;
    buttonAttr.y = menuAttr.h-90;
    buttonAttr.w = 60;
    buttonAttr.h = 60;
    button = tr_ButtonInitEx2(NULL,&buttonAttr,cmainmenu);
    tr_SetRound(button,2);
    tr_SetColor(button,(char*)"#fafafa");
    tr_SetAction(button,onMain,TR_ACTION_BUTTON);
    tr_CanvasAddObjStatic(cmainmenu,button);
    image = tr_ImageInitEx(NULL,&buttonAttr,(char*)"img/back.jpg");
    tr_AddImage(button,image);
    tr_ImagePosCenter(image,button);
  }
  else
  {
    #ifdef _DEBUG
      printf("generateMainMenu ERROR couldn't fetch main menu object\n");
    #endif
  }
  return 0;
}

static int posterid = 1;

void *onPosterUP(void*data)
{
  struct tr_Object *image = NULL;
  struct tr_Attr buttonAttr;
  unsigned char name[128];
  
  buttonAttr.x = 0;
  buttonAttr.y = 0;
  buttonAttr.w = 282;
  buttonAttr.h = 395;
  sprintf((char*)name,"img/presentationPoster00%d.jpg\n",posterid);
  image = tr_ImageInitEx(NULL,&buttonAttr,(char*)name);
  //tr_AddImage(poster,image);
  //tr_ImagePosCenter(image,poster);
  tr_WindowExpose(wmain);
  
  if (posterid==1)
    posterid=3;
  else
    posterid--;
}

void *onPosterDOWN(void*data)
{
  struct tr_Object *image = NULL;
  unsigned char name[128];
  
  buttonAttr.x = 0;
  buttonAttr.y = 0;
  buttonAttr.w = 282;
  buttonAttr.h = 395;
  sprintf((char*)name,"img/presentationPoster00%d.jpg\n",posterid);
  image = tr_ImageInitEx(NULL,&buttonAttr,(char*)name);
  //tr_AddImage(poster,image);
  //tr_ImagePosCenter(image,poster);
  tr_WindowExpose(wmain);
  
  if (posterid==3)
    posterid=1;
  else
    posterid++;
}

int main( )
{
  osInit();
  trop_Init();
  gui_Init();
  tr_WmInit();
  
  basicConfig = basicInit(NULL);
  
  layout = NULL;
  rotation = ROT_NONE;
  
  mainAttr.x = 0;
  mainAttr.y = 0;
  mainAttr.w = _WINDOW_SIZE; // 455
  mainAttr.h = _WINDOW_SIZE;
  
  menuAttr.x = 0;
  menuAttr.y = 0;
  menuAttr.w = mainAttr.w;
  menuAttr.h = mainAttr.h;
  
  titleAttr.x = 0;
  titleAttr.y = 0;
  titleAttr.w = mainAttr.w;
  titleAttr.h = menuAttr.y;
  
  root 		= tr_WindowInit(NULL,root,&mainAttr);
  wmain 	= tr_WindowInit(NULL,root,&mainAttr);
  wmainmenu	= tr_WindowInit(NULL,root,&menuAttr);
  wordermenu	= tr_WindowInit(NULL,root,&menuAttr);
  wconfirm	= tr_WindowInit(NULL,root,&menuAttr);
  wmika	= tr_WindowInit(NULL,root,&menuAttr);
  
  tr_WindowMap(wmain);
  tr_WindowToTop(wmain);
  cmain 	= tr_CanvasInit(NULL,&mainAttr,wmain);
  
  struct tr_Object *button;
  struct tr_Object *image;
  char *text = NULL;
  tr_WindowToTop(wordermenu);
  cordermenu	= tr_CanvasInit(NULL,&menuAttr,wordermenu);
  tr_SetRound(cordermenu,3);
  tr_SetColor(cordermenu,(char*)"#000000");
  form_CanvasSetMov(cordermenu->widget.sub,CAN_MOV_NONE);
  buttonAttr.x = menuAttr.w-70;
  buttonAttr.y = menuAttr.h-70;
  buttonAttr.w = 60;
  buttonAttr.h = 60;
  button = tr_ButtonInitEx2(NULL,&buttonAttr,cordermenu);
  tr_SetRound(button,2);
  tr_SetColor(button,(char*)"#fafafa");
  tr_SetAction(button,onMain,TR_ACTION_BUTTON);
  tr_CanvasAddObjStatic(cordermenu,button);
  image = tr_ImageInitEx(NULL,&buttonAttr,(char*)"img/back.jpg");
  tr_AddImage(button,image);
  tr_ImagePosCenter(image,button);
  // order, decline buttons
  unsigned char *tmp = NULL;
  buttonAttr.x = 30;
  buttonAttr.y = 30;
  buttonAttr.w = 230;
  buttonAttr.h = 100;
  button = tr_ButtonInitEx2(NULL,&buttonAttr,cordermenu);
  tr_SetRound(button,2);
  tr_SetColor(button,(char*)"#20fa20");
  text = (char*)malloc(256);
  tmp = langGet(lang,(unsigned char*)"Order");
  sprintf((char*)text,(char*)tmp);
  
  tr_SetFontColor(button,0xfa,0xfa,0xfa,240);
  tr_SetFontSize(button,22);
  tr_ChangeTextExPos(button,text,75,50);
  tr_SetAction(button,onOrder,TR_ACTION_BUTTON);
  tr_CanvasAddObjStatic(cordermenu,button);
  buttonAttr.x = 30;
  buttonAttr.y = 190;
  buttonAttr.w = 230;
  buttonAttr.h = 100;
  button = tr_ButtonInitEx2(NULL,&buttonAttr,cordermenu);
  tr_SetRound(button,2);
  tr_SetColor(button,(char*)"#fa2020");
  text = (char*)malloc(256);
  tmp = langGet(lang,(unsigned char*)"Cancel");
  sprintf((char*)text,(char*)tmp);
  
  tr_SetFontColor(button,0xfa,0xfa,0xfa,240);
  tr_SetFontSize(button,22);
  tr_ChangeTextExPos(button,text,75,50);
  tr_SetAction(button,onCancel,TR_ACTION_BUTTON);
  tr_CanvasAddObjStatic(cordermenu,button);
  
  tr_WindowToTop(wmika); // about mika screen
  cmika 	= tr_CanvasInit(NULL,&mainAttr,wmika);
  form_CanvasSetMov(cmika->widget.sub,CAN_MOV_NONE);
  tr_SetColor(cmika,(char*)"#000000");
  tr_SetRound(cmika,3);
  buttonAttr.x = mainAttr.w-90;
  buttonAttr.y = mainAttr.h-90;
  buttonAttr.w = 60;
  buttonAttr.h = 60;
  button = tr_ButtonInitEx2(NULL,&buttonAttr,cmika);
  tr_SetAction(button,onMain,TR_ACTION_BUTTON);
  tr_SetRound(button,2);
  tr_SetColor(button,(char*)"#fafafa");
  tr_CanvasAddObjStatic(cmika,button);
  image = tr_ImageInitEx(NULL,&buttonAttr,(char*)"img/back.jpg");
  tr_AddImage(button,image);
  tr_ImagePosCenter(image,button);
  // Mika poster
  buttonAttr.x = 0;
  buttonAttr.y = 0;
  buttonAttr.w = 282;
  buttonAttr.h = 395;
  poster = tr_ButtonInitEx2(NULL,&buttonAttr,cmika);
  tr_SetRound(poster,2);
  tr_SetColor(poster,(char*)"#101010");
  tr_CanvasAddObjStatic(cmika,poster);
  image = tr_ImageInitEx(NULL,&buttonAttr,(char*)"img/presentationPoster001.jpg");
  tr_AddImage(poster,image);
  tr_ImagePosCenter(image,poster);
  
  buttonAttr.x = mainAttr.w-90;
  buttonAttr.y = (mainAttr.h/2)-(DEFA_CSMARRL+10)+40;
  buttonAttr.w = 70;
  buttonAttr.h = 70;
  image = tr_ImageInitEx(NULL,&buttonAttr,(char*)"img/bup.jpg");
  button = tr_ButtonInitEx2(NULL,&buttonAttr,cmika);
  tr_SetAction(button,onPosterUP,TR_ACTION_BUTTON);
  tr_SetColor(button,(char*)"#000000");
  tr_CanvasAddObjStatic(cmika,button);
  tr_AddImage(button,image);
  tr_ImagePosCenter(image,button);
  //
  buttonAttr.x = mainAttr.w-90;
  buttonAttr.y = (mainAttr.h/2)+10;
  buttonAttr.w = 70;
  buttonAttr.h = 70;
  image = tr_ImageInitEx(NULL,&buttonAttr,(char*)"img/bdown.jpg");
  button = tr_ButtonInitEx2(NULL,&buttonAttr,cmika);
  tr_SetAction(button,onPosterDOWN,TR_ACTION_BUTTON);
  tr_SetColor(button,(char*)"#000000");
  tr_CanvasAddObjStatic(cmika,button);
  tr_AddImage(button,image);
  tr_ImagePosCenter(image,button);
  
  tr_WindowToTop(wconfirm); // confirmation screen
  cconfirm	= tr_CanvasInit(NULL,&menuAttr,wconfirm);
  form_CanvasSetMov(cconfirm->widget.sub,CAN_MOV_NONE);
  tr_SetColor(cconfirm,(char*)"#000000");
  tr_SetRound(cconfirm,3);
  //
  buttonAttr.x = menuAttr.w-90;
  buttonAttr.y = menuAttr.h-90;
  buttonAttr.w = 60;
  buttonAttr.h = 60;
  button = tr_ButtonInitEx2(NULL,&buttonAttr,cconfirm);
  tr_SetRound(button,2);
  tr_SetColor(button,(char*)"#fafafa");
  tr_SetAction(button,onConfirmBack,TR_ACTION_BUTTON);
  tr_CanvasAddObjStatic(cconfirm,button);
  image = tr_ImageInitEx(NULL,&buttonAttr,(char*)"img/back.jpg");
  tr_AddImage(button,image);
  tr_ImagePosCenter(image,button);
  
  tr_WindowToTop(wmainmenu);
  cmainmenu	= tr_CanvasInit(NULL,&menuAttr,wmainmenu);
   
  tr_SetRound(cmain,3);
  tr_SetColor(cmain,(char*)"#000000");
  form_CanvasSetMov(cmain->widget.sub,CAN_MOV_NONE);

  tr_SetRound(cmainmenu,3);
  tr_SetColor(cmainmenu,(char*)"#000000");
  form_CanvasSetMov(cmainmenu->widget.sub,CAN_MOV_SCROLL);

  int n;
  int order = 0;
  buttonAttr.x = 0;
  buttonAttr.y = 0;
  buttonAttr.w = 420;
  buttonAttr.h = 70;
  
  struct tr_Attr imageAttr;
  imageAttr.x = 0;
  imageAttr.y = 0;
  imageAttr.w = 420;
  imageAttr.h = 70;
  
  #ifdef _DEBUG
    printf("activity object handling at sushi\n");
  #endif
  actObjects = activityObjectsClone();
  if (actObjects)
  {
    if ( activityObjectRegister( actObjects,(unsigned char*)"anglesushi" ) != 0 )
    {
      #ifdef _DEBUG
        printf("activity object could not be created at anglesushi\n");
      #endif
    }
  
    if ( activityObjectRegister( actObjects,(unsigned char*)"sushi" ) != 0 )
    {
      #ifdef _DEBUG
        printf("activity object could not be created at sushi\n");
      #endif
    }
    else
    {
      struct activityObject oSushi;
      activityObjectClone(actObjects,(unsigned char*)"sushi",&oSushi);
      oSushi.fastData[0] = 1;
      #ifdef _DEBUG
        printf("activity object sushi cloned name(%s)\n",(char*)oSushi.name);
      #endif
      activityObjectSet(actObjects,(unsigned char*)"sushi",&oSushi);
    }
  }
  
  #ifndef _CONNECTED
    pthread_t t;
    pthread_create(&t,NULL,_poll_visibility,NULL);
  #endif
  
  /**
  struct tr_Attr lbattr;
  lbattr.x = 10;
  lbattr.y = 40;
  lbattr.w = 330;
  lbattr.h = 300;
  listbox = tr_ListboxInit(NULL,&lbattr,wmain);
  tr_SetColor(listbox,(char*)"#e0e0e0");
  tr_CanvasAddObjStatic(cmain,listbox);
  form_ListboxSetCanvasDelegate(listbox->widget.sub,cmain->widget.sub);
  */
  
  statusAttr.x = 10;
  statusAttr.y = mainAttr.h-27;
  statusAttr.w = 320;
  statusAttr.h = 27;
  status = tr_ButtonInitEx2(NULL,&statusAttr,cmain);
  tr_SetColor(status,"#000000");
  tr_CanvasAddObjStatic(cmain,status);
  
  unsigned char setupServer[256];
  sprintf((char*)setupServer,"ftp://%s",basicGet(basicConfig,(unsigned char*)"server"));
  setup = networkSetupInit(NULL);
  strcpy((char*)setup->server,(char*)setupServer);
  strcpy((char*)setup->path,"");
  strcpy((char*)setup->username,"anonymous");
  strcpy((char*)setup->password,"secret");
  setup->port = 21;
  networkSetupDownloadSetup(setup);
  
  parser = setupParserInit(NULL,setup);
  TiXmlElement *screen = setupParserGetScreenNamed(parser,setup,(unsigned char*)"main");
  if (screen)
  {
    int itemCount = setupParserScreenGetItemCount(parser,setup,screen);
    int n;
    TiXmlElement *item = NULL;
    for(n = 0; n < itemCount; n++)
    {
      item = setupParserScreenGetItemAtIndex(parser,setup,screen,n);
      if (item)
      {
        #ifdef _DEBUG
          printf("olympic item(%d,%s) at screen(main)\n",n,item->Attribute("name"));
        #endif
      }
    }
  }
  
  lang = langSetupInit(NULL,parser);
  
  pos = posSetupInit(NULL);
  if (pos)
  {
    if ( posSetupConfigure(pos,parser,setup) != 0 )
    {
      #ifdef _DEBUG
        printf("olympic pos error, unable to retrieve posSetup\n");
      #endif
    }
    else
    {
      #ifdef _DEBUG
        printf("olympic pos server(%s) port(%d)\n",(char*)pos->server,pos->port);
      #endif
    }
  }
  
  mikaPosClientInit(&client);
  strcpy((char*)client.server,(char*)pos->server);
  client.port = pos->port;
  
  #ifdef _SUSHI_CONNECTION_WATCHDOG
    struct connectionWatchdogDelegate *delegate = (struct connectionWatchdogDelegate*)malloc(sizeof(struct connectionWatchdogDelegate));
    delegate->onConnectionStatus = _on_connection_status;
    struct connectionWatchdog *watchdog = connectionWatchdogInit(NULL);
    if (watchdog)
    {
      strcpy((char*)watchdog->server,"http://www.mika.com");
      watchdog->delegate = delegate;
      connectionWatchdogRun(watchdog);
    }
  #endif
  
  pthread_t post;
  pthread_create(&post,NULL,postinitSetup,NULL);
  
  actObjects = activityObjectsClone();
  if (actObjects)
  {
    if ( activityObjectRegister(actObjects,(unsigned char*)"olympic") != 0 )
    {
      #ifdef _DEBUG
        printf("olympic activityObjectRegister ERROR couldn't register activity\n");
      #endif
    }
    else
    {
      #ifdef _DEBUG
        printf("olympic activityObjectRegister OK, setting object data\n");
      #endif
      struct activityObject oOlympic;
      activityObjectClone(actObjects,(unsigned char*)"olympic",&oOlympic);
      oOlympic.fastData[0] = 1;
      activityObjectSet(actObjects,(unsigned char*)"olympic",&oOlympic);
    }
  }
  
  // processing activity object events - events from other apps
  #ifdef _SUSHI_INACTIVITY
    //pthread_t at;
    //pthread_create(&at,NULL,_activity_processing,NULL);
    
    bigInterface->delegate = &interfaceDelegate;
    interfaceDelegate.interfaceOnEvent = interfaceOnEvent;
  
    pthread_t it;
    pthread_create(&it,NULL,_inactivity_thread_ex,NULL);
  #endif
  
  generateScreens(NULL,NULL);
  listScreenHierarchy(windowHierarchy);
  //generateMainMenu();
  tr_WindowToTop(windowHierarchy->window);
  
  //tr_WindowToTop(wmain);
  tr_WindowQueue(root);
  
  return 0;
}
