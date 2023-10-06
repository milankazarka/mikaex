#include "hotel.h"

#define _APP_SIZE_W 480
#define _APP_SIZE_H 640

int hotelMenuSetupCanvas( struct tr_Object *canvas )
{
    if (!canvas)
    {
        #ifdef _DEBUG
            printf("hotelMenuSetupCanvas error, invalid canvas\n");
        #endif
        return 1;
    }
    
    struct tr_Attr battr;
    battr.x = 0;
    battr.y = 0;
    battr.w = _APP_SIZE_W;
    battr.h = 100;
    struct tr_Object *title = tr_ButtonInitEx2(NULL,&battr,canvas);
    tr_SetColor(title,(char*)"#fafafa");
    tr_SetRound(title,2);
    tr_CanvasAddObjStatic(canvas,title);
    struct tr_Object *titleBg = tr_ImageInitEx(NULL,&battr,(char*)"imgHotel/title.jpg");
    tr_AddImage(title,titleBg);
    tr_ImagePosCenter(titleBg,title);
    
    /**
    battr.x = 0;
    battr.y = 100;
    battr.w = _APP_SIZE_W;
    battr.h = _APP_SIZE_H-100;
    struct tr_Object *poster = tr_ButtonInitEx2(NULL,&battr,canvas);
    tr_SetColor(poster,(char*)"#fafafa");
    tr_SetRound(poster,2);
    tr_CanvasAddObjStatic(canvas,poster);
    struct tr_Object *posterBg = tr_ImageInitEx(NULL,&battr,(char*)"imgHotel/poster.jpg");
    tr_AddImage(poster,posterBg);
    tr_ImagePosCenter(posterBg,poster);
    */
    return 0;
}

void *hotelMenuOnOrder( struct mikaListboxNode *node )
{
    #ifdef _DEBUG
        printf("hotelMenuOnOrder\n");
    #endif
    return NULL;
}

void *hotelMenuOnSelection( struct mikaListboxNode *node )
{
    #ifdef _DEBUG
        printf("hotelMenuOnSelection\n");
    #endif
    return NULL;
}

hotelPoster::hotelPoster( struct tr_Window *parentWindow )
{
}

hotelPoster::~hotelPoster( )
{
}

hotelMenu::hotelMenu( struct tr_Window *parentWindow )
{
    #ifdef _DEBUG
        printf("hotelMenu::hotelMenu\n");
    #endif
    attr.x = 0;
    attr.y = 0;
    attr.w = _APP_SIZE_W;
    attr.h = _APP_SIZE_H;
    
    window	= tr_WindowInit(NULL,parentWindow,&attr);
    tr_WindowMap(window);
    tr_WindowToTop(window);
    
    canvas = tr_CanvasInit(NULL,&attr,window);
    form_CanvasSetMov(canvas->widget.sub,CAN_MOV_NONE);
    tr_SetRound(canvas,2);
    tr_SetColor(canvas,(char*)"#303030");
    
    struct tr_Attr lattr;
    lattr.x = 0;
    lattr.y = 100;
    lattr.w = _APP_SIZE_W;
    lattr.h = _APP_SIZE_H-(100*2);
    listbox = new mikaListbox(canvas,&lattr,5);
    listbox->loadNodeFile((unsigned char*)"hotelMenu.xml");
    
    hotelMenuSetupCanvas(canvas);
    
    delegate.onOrder = hotelMenuOnOrder;
    delegate.onSelection = hotelMenuOnSelection;
    listbox->delegate = &delegate;
    
    mikaPanelItem *pitem = NULL;
    lattr.x = 2;
    lattr.y = _APP_SIZE_H-98;
    lattr.w = _APP_SIZE_W-4;
    lattr.h = 96;
    panel = new mikaPanel(canvas,&lattr);
    lattr.x = 5;
    lattr.y = 5;
    lattr.w = 90;
    lattr.h = 90;
    pitem = new mikaPanelItem(canvas,&lattr);
    pitem->setName((unsigned char*)"panel");
    panel->addItem(pitem);
}

hotelMenu::~hotelMenu( )
{
    
}

int hotelMenu::raise( )
{
    #ifdef _DEBUG
        printf("hotelMenu::raise\n");
    #endif
    tr_WindowToTop(window);
    tr_WindowExpose(window);
    return 0;
}

hotel::hotel( )
{
    osInit();
    trop_Init();
    gui_Init();
    tr_WmInit();
  
    layout = NULL;
    rotation = ROT_NONE;
    
    mainAttr.x = 0;
    mainAttr.y = 0;
    mainAttr.w = _APP_SIZE_W;
    mainAttr.h = _APP_SIZE_H;
    
    root    = tr_WindowInit(NULL,NULL,&mainAttr);
    wmain   = tr_WindowInit(NULL,root,&mainAttr);
    tr_WindowMap(wmain);
    tr_WindowToTop(wmain);
    cmain = tr_CanvasInit(NULL,&mainAttr,wmain);
    form_CanvasSetMov(cmain->widget.sub,CAN_MOV_NONE);
    tr_SetRound(cmain,2);
    tr_SetColor(cmain,(char*)"#ffffff");
    
    interfaceSetBackplaneSize(0,0);
    activitySetMovable(&bigInterface->activity,0,0);
  
    hMenu = new hotelMenu(root);
  
    tr_WindowToTop(wmain);
    hMenu->raise();
    tr_WindowQueue(root);
}

hotel::~hotel( )
{
    
}

int main( )
{
    hotel *hi = new hotel();
    return 0;
}
