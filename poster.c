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

struct tr_Window *root = NULL;
struct tr_Window *wmain = NULL;

struct tr_Object *cmain = NULL;

int main( )
{
  struct tr_Attr	mainAttr;
  struct tr_Attr	buttonAttr;
  struct tr_Attr	imageAttr;

  struct tr_Object	*bPoster;
  struct tr_Object	*bimage;
  char			*text;

  interfaceSetName((unsigned char*)"POSTER");

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

  buttonAttr.x = 0;
  buttonAttr.y = 0;
  buttonAttr.w = 340;
  buttonAttr.h = 340;

  imageAttr.x = 0;
  imageAttr.y = 0;
  imageAttr.w = 340;
  imageAttr.h = 340;
  
  root = tr_WindowInit(NULL,root,&mainAttr);
  wmain = tr_WindowInit(NULL,root,&mainAttr);
  
  tr_WindowMap(wmain);
  tr_WindowToTop(wmain);
  
  cmain = tr_CanvasInit(NULL,&mainAttr,wmain);
  tr_SetRound(cmain,3);
  tr_SetColor(cmain,(char*)"#101010");
  form_CanvasSetMov(cmain->widget.sub,CAN_MOV_NONE);
  
  bPoster = tr_ButtonInitEx2(NULL,&buttonAttr,cmain);
  tr_SetRound(bPoster,4);
  tr_SetColor(bPoster,(char*)"#101010");
  tr_CanvasAddObj(cmain,bPoster);
  bimage = tr_ImageInitEx(NULL,&imageAttr,(char*)"img/posterConcept.jpg");
  tr_ImageSetRound(bimage,2);
  tr_AddImage(bPoster,bimage);
  tr_ImagePosCenter(bimage,bPoster);
  
  tr_WindowToTop(wmain);
  tr_WindowQueue(root);
  
  return 0;
}
