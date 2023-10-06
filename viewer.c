#include "viewer.h"

viewer *gv = NULL;
int gnid = 1;

struct tr_Object *images[50];
struct tr_Object *button;

void *onButton(void*data)
{
    gv->setImage(gnid);
    gnid++;
}

void viewer::setImage( int nid )
{
    struct tr_Attr attr;
    attr.w = _APP_W-20;
    attr.h = _APP_H-20;
    attr.x = 10;
    attr.y = 10;
    //struct tr_Object *image;
    //unsigned char tmp[64];
    //sprintf((char*)tmp,"KENDU/00%d.jpg",nid);
    //image = tr_ImageInitEx(NULL,&attr,(char*)tmp);
    
    tr_AddImage(button,images[nid]);
    tr_ImagePosCenter(images[nid],button);
    tr_WindowToTop(gv->window->window);
    tr_WindowExpose(gv->window->window);
}

viewer::viewer( )
{
    gv = this;
    struct tr_Attr wattr;
    struct mikaWidgetData *wd = NULL;
    struct tr_Attr attr;
    attr.x = 0;
    attr.y = 0;
    attr.w = _APP_W;
    attr.h = _APP_H;
    root = new mikaWindow(NULL,&attr);
    window = new mikaWindow(root,&attr);       

    attr.w = _APP_W-20;
    attr.h = _APP_H-20;
    attr.x = 10;
    attr.y = 10;
    button = tr_ButtonInitEx2(NULL,&attr,window->canvas);
    tr_SetAction(button,onButton,TR_ACTION_BUTTON);    
    
    /**
    struct tr_Object *image = tr_ImageInitEx(NULL,&attr,(char*)"KENDU/001.jpg");
    tr_AddImage(button,image);
    tr_ImagePosCenter(image,button);
    image = tr_ImageInitEx(NULL,&attr,(char*)"KENDU/002.jpg");
    tr_AddImage(button,image);
    tr_ImagePosCenter(image,button);    
    */
    attr.w = _APP_W-20;
    attr.h = _APP_H-20;
    attr.x = 10;
    attr.y = 10;
    int n;
    for(n = 1; n < 45; n++)
    {
        struct tr_Object *image;
        unsigned char tmp[64];
        sprintf((char*)tmp,"KENDU/00%d.jpg",n);
        image = tr_ImageInitEx(NULL,&attr,(char*)tmp);
        images[n] = image;
    }
    
    setImage(7);
    
    tr_SetColor(button,(char*)"#ffffff");
    tr_CanvasAddObjStatic(window->canvas,button);  
    
    activitySetMovable(&bigInterface->activity,0,0);
    activitySetForceangle(&bigInterface->activity,_INIT_ROTATION,0);
    
    tr_WindowToTop(window->window);
    tr_WindowQueue(root->window);
}

viewer::~viewer( )
{
    
}

void *onListener(int nid)
{
    gv->setImage(nid);
    return NULL;
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

    listener *l = new listener();
    l->delegate.onListener = onListener;
    l->loop();
    
    viewer *v = new viewer();
    return 0;
}
