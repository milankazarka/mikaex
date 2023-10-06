#include "mikaGalleryWidget.h"

mikaGalleryItem::mikaGalleryItem( unsigned char *id, int w, int h, int type )
{
  #ifdef _DEBUG
    printf("mikaGalleryItem::mikaGalleryItem\n");
  #endif
  
  prev = NULL;
  next = NULL;
  
  if (id)
  {
    #ifdef _DEBUG
      printf("mikaGalleryImage::mikaGalleryImage id(%s)\n",(char*)id);
    #endif
    strcpy((char*)myId,(char*)id);
    myType = type;
    mw = w;
    mh = h;
    load();
  }
  else
  {
    // error handling
  }
}

mikaGalleryItem::~mikaGalleryItem( )
{
}

int mikaGalleryItem::load( )
{
  switch(myType)
  {
    case _MIKA_GALLERY_IMAGE:
      #ifdef _TR_SYSTEM
        struct tr_Attr attr;
        attr.x = 0;
        attr.y = 0;
        attr.w = mw;
        attr.h = mh;
        image = tr_ImageInitEx(NULL,&attr,(char*)myId);
      #endif
      break;
  }
  return 0;
}

mikaGalleryWidgetButton::mikaGalleryWidgetButton( struct mikaWidgetData *data )
 : mikaWidget(data)
{
 #ifdef _DEBUG
  printf("mikaGalleryWidgetButton::mikaGalleryWidgetButton\n");
 #endif
 gallery = NULL;
}

mikaGalleryWidgetButton::mikaGalleryWidgetButton( struct mikaWidgetData *data, mikaWidget *parent )
 : mikaWidget(data,parent)
{
 #ifdef _DEBUG
  printf("mikaGalleryWidgetButton::mikaGalleryWidgetButton\n");
 #endif
 gallery = NULL;
}

mikaGalleryWidgetButton::~mikaGalleryWidgetButton( )
{
}

void mikaGalleryWidgetButton::onEvent( struct mikaEvent *event )
{
 #ifdef _DEBUG
  printf("mikaGalleryWidgetButton::onEvent\n");
 #endif
 if (gallery)
 {
  mikaGalleryWidget *gw = (mikaGalleryWidget*)gallery;
  gw->onControl(this);
 }
}

void mikaGalleryWidgetButton::onButtonDown( struct mikaEvent *event )
{
}

void mikaGalleryWidgetButton::onButtonUp( struct mikaEvent *event )
{
}

mikaGalleryWidget::mikaGalleryWidget( struct mikaWidgetData *data)
  : mikaWidget(data)
{
  #ifdef _DEBUG
   printf("mikaGalleryWidget::mikaGalleryWidget position(%dx%d+%d+%d)\n",
    data->attr.w,data->attr.h,data->attr.x,data->attr.y);
  #endif
  items = NULL;
  topItem = NULL;
  currentItem = NULL;
  
  struct tr_Attr attr;
  attr.x = 0;
  attr.y = (int)((float)data->attr.h*(float)0.8);
  attr.w = data->attr.w/2;
  attr.h = (int)((float)data->attr.h*(float)0.2);
  #ifdef _DEBUG
   printf("mikaGalleryWidget::mikaGalleryWidget prev(%dx%d+%d+%d)\n",
    attr.w,attr.h,attr.x,attr.y);
  #endif
  struct mikaWidgetData *wd = mikaWidgetDataNewEx(data->mwindow,&attr);
  prev = new mikaGalleryWidgetButton(wd,this);
  prev->gallery = this;
  #ifdef _TR_SYSTEM
   tr_ResetColor((struct tr_Object*)prev->getLow());
   tr_SetFontSize((struct tr_Object*)prev->getLow(),18);
   tr_ChangeTextExPos((struct tr_Object*)prev->getLow(),(char*)"prev",10,30);
   #ifdef _MIKA_GALLERY_WIDGET_ARROWS
    struct tr_Attr iattr;
    iattr.x = 0;
    iattr.y = 0;
    iattr.w = 70;
    iattr.h = 70;
    struct tr_Object *prevarrow = tr_ImageInitEx(NULL,&iattr,(char*)"img/leftarrow.jpg");
    if (prevarrow)
    {
     tr_AddImage((struct tr_Object*)prev->getLow(),prevarrow);
     tr_ImagePosCenter(prevarrow,(struct tr_Object*)prev->getLow());
    }
   #endif
  #endif
  
  attr.x = data->attr.w/2;
  wd = mikaWidgetDataNewEx(data->mwindow,&attr);
  next = new mikaGalleryWidgetButton(wd,this);
  next->gallery = this;
  #ifdef _TR_SYSTEM
   tr_ResetColor((struct tr_Object*)next->getLow());
   tr_SetFontSize((struct tr_Object*)next->getLow(),18);
   tr_ChangeTextExPos((struct tr_Object*)next->getLow(),(char*)"next",10,30);
  #endif
}

mikaGalleryWidget::~mikaGalleryWidget( )
{
}

void mikaGalleryWidget::onEvent( struct mikaEvent *event )
{
 #ifdef _DEBUG
  printf("mikaGalleryWidget::onEvent\n");
 #endif
}

void mikaGalleryWidget::onButtonDown( struct mikaEvent *event )
{
}

void mikaGalleryWidget::onButtonUp( struct mikaEvent *event )
{
}

int mikaGalleryWidget::addItem( mikaGalleryItem *item )
{
 #ifdef _DEBUG
  printf("mikaGalleryWidget::addItem\n");
 #endif
 if (!item)
  return 1;
 
 if (items)
  items->prev = item;
 item->next = items;
 items = item;
 
 if (!topItem)
  topItem = item;
  
 currentItem = items;
 
 return 0;
}

int mikaGalleryWidget::loadItemsDirectory( unsigned char *directory )
{
 if (!directory)
 {
  #ifdef _DEBUG
   printf("mikaGalleryWidget::loadItemsDirectory error, invalid directory\n");
  #endif
  return 0;
 }
 
 struct dirent *dp = NULL;
 DIR *dir = opendir((char*)directory);
 unsigned char path[512];
 if (dir)
 {
  while((dp=readdir(dir))!=NULL)
  {
   if (dp->d_name[0]=='.')
    continue;
   sprintf((char*)path,"%s/%s",(char*)directory,(char*)dp->d_name);
   
   mikaGalleryItem *item = new mikaGalleryItem((unsigned char*)path,
    myData.attr.w,(int)((float)myData.attr.h*(float)0.9),_MIKA_GALLERY_IMAGE);
   addItem(item);
  }
  closedir(dir);
 }
 
 return 0;
}

/** update with the current gallery item
 */
int mikaGalleryWidget::update( )
{
 if (currentItem)
 {
  #ifdef _TR_SYSTEM
   tr_AddImage((struct tr_Object*)getLow(),currentItem->image);
  #endif
 }
 tr_WindowToTop(myData.window);
 tr_WindowExpose(myData.window);
 return 0;
}

int mikaGalleryWidget::onControl( mikaGalleryWidgetButton *button )
{
 #ifdef _DEBUG
  printf("mikaGalleryWidget::onControl\n");
 #endif
 
 if (!button)
 {
  #ifdef _DEBUG
   printf("mikaGalleryWidget::onControl error, invalid button\n");
  #endif
  return 1;
 }
 
 if (button==prev)
  onPrev();
 else if (button==next)
  onNext();
 else
 
 return 0;
}

int mikaGalleryWidget::onNext( )
{
 #ifdef _DEBUG
  printf("mikaGalleryWidget::onNext\n");
 #endif
 if (currentItem->next)
  currentItem = currentItem->next;
 else
  currentItem = items;
 update();
 return 0;
}

int mikaGalleryWidget::onPrev( )
{
 #ifdef _DEBUG
  printf("mikaGalleryWidget::onPrev\n");
 #endif
 if (currentItem->prev)
  currentItem = currentItem->prev;
 else
  currentItem = topItem;
 update();
 return 0;
}
