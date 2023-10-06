#include "system.h"

#include "sample.xpm"

void *thread(void*data)
{
  printf("@thread\n");
}

void *onclick(void*data)
{
  printf(":clickproc\n");
  struct gui_Event *e = (struct gui_Event*)data;
  struct gui_Widget *w = (struct gui_Widget*)e->widget;
  printf("\t:%d %d\n",e->x,e->y);
}

void *buttonclick00(void*data)
{
  printf(":clickproc button00\n");
  struct gui_Event *e = (struct gui_Event*)data;
  struct gui_Widget *w = (struct gui_Widget*)e->widget;
  printf("\t:%d %d\n",e->x,e->y);
}

void *onmotion(void*data)
{
  printf(":onmotion\n");
}

void *task(void*data)
{
  printf("\t:task\n");
}

int main( )
{
  trop_Init();
  
  struct trop_Queue *q = trop_MsgQCreate( );
  struct trop_Msg m;
  m.id = 0;
  m.time = time(NULL);
  m.data = NULL;
  trop_MsgSend(q,&m);

  gui_Widget w, w2, w3;
  gui_WidgetInitAttr(&w);
  gui_WidgetInitAttr(&w2);
  gui_WidgetInitAttr(&w3);
  w.attr.x = 0;
  w.attr.y = 0;
  w.attr.w = 320;
  w.attr.h = 240;
  w.attr.form = -1;
  gui_WidgetInit(&w,NULL);
  w.attr.onmotion = onmotion;
  //gui_WidgetSetColor(&w,"purple4");
  gui_WidgetMap(&w);

  struct gui_Text text;
  text.str = "hello";
  text.len = 5;
  text.pos.x = 40;
  text.pos.y = 40;
  text.font = NULL;
  
  gui_WidgetAddText(&w,&text);
  
  w2.attr.x = 60;
  w2.attr.y = 60;
  w2.attr.h = 60;
  w2.attr.w = 60;
  w2.attr.form = FORM_NONE;
  gui_WidgetInit(&w2,&w);
  w2.attr.onclick = onclick;
  gui_WidgetMap(&w2);
  
  w3.attr.x = 100;
  w3.attr.y = 10;
  w3.attr.h = 40;
  w3.attr.w = 100;
  w3.attr.form = FORM_NONE;
  gui_WidgetInit(&w3,&w);
  w3.attr.onclick = onclick;
  gui_WidgetMap(&w3);
  gui_WidgetSetColor(&w3,"#ffffff");
  
  struct gui_Widget *w4 = gui_NewWidget();
  w4->attr.x = 0;
  w4->attr.y = 100;
  w4->attr.h = 120;
  w4->attr.w = 40;
  w4->attr.form = FORM_MENU;
  gui_WidgetInit(w4,&w);
  gui_WidgetMap(w4);
  gui_WidgetSetColor(w4,"darkred");

  struct gui_Image icon;
  strcpy((char*)icon.filename,"icon.xpm");
  gui_CreateImage(&w,&icon);
  gui_WidgetAddImage(&w,&icon);
  gui_WidgetAddImage(&w2,&icon);

  struct gui_Widget *b00 = gui_NewWidget();
  b00->attr.w = 40;
  b00->attr.h = 40;
  b00->attr.form = FORM_BUTTON;
  b00->attr.onclick = buttonclick00;
  gui_WidgetInit(b00,w4);
  gui_WidgetMap(b00);
  gui_WidgetSetColor(b00,"purple");

  struct gui_Widget *b01 = gui_NewWidget();
  b01->attr.w = 40;
  b01->attr.h = 40;
  b01->attr.form = FORM_BUTTON;
  gui_WidgetInit(b01,w4);
  gui_WidgetMap(b01);
  gui_WidgetSetColor(b01,"black");
  gui_WidgetAddImage(b01,&icon);

  form_MenuAddButton(w4,b00);
  form_MenuAddButton(w4,b01);
  
  struct gui_Image *back = (struct gui_Image*)malloc(sizeof(gui_Image));
  strcpy((char*)back->filename,"sample.xpm");
  gui_CreateImage(&w,back);
  back->x = 60;
  back->y = 60;
  gui_WidgetAddImage(&w,back);
  gui_WidgetSetColor(&w,"orange2");
  //gui_WidgetAddImage(&w2,back);
  
  //struct gui_Image icon;
  //strcpy((char*)icon.filename,"icon.xpm");
  //gui_CreateImage(&w,&icon);
  //gui_WidgetAddImage(&w,&icon);
  //gui_WidgetAddImage(&w2,&icon);
  
  gui_WidgetPaintRegion(&w,20,20,90,90);

  struct gui_Scene *scene00 = gui_NewScene();
  //scene = scene00;
  //gui_SceneSet(scene00);
  
  gui_WidgetQueue(&w);

  struct trop_Conn con;
  strcpy(con.remote,"localhost");
  con.port = 2501;
  con.bsecure = TRUE;
  trop_ConnInit( &con );

  struct trop_SAtom satom;
  trop_SAtomInit(&satom);
  struct trop_STask stask;
  stask.call = task;
  trop_SAtomAddTask(&satom,&stask,200);

  //struct trop_Atom atom;
  //atom.call = thread;
  //trop_AtomInit(&atom);
  while(1)
  {
  }
  return 0;
}
