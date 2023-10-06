#include "mikaPanel.h"
#include "mikaSkin.h"
#include "config.h"

void *mikaPanelItemOnButton( void *data )
{
  #ifdef _DEBUG
    printf("mikaPanelItemOnButton\n");
  #endif
  return NULL;
}

mikaPanelItem::mikaPanelItem( struct tr_Object *canvas, struct tr_Attr *attr )
{
  next 		= NULL;
  widget	= tr_ButtonInitEx2(NULL,attr,canvas);
  //tr_SetColor(widget,(char*)"#101010");
  tr_SetRound(widget,2);
  tr_SetFontColor(widget,0xff,0xff,0xff,240);
  tr_SetFontSize(widget,20);
  tr_SetAction(widget,mikaPanelItemOnButton,TR_ACTION_BUTTON);
}

mikaPanelItem::~mikaPanelItem( )
{
}

int mikaPanelItem::setName( unsigned char *name )
{
  if (!name)
  {
    #ifdef _DEBUG
      printf("mikaPanelItem::setName error, invalid arguments\n");
    #endif
    return 1;
  }
  
  char *txt = (char*)malloc(strlen((char*)name)+1);
  strcpy((char*)txt,(char*)name);
  tr_SetTextEx(widget,(char*)txt,PLACE_CENTER);
  
  return 0;
}

void *mikaPanelOnButton( void *data )
{
  #ifdef _DEBUG
    printf("mikaPanelOnButton\n");
  #endif
  return NULL;
}

mikaPanel::mikaPanel( struct tr_Object *canvas, struct tr_Attr *attr )
{
  items 	= NULL;
  widget	= tr_ButtonInitEx2(NULL,attr,canvas);
  tr_SetColor(widget,(char*)"#050505");
  tr_SetRound(widget,2);
  // regression - causes synchronisation problems
  if (1) tr_SetAction(widget,mikaPanelOnButton,TR_ACTION_BUTTON);
  tr_CanvasAddObjStatic(canvas,widget);
}

mikaPanel::~mikaPanel( )
{
}

int mikaPanel::addItem( mikaPanelItem *item )
{
  item->next = items;
  items = item;
  tr_PutObject(widget,item->widget);
}
