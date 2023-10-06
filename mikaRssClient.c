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
  #include "xipc.h"
}
#include "mikaRssClient.h"
#include "setupParser.h"
#include "mikaRssClass.h"
#include "mikaTextfield.h"

struct tr_Window *root		= NULL;
struct tr_Window *wmain		= NULL;
struct tr_Object *cmain		= NULL;
struct tr_Object *widget	= NULL;

struct tr_Object *title		= NULL;
struct tr_Object *descr		= NULL;
struct tr_Object *prev		= NULL;
struct tr_Object *next		= NULL;

struct tr_Attr	mainAttr;
struct tr_Attr	attr;

struct mikaRss	*rss		= NULL;
mikaRssClass	*rssc		= NULL;
mikaRssClient	*client		= NULL;
mikaTextfield 	*textfield	= NULL;

char 		*tmp		= NULL;

mikaRssClient::mikaRssClient( unsigned char *filename )
{
  if (!filename)
  {
    // todo, error handling
    #ifdef _DEBUG
      printf("mikaRssClient::mikaRssClient error invalid arguments\n");
    #endif
  }
  else
  {
    rssc = new mikaRssClass(filename);
    if (rssc)
    {
      unsigned char titlestr[4096];
      unsigned char descriptionstr[4096];
      mrss_item_t *item = NULL;
      item = rssc->mrss->item;
      int n = 0;
      while(item)
      {
        if (n==0)
        {
          strcpy(tmp,(char*)item->title);
          tr_ChangeTextExPos(title,tmp,0,20);
        }
        strcpy((char*)titlestr,(char*)item->title);
        strcpy((char*)descriptionstr,(char*)item->description);
        #ifdef _DEBUG
          printf("mikaRssClient RSS title(%s) description(%s)\n",
            (char*)titlestr,(char*)descriptionstr);
        #endif
        textfield->setText((unsigned char*)descriptionstr);
        item = item->next;
        n++;
      }
    }
  }
}

mikaRssClient::~mikaRssClient( )
{
}

int main(void)
{
  interfaceSetName((unsigned char*)"MIKARSSCLIENT");
  
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
  tr_SetColor(cmain,(char*)"050505");
  form_CanvasSetMov(cmain->widget.sub,CAN_MOV_NONE);
  interfaceSetBackplaneSize(0,0);
  
  attr.x = 5;
  attr.y = 5;
  attr.w = 470;
  attr.h = 40;
  title = tr_ButtonInitEx2(NULL,&attr,cmain);
  tr_SetColor(title,(char*)"#030303");
  tr_SetRound(title,3);
  tr_SetFontColor(title,0xce,0xce,0xce,250);
  tr_SetFontSize(title,16);
  tmp = (char*)malloc(256);
  strcpy(tmp,"test");
  tr_ChangeTextExPos(title,tmp,0,20);
  tr_CanvasAddObjStatic(cmain,title);
  
  // description widget
  attr.x = 5;
  attr.y = 50;
  attr.w = 470;
  attr.h = 320;
  descr = tr_ButtonInitEx2(NULL,&attr,cmain);
  tr_SetColor(descr,(char*)"#030303");
  tr_SetRound(descr,3);
  tr_SetFontColor(descr,0xce,0xce,0xce,250);
  tr_SetFontSize(descr,12);
  tr_CanvasAddObjStatic(cmain,descr);
  
  textfield = new mikaTextfield(cmain,descr,25);
  if (textfield)
  {
  //  textfield->setText((unsigned char*)"test");
  }
  
  attr.x = 10;
  attr.y = 400;
  attr.w = 220;
  attr.h = 70;
  prev = tr_ButtonInitEx2(NULL,&attr,cmain);
  tr_SetColor(prev,(char*)"#404040");
  tr_SetRound(prev,3);
  tr_SetFontColor(prev,0xce,0xce,0xce,250);
  tr_SetFontSize(prev,12);
  tr_CanvasAddObjStatic(cmain,prev);
  
  attr.x = 250;
  next = tr_ButtonInitEx2(NULL,&attr,cmain);
  tr_SetColor(next,(char*)"#404040");
  tr_SetRound(next,3);
  tr_SetFontColor(next,0xce,0xce,0xce,250);
  tr_SetFontSize(next,12);
  tr_CanvasAddObjStatic(cmain,next);
  
  // RSS logo
  attr.x = 400;
  attr.y = 0;
  attr.w = 70;
  attr.h = 40;
  tr_Object *iholder = tr_ButtonInitEx2(NULL,&attr,cmain);
  tr_Object *image = tr_ImageInitEx(NULL,&attr,(char*)"img/rsslogo02.jpg");
  tr_AddImage(iholder,image);
  tr_ImagePosCenter(image,iholder);
  tr_CanvasAddObjStatic(cmain,iholder);
  
  client = new mikaRssClient((unsigned char*)"rss04.rss");
  
  tr_WindowQueue(root);
  
  return 0;
}
