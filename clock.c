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
  #include "xipc.h"
}

struct tr_Window *root		= NULL;
struct tr_Window *wmain		= NULL;
struct tr_Object *cmain		= NULL;

struct tr_Attr	mainAttr;

int main(void)
{
  interfaceSetName((unsigned char*)"CLOCK");
  
  osInit();
  trop_Init();
  gui_Init();
  tr_WmInit();
  
  layout = NULL;
  rotation = ROT_NONE;

  mainAttr.x = 0;
  mainAttr.y = 0;
  mainAttr.w = 100;
  mainAttr.h = 100;

  root = tr_WindowInit(NULL,root,&mainAttr);
  wmain = tr_WindowInit(NULL,root,&mainAttr);

  cmain = tr_CanvasInit(NULL,&mainAttr,wmain);
  tr_SetRound(cmain,3);
  tr_SetColor(cmain,(char*)"#000000");

  tr_WindowQueue(root);

  return 0;
}
