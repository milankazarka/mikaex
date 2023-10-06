#include "buddy.h"

buddy *gbuddy = NULL;

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

buddyPos::buddyPos( mikaWindow *root )
{
    #ifdef _DEBUG
        printf("buddyPos::buddyPos\n");
    #endif
    
    struct tr_Attr attr;
    attr.x = 0;
    attr.y = 0;
    attr.w = _APP_W;
    attr.h = _APP_H;
    window = new mikaWindow(root,&attr);
    
}

buddyPos::~buddyPos( )
{
}

buddy::buddy( )
{
    #ifdef _DEBUG
        printf("buddy::buddy\n");
    #endif
    struct tr_Attr attr;
    attr.x = 0;
    attr.y = 0;
    attr.w = _APP_W;
    attr.h = _APP_H;
    root = new mikaWindow(NULL,&attr);
    window = new mikaWindow(root,&attr);
    
    struct tr_Attr wattr;
    wattr.x = 10;
    wattr.y = 10;
    wattr.w = 100;
    wattr.h = 100;
    struct mikaWidgetData *wd = mikaWidgetDataNewEx(window,&wattr);
    buddyWidget *w = new buddyWidget(wd);
    
    wattr.x = 10;
    wattr.y = 140;
    wattr.w = 100;
    wattr.h = 100;
    wd = mikaWidgetDataNewEx(window,&wattr);
    buddyWidget *w02 = new buddyWidget(wd);
    
    wattr.x = 10;
    wattr.y = 300;
    wattr.w = 300;
    wattr.h = 100;
    wd = mikaWidgetDataNewEx(window,&wattr);
    signage = new mikaSignageWidget(wd);
    mikaSignageWidgetItem *item = new mikaSignageWidgetItem((unsigned char*)"imgBuddy/signage001.jpg",300,100,
        _MIKA_SIGNAGE_WIDGET_IMAGE);
    signage->addItem(item);
    item = new mikaSignageWidgetItem((unsigned char*)"imgBuddy/signage002.jpg",300,100,
        _MIKA_SIGNAGE_WIDGET_IMAGE);
    signage->addItem(item);
    item = new mikaSignageWidgetItem((unsigned char*)"imgBuddy/signage003.jpg",300,100,
        _MIKA_SIGNAGE_WIDGET_IMAGE);
    signage->addItem(item);
    signage->play();
    
    wattr.x = 120;
    wattr.y = 5;
    wattr.w = 200;
    wattr.h = 200;
    wd = mikaWidgetDataNewEx(window,&wattr);
    gallery = new mikaGalleryWidget(wd);
    /**
    mikaGalleryItem *gitem = new mikaGalleryItem((unsigned char*)"imgBuddy/gallery001.jpg",200,120,_MIKA_GALLERY_IMAGE);
    gallery->addItem(gitem);
    gitem = new mikaGalleryItem((unsigned char*)"imgBuddy/gallery002.jpg",200,120,_MIKA_GALLERY_IMAGE);
    gallery->addItem(gitem);
    */
    gallery->loadItemsDirectory((unsigned char*)"imgBuddyGallery");
    gallery->update();
    
    wattr.x = 0;
    wattr.y = 0;
    wattr.w = 320;
    wattr.h = 200;
    wd = mikaWidgetDataNewEx(window,&wattr);
    video = new mikaVideoWidget(wd);
    //video->loadDirectory((unsigned char*)"./imgFilm");
    //video->play();
    
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
    
    activitySetMovable(&bigInterface->activity,0,0);
    activitySetForceangle(&bigInterface->activity,90,0);
    
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
