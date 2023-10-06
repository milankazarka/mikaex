#include "gallery.h"

#define _ITEMS 56
struct tr_Object *buttons[_ITEMS];
gallery *g = NULL;

void *onButton(void*data)
{
    printf("gallery onButton\n");
  
    struct gui_Event *event = (struct gui_Event*)data;
    struct gui_Widget *widget = event->widget;
    
    int n;
    for(n = 0; n < _ITEMS; n++)
    {
        if (buttons[n]==0x00)
            continue;
        if (widget==buttons[n]->widget.sub)
        {
            printf("gallery onButton match\n");
            
            struct sockaddr_in addr;
            struct hostent *server;
            int sockfd;  
  
            sockfd = socket( AF_INET, SOCK_STREAM, 0 );  
  
            if (sockfd<0)
            {
                printf("gallery error couldn't create socket\n");
                return NULL;
            }
  
            server = gethostbyname((char*)"127.0.0.1");

            bzero((char*)&addr,sizeof(addr));    
            
            addr.sin_family = AF_INET;
            bcopy(
                (char*)server->h_addr,
                (char*)&addr.sin_addr.s_addr,
                server->h_length
            );
     
            addr.sin_port = htons(2501);
            
            unsigned char msg[12];
            sprintf((char*)msg,"%d\n",n);
            printf("sendData%d\n",n);
            
            int conn = connect(sockfd,(struct sockaddr*)&addr,sizeof(addr));
            if (conn<0)
            {
                printf("sendData error\n");
                return NULL;
            }
            
            //if (!conn)
            {
                printf("sendData sending\n");
                int data = send(sockfd,(char*)msg,strlen((char*)msg),0);
                close(sockfd);
            }
        }    
    }
}

void *onWindow001(void*data)
{
    printf("onWindow001\n");
    tr_WindowToTop(g->window001->window);
    tr_WindowExpose(g->window001->window);
}

void *onWindow002(void*data)
{
    printf("onWindow002\n");
    tr_WindowToTop(g->window002->window);
    tr_WindowExpose(g->window002->window);
}

void *onWindow003(void*data)
{
    printf("onWindow003\n");
    tr_WindowToTop(g->window003->window);
    tr_WindowExpose(g->window003->window);
}

void *onWindow004(void*data)
{
    printf("onWindow004\n");
    tr_WindowToTop(g->window004->window);
    tr_WindowExpose(g->window004->window);
}

void gallery::setItems( mikaWindow *_window )
{
    printf("gallery::setItems\n");
    int n;
    static int position = 1;
    struct tr_Attr attr;
    attr.w = 200;
    attr.h = 200;
    attr.x = 10;
    attr.y = 10;
    unsigned char path[64];
    struct tr_Object *button = NULL;
    int yfill = 10;
    int xfill = 10;

    int elements = 12;    
    if (_window==window004)
        elements=7;
    
    for( n = 1; n < elements+1; n++ )
    {
        button = tr_ButtonInitEx2(NULL,&attr,_window->canvas);
        sprintf((char*)path,"KENDU/00%ds.jpg",position);
        struct tr_Object *image = tr_ImageInitEx(NULL,&attr,(char*)path);
        tr_AddImage(button,image);
        tr_ImagePosCenter(image,button);
        tr_SetAction(button,onButton,TR_ACTION_BUTTON);
        tr_SetColor(button,(char*)"#ffffff");
        tr_CanvasAddObjStatic(_window->canvas,button);    
        buttons[position] = button;
        
        if (xfill+attr.w>820)
        {
            xfill = 10;
            yfill += 200;
        }
        else
        {
            xfill += 220;
        }
        attr.x = xfill;
        attr.y = yfill;
        
        position++;
    }
    
    attr.x = 5.0f;
    attr.y = _APP_H-90;
    attr.w = (_APP_W/4)-10;
    attr.h = 90;

    button = tr_ButtonInitEx2(NULL,&attr,_window->canvas);
    tr_SetAction(button,onWindow001,TR_ACTION_BUTTON);
    tr_SetColor(button,(char*)"#c0c0c0");
    tr_SetRound(button,3);
    tr_CanvasAddObjStatic(_window->canvas,button);
    //
    attr.x+=_APP_W/4;
    button = tr_ButtonInitEx2(NULL,&attr,_window->canvas);
    tr_SetAction(button,onWindow002,TR_ACTION_BUTTON);
    tr_SetColor(button,(char*)"#b0b0b0");
    tr_SetRound(button,3);
    tr_CanvasAddObjStatic(_window->canvas,button);
    //
    attr.x+=_APP_W/4;
    button = tr_ButtonInitEx2(NULL,&attr,_window->canvas);
    tr_SetAction(button,onWindow003,TR_ACTION_BUTTON);
    tr_SetColor(button,(char*)"#a0a0a0");
    tr_SetRound(button,3);
    tr_CanvasAddObjStatic(_window->canvas,button);
    //
    attr.x+=_APP_W/4;
    button = tr_ButtonInitEx2(NULL,&attr,_window->canvas);
    tr_SetAction(button,onWindow004,TR_ACTION_BUTTON);
    tr_SetColor(button,(char*)"#606060");
    tr_SetRound(button,3);
    tr_CanvasAddObjStatic(_window->canvas,button);
}

gallery::gallery( )
{
    g = this;
    struct tr_Attr wattr;
    struct mikaWidgetData *wd = NULL;
    struct tr_Attr attr;
    attr.x = 0;
    attr.y = 0;
    attr.w = _APP_W;
    attr.h = _APP_H;
    root = new mikaWindow(NULL,&attr);
    window = new mikaWindow(root,&attr);
    window001 = new mikaWindow(root,&attr);    
    window002 = new mikaWindow(root,&attr);    
    window003 = new mikaWindow(root,&attr);    
    window004 = new mikaWindow(root,&attr);    
    
    struct tr_Object *button = NULL;

    int n;
    for(n = 0; n < _ITEMS; n++)
    {
        buttons[n] = 0x00;
    }
    
    setItems( window001 );   
    setItems( window002 );     
    setItems( window003 );     
    setItems( window004 );     
    
    activitySetMovable(&bigInterface->activity,0,0);
    activitySetForceangle(&bigInterface->activity,_INIT_ROTATION,0);
    
    tr_WindowToTop(window001->window);
    tr_WindowQueue(root->window);
}

gallery::~gallery( )
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

    g = new gallery();    
    
    return 0;
}
