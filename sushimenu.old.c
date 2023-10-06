extern "C" {
  #include "interface.h"
  #include "config.h"
  #include "os.h"
  #include "system.h"
  #include "global.h"
  #include "xml.h"
  #include "wm.h"
  #include "dom.h"
}

struct tr_Window *root		= NULL;
struct tr_Window *wmain		= NULL;
struct tr_Window *wsushi	= NULL;
struct tr_Window *wmaki		= NULL;
struct tr_Window *wbento	= NULL;
struct tr_Window *wsoft		= NULL;

struct tr_Object *cmain		= NULL;
struct tr_Object *csushi	= NULL;
struct tr_Object *cmaki		= NULL;
struct tr_Object *cbento	= NULL;
struct tr_Object *csoft		= NULL;

struct tr_Object *button	= NULL;
struct tr_Object *image		= NULL;

struct tr_Attr	mainAttr;
struct tr_Attr	buttonAttr;

void *onBack(void*data)
{
  tr_WindowToTop(wmain);
  tr_WindowExpose(wmain);
}

void *onSushi(void*data)
{
  tr_WindowToTop(wsushi);
  tr_WindowExpose(wsushi);
}

void *onMaki(void*data)
{
  tr_WindowToTop(wmaki);
  tr_WindowExpose(wmaki);
}

void *onBento(void*data)
{
  tr_WindowToTop(wbento);
  tr_WindowExpose(wbento);
}

void *onSoft(void*data)
{
  tr_WindowToTop(wsoft);
  tr_WindowExpose(wsoft);
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
  mainAttr.w = 380;
  mainAttr.h = 380;

  root		= tr_WindowInit(NULL,root,&mainAttr);
  wmain		= tr_WindowInit(NULL,root,&mainAttr);
  wsushi	= tr_WindowInit(NULL,root,&mainAttr);
  wmaki		= tr_WindowInit(NULL,root,&mainAttr);
  wbento	= tr_WindowInit(NULL,root,&mainAttr);
  wsoft		= tr_WindowInit(NULL,root,&mainAttr);

  tr_WindowMap(wmain);
  tr_WindowToTop(wmain);
  cmain		= tr_CanvasInit(NULL,&mainAttr,wmain);
  tr_SetRound(cmain,3);
  tr_SetColor(cmain,(char*)"#fdfdfd");
  form_CanvasSetMov(cmain->widget.sub,CAN_MOV_NONE);
  
  // logo
  buttonAttr.x = 10;
  buttonAttr.y = 280;
  buttonAttr.w = 280;
  buttonAttr.h = 90;
  button = tr_ButtonInitEx2(NULL,&buttonAttr,cmain);
  tr_SetRound(button,2);
  tr_SetColor(button,(char*)"#909090");
  tr_CanvasAddObj(cmain,button);
  image = tr_ImageInitEx(NULL,&buttonAttr,(char*)"img/title.jpg");
  tr_AddImage(button,image);
  tr_ImagePosCenter(image,button);
  
  int n;
  for(n = 0; n < 4; n++)
  {
    buttonAttr.x = 0;
    buttonAttr.y = 0;
    buttonAttr.w = 130;
    buttonAttr.h = 130;
    button = tr_ButtonInitEx2(NULL,&buttonAttr,cmain);
    tr_SetRound(button,2);
    tr_SetColor(button,(char*)"#909090");
    tr_CanvasAddObj(cmain,button);
    switch(n)
    {
      case 0:
        image = tr_ImageInitEx(NULL,&buttonAttr,(char*)"sushiImages/sushi.jpg");
        tr_SetAction(button,onSushi,TR_ACTION_BUTTON);
        break;
      case 1:
        image = tr_ImageInitEx(NULL,&buttonAttr,(char*)"sushiImages/maki.jpg");
        tr_SetAction(button,onMaki,TR_ACTION_BUTTON);
        break;
      case 2:
        image = tr_ImageInitEx(NULL,&buttonAttr,(char*)"sushiImages/bento.jpg");
        tr_SetAction(button,onBento,TR_ACTION_BUTTON);
        break;
      case 3:
        image = tr_ImageInitEx(NULL,&buttonAttr,(char*)"sushiImages/soft.jpg");
        tr_SetAction(button,onSoft,TR_ACTION_BUTTON);
        break;
    }
    tr_ImageSetRound(image,2);
    tr_AddImage(button,image);
    tr_ImagePosCenter(image,button);
  }

  tr_WindowToTop(wsushi);
  csushi	= tr_CanvasInit(NULL,&mainAttr,wsushi);
  tr_SetRound(csushi,3);
  tr_SetColor(csushi,(char*)"#fdfdfd");
  form_CanvasSetMov(csushi->widget.sub,CAN_MOV_SCROLL);
  buttonAttr.x = mainAttr.w-70;
  buttonAttr.y = mainAttr.h-70;
  buttonAttr.w = 60;
  buttonAttr.h = 60;
  button = tr_ButtonInitEx2(NULL,&buttonAttr,csushi);
  tr_SetRound(button,2);
  tr_SetColor(button,(char*)"#ffffff");
  tr_SetAction(button,onBack,TR_ACTION_BUTTON);
  tr_CanvasAddObjStatic(csushi,button);
  image = tr_ImageInitEx(NULL,&buttonAttr,(char*)"img/home_small_new2.jpg");
  tr_AddImage(button,image);
  tr_ImagePosCenter(image,button);
  unsigned char filename[64];
  for(n = 1; n < 5; n++)
  {
    buttonAttr.x = 0;
    buttonAttr.y = 0;
    buttonAttr.w = 250;
    buttonAttr.h = 120;
    button = tr_ButtonInitEx2(NULL,&buttonAttr,csushi);
    tr_SetRound(button,2);
    tr_SetColor(button,(char*)"#ffffff");
    tr_CanvasAddObj(csushi,button);
    sprintf((char*)filename,"sushiImages/sushi00%d.jpg",n);
    image = tr_ImageInitEx(NULL,&buttonAttr,(char*)filename);
    tr_ImageSetRound(image,2);
    tr_AddImage(button,image);
    tr_ImagePosCenter(image,button);
  }

  tr_WindowToTop(wmaki);
  cmaki 	= tr_CanvasInit(NULL,&mainAttr,wmaki);
  tr_SetRound(cmaki,3);
  tr_SetColor(cmaki,(char*)"#fdfdfd");
  form_CanvasSetMov(cmaki->widget.sub,CAN_MOV_SCROLL);
  buttonAttr.x = mainAttr.w-70;
  buttonAttr.y = mainAttr.h-70;
  buttonAttr.w = 60;
  buttonAttr.h = 60;
  button = tr_ButtonInitEx2(NULL,&buttonAttr,cmaki);
  tr_SetRound(button,2);
  tr_SetColor(button,(char*)"#ffffff");
  tr_SetAction(button,onBack,TR_ACTION_BUTTON);
  tr_CanvasAddObjStatic(cmaki,button);
  image = tr_ImageInitEx(NULL,&buttonAttr,(char*)"img/home_small_new2.jpg");
  tr_AddImage(button,image);
  tr_ImagePosCenter(image,button);
  for(n = 1; n < 10; n++)
  {
    buttonAttr.x = 0;
    buttonAttr.y = 0;
    buttonAttr.w = 250;
    buttonAttr.h = 120;
    button = tr_ButtonInitEx2(NULL,&buttonAttr,cmaki);
    tr_SetRound(button,2);
    tr_SetColor(button,(char*)"#ffffff");
    tr_CanvasAddObj(cmaki,button);
    sprintf((char*)filename,"sushiImages/maki00%d.jpg",n);
    image = tr_ImageInitEx(NULL,&buttonAttr,(char*)filename);
    tr_ImageSetRound(image,2);
    tr_AddImage(button,image);
    tr_ImagePosCenter(image,button);
  }

  tr_WindowToTop(wbento);
  cbento 	= tr_CanvasInit(NULL,&mainAttr,wbento);
  tr_SetRound(cbento,3);
  tr_SetColor(cbento,(char*)"#fdfdfd");
  form_CanvasSetMov(cbento->widget.sub,CAN_MOV_SCROLL);
  buttonAttr.x = mainAttr.w-70;
  buttonAttr.y = mainAttr.h-70;
  buttonAttr.w = 60;
  buttonAttr.h = 60;
  button = tr_ButtonInitEx2(NULL,&buttonAttr,cbento);
  tr_SetRound(button,2);
  tr_SetColor(button,(char*)"#ffffff");
  tr_SetAction(button,onBack,TR_ACTION_BUTTON);
  tr_CanvasAddObjStatic(cbento,button);
  image = tr_ImageInitEx(NULL,&buttonAttr,(char*)"img/home_small_new2.jpg");
  tr_AddImage(button,image);
  tr_ImagePosCenter(image,button);
  for(n = 1; n < 3; n++)
  {
    buttonAttr.x = 0;
    buttonAttr.y = 0;
    buttonAttr.w = 250;
    buttonAttr.h = 120;
    button = tr_ButtonInitEx2(NULL,&buttonAttr,cbento);
    tr_SetRound(button,2);
    tr_SetColor(button,(char*)"#ffffff");
    tr_CanvasAddObj(cbento,button);
    sprintf((char*)filename,"sushiImages/bento00%d.jpg",n);
    image = tr_ImageInitEx(NULL,&buttonAttr,(char*)filename);
    tr_ImageSetRound(image,2);
    tr_AddImage(button,image);
    tr_ImagePosCenter(image,button);
  }

  tr_WindowToTop(wsoft);
  csoft 	= tr_CanvasInit(NULL,&mainAttr,wsoft);
  tr_SetRound(csoft,3);
  tr_SetColor(csoft,(char*)"#fdfdfd");
  form_CanvasSetMov(csoft->widget.sub,CAN_MOV_SCROLL);
  buttonAttr.x = mainAttr.w-70;
  buttonAttr.y = mainAttr.h-70;
  buttonAttr.w = 60;
  buttonAttr.h = 60;
  button = tr_ButtonInitEx2(NULL,&buttonAttr,csoft);
  tr_SetRound(button,2);
  tr_SetColor(button,(char*)"#ffffff");
  tr_SetAction(button,onBack,TR_ACTION_BUTTON);
  tr_CanvasAddObjStatic(csoft,button);
  image = tr_ImageInitEx(NULL,&buttonAttr,(char*)"img/home_small_new2.jpg");
  tr_AddImage(button,image);
  tr_ImagePosCenter(image,button);
  for(n = 1; n < 4; n++)
  {
    buttonAttr.x = 0;
    buttonAttr.y = 0;
    buttonAttr.w = 250;
    buttonAttr.h = 120;
    button = tr_ButtonInitEx2(NULL,&buttonAttr,csoft);
    tr_SetRound(button,2);
    tr_SetColor(button,(char*)"#ffffff");
    tr_CanvasAddObj(csoft,button);
    sprintf((char*)filename,"sushiImages/soft00%d.jpg",n);
    image = tr_ImageInitEx(NULL,&buttonAttr,(char*)filename);
    tr_ImageSetRound(image,2);
    tr_AddImage(button,image);
    tr_ImagePosCenter(image,button);
  }

  tr_WindowToTop(wmain);
  tr_WindowQueue(root);

  return 0;
}
