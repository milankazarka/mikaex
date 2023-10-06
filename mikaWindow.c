#include "mikaWindow.h"
#include "config.h"
#include "mikaSkinEx.h"

mikaWindow::mikaWindow( mikaWindow *parent, struct tr_Attr *attr )
{
    #ifdef _DEBUG
        printf("mikaWindow::mikaWindow parent(%p)\n",parent);
    #endif
    
    myParent = parent;
    if (!parent)
    {
            #ifdef _DEBUG
                printf("mikaWindow::mikaWindow creating root window\n");
            #endif
            #ifdef _TR_SYSTEM
                    window = tr_WindowInit(NULL,NULL,attr);
                    interfaceSetBackplaneSize(0,0);
                    activitySetMovable(&bigInterface->activity,1,0);
            #endif
            canvas = NULL;
    }
    else
    {
        #ifdef _DEBUG
            printf("mikaWindow::mikaWindow init child\n");
        #endif
        #ifdef _TR_SYSTEM
            #ifdef _DEBUG
                printf("mikaWindow::mikaWindow init window\n");
            #endif
            window = tr_WindowInit(NULL,parent->window,attr);
            tr_WindowMap(window);
            tr_WindowToTop(window);
            canvas = tr_CanvasInit(NULL,attr,window);
            
            tr_SetColor(canvas,(char*)"#ffffff");
            form_CanvasSetMov(canvas->widget.sub,CAN_MOV_NONE);
            tr_SetRound(canvas,2);
            #ifdef _DEBUG
                printf("mikaWindow::mikaWindow init window finish\n");
            #endif
        #endif
        struct mikaWidgetData *data = (struct mikaWidgetData*)malloc(sizeof(struct mikaWidgetData));
        #ifdef _TR_SYSTEM
            data->canvas = canvas;
            data->window = window;
        #endif
        mcanvas = new mikaCanvas(data);
    }
}

mikaWindow::~mikaWindow( )
{
    
}

/** add widget to a speciffic possition
 *
 */
int mikaWindow::addWidget( mikaWidget *widget )
{
        #ifdef _DEBUG
                printf("mikaWindow::addWidget\n");
        #endif
        if (!widget)
        {
                #ifdef _DEBUG
                        printf("mikaWindow::addWidget error, invalid widget\n");
                #endif
                return 1;
        }
        
        mcanvas->addWidget(widget);
        
        return 0;
}

int mikaWindow::push( )
{
    #ifdef _DEBUG
        printf("mikaWindow::push\n");
    #endif
    #ifdef _TR_SYSTEM
        tr_WindowToTop(window);
        tr_WindowExpose(window);
    #endif
    return 0;
}

/** received from children or from others when needed
 *
 */
void mikaWindow::onEventNotify( struct mikaEvent *event )
{
    if (!event)
    {
        #ifdef _DEBUG
            printf("mikaWindow::onEventNotify error, invalid event\n");
        #endif
        return;
    }
    #ifdef _DEBUG
        printf("mikaWindow::onEventNotify\n");
    #endif
}

