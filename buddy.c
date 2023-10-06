#include "buddy.h"

buddy *gbuddy = NULL;

buddyPos::buddyPos( )
{
}

buddyPos::~buddyPos( )
{
}

buddyGallery::buddyGallery( mikaWindow *root )
{
}

buddyGallery::~buddyGallery( )
{
}

buddyWidget::buddyWidget( struct mikaWidgetData *data )
 : mikaWidget(data)
{
}

buddyWidget::~buddyWidget( )
{
}

void buddyWidget::onButtonDown( struct mikaEvent *event )
{
    #ifdef _DEBUG
        printf("buddyWidget::onButtonDown\n");
    #endif
}

void buddyWidget::onButtonUp( struct mikaEvent *event )
{
    #ifdef _DEBUG
        printf("buddyWidget::onButtonUp\n");
    #endif
}

void buddyWidget::onEvent( struct mikaEvent *event )
{
    #ifdef _DEBUG
        printf("buddyWidget::onEvent\n");
    #endif
}

void *onListNodeEx( TiXmlElement *element, int position )
{
  if (!element || position<0)
  {
    return NULL;
  }
  #ifdef _DEBUG
    printf("\tposapp mikaList onListNodeEx position(%d)\n",position);
  #endif
  
  unsigned char text[128];
  int count = 0;
  
  mikaListItem *current = gbuddy->posList->items;

  while(current)
  {
    #ifdef _DEBUG
      printf("\tposapp mikaList onListNodeEx loop\n");
    #endif
    
    if (gbuddy->posList->nitems-position-1==count)
    {
      struct tr_Object *button = current->button;
      if (element->Attribute("image"))
      {
        #ifdef _DEBUG
          printf("\tposapp mikaList onListNodeEx reading image(%s)\n",(char*)element->Attribute("image"));
        #endif
        struct tr_Attr attr;
        attr = tr_GetAttr(150,70,0,0);
        struct tr_Object *image = tr_ImageInitEx(NULL,&attr,(char*)element->Attribute("image"));
        tr_AddImage(button,image);
      }
      if (element->Attribute("name"))
      {
        strcpy((char*)text,(char*)element->Attribute("name"));
        #ifdef _DEBUG
          printf("\tonListNode mikaList name(%s)\n",(char*)text);
        #endif
        tr_ChangeTextExPos(button,(char*)text,250,50);        
      }
      if (element->Attribute("price"))
      {
        strcpy((char*)text,(char*)element->Attribute("price"));
        #ifdef _DEBUG
          printf("\tonListNode mikaList price(%s)\n",(char*)text);
        #endif
        tr_ChangeTextExPos(button,(char*)text,250,70);      
      }
    }
    
    current = current->next;
    count++;
  }
  
  tr_WindowExpose(gbuddy->window->window);
 
  return NULL;
}

buddy::buddy( )
{
    #ifdef _DEBUG
        printf("buddy::buddy\n");
    #endif
    gbuddy = this;
    struct tr_Attr wattr;
    struct mikaWidgetData *wd = NULL;
    struct tr_Attr attr;
    attr.x = 0;
    attr.y = 0;
    attr.w = _APP_W;
    attr.h = _APP_H;
    root = new mikaWindow(NULL,&attr);
    window = new mikaWindow(root,&attr);
    
    /**
    wattr.x = 10;
    wattr.y = 10;
    wattr.w = 100;
    wattr.h = 100;
    struct mikaWidgetData *wd = mikaWidgetDataNewEx(window,&wattr);
    buddyWidget *w = new buddyWidget(wd);
    */
    
    /**
    wattr.x = 10;
    wattr.y = 140;
    wattr.w = 100;
    wattr.h = 100;
    wd = mikaWidgetDataNewEx(window,&wattr);
    buddyWidget *w02 = new buddyWidget(wd);
    */
    
    wattr.x = 0;
    wattr.y = 80;
    wattr.w = _APP_W;
    wattr.h = 120;
    wd = mikaWidgetDataNewEx(window,&wattr);
    signage = new mikaSignageWidget(wd);
    mikaSignageWidgetItem *item = new mikaSignageWidgetItem((unsigned char*)"imgBuddy/signage002b.jpg",300,100,
        _MIKA_SIGNAGE_WIDGET_IMAGE);
    signage->addItem(item);
    item = new mikaSignageWidgetItem((unsigned char*)"imgBuddy/signage003b.jpg",300,100,
        _MIKA_SIGNAGE_WIDGET_IMAGE);
    signage->addItem(item);
    item = new mikaSignageWidgetItem((unsigned char*)"imgBuddy/signage004b.jpg",300,100,
        _MIKA_SIGNAGE_WIDGET_IMAGE);
    signage->addItem(item);
    item = new mikaSignageWidgetItem((unsigned char*)"imgBuddy/signage005b.jpg",300,100,
        _MIKA_SIGNAGE_WIDGET_IMAGE);
    signage->addItem(item);
    signage->play();
    
    wattr.x = (_APP_W/2)-75;
    wattr.y = 2;
    wattr.w = 150;
    wattr.h = 78;
    wd = mikaWidgetDataNewEx(window,&wattr);
    rotate = new mikaRotateWidget(wd,_INIT_ROTATION);
    rotate->flip = 1;
    
    wattr.x = 0;
    wattr.y = 200;
    wattr.w = _APP_W-100;
    wattr.h = _APP_H-300;
    posList = new mikaList(window->canvas,&wattr,3);
    posDelegate.onLowest = NULL;
    posDelegate.onSelection = NULL;
    posDelegate.onNodeElement = onListNodeEx;
    posList->delegate = &posDelegate;
    posList->loadFile((unsigned char*)"sushi.xml");
    posList->update();

    wattr = tr_GetAttr(80,80,_APP_W-103,((_APP_H/2)-110)+40);
    up = tr_ButtonInitEx2(NULL,&wattr,window->canvas);
    tr_SetColor(up,(char*)"#ffffff");
    tr_SetRound(up,3);
    tr_CanvasAddObjStatic(window->canvas,up);
    //tr_SetAction(up,onUp,TR_ACTION_BUTTON);
    struct tr_Object *image = tr_ImageInitEx(NULL,&wattr,(char*)"img/uparrow.jpg");
    tr_AddImage(up,image);
    tr_ImagePosCenter(image,up);
  
    wattr = tr_GetAttr(80,80,_APP_W-103,((_APP_H/2)+20)+40);
    down = tr_ButtonInitEx2(NULL,&wattr,window->canvas);
    tr_SetColor(down,(char*)"#ffffff");
    tr_SetRound(down,3);
    tr_CanvasAddObjStatic(window->canvas,down);
    //tr_SetAction(down,onDown,TR_ACTION_BUTTON);
    image = tr_ImageInitEx(NULL,&wattr,(char*)"img/downarrow.jpg");
    tr_AddImage(down,image);
    tr_ImagePosCenter(image,down);    
    
    /**
    wattr.x = 120;
    wattr.y = 5;
    wattr.w = 200;
    wattr.h = 200;
    wd = mikaWidgetDataNewEx(window,&wattr);
    gallery = new mikaGalleryWidget(wd);
    
    mikaGalleryItem *gitem = new mikaGalleryItem((unsigned char*)"imgBuddy/gallery001.jpg",200,120,_MIKA_GALLERY_IMAGE);
    gallery->addItem(gitem);
    gitem = new mikaGalleryItem((unsigned char*)"imgBuddy/gallery002.jpg",200,120,_MIKA_GALLERY_IMAGE);
    gallery->addItem(gitem);
    
    gallery->loadItemsDirectory((unsigned char*)"imgBuddyGallery");
    gallery->update();
    */
    
    /**
    wattr.x = 0;
    wattr.y = 0;
    wattr.w = 320;
    wattr.h = 200;
    wd = mikaWidgetDataNewEx(window,&wattr);
    video = new mikaVideoWidget(wd);
    video->loadDirectory((unsigned char*)"./imgFilm");
    video->play();
    */
    
    /**
    wattr.x = 320;
    wattr.y = 320;
    wattr.w = 120;
    wattr.h = 60;
    wd = mikaWidgetDataNewEx(window,&wattr);
    clock = new mikaClockWidget(wd);
    
    wattr.x = 0;
    wattr.y = 500;
    wattr.w = 150;
    wattr.h = 150;
    wd = mikaWidgetDataNewEx(window,&wattr);
    rotate = new mikaRotateWidget(wd);
    
    pos = new buddyPos(root);
    pos->window->addWidget(signage);
    */
    
    activitySetMovable(&bigInterface->activity,0,0);
    activitySetForceangle(&bigInterface->activity,_INIT_ROTATION,0);
    
    tr_WindowToTop(window->window);
    tr_WindowQueue(root->window);
}

buddy::~buddy( )
{
    
}

int main( )
{
    interfaceSetName((unsigned char*)"BUDDY");

    osInit();
    trop_Init();
    gui_Init();
    tr_WmInit();
  
    layout = NULL;
    rotation = ROT_NONE;

    gbuddy = new buddy();
    
    return 0;
}
