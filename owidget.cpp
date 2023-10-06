/**
 *  Mika 2010
 */
#include "wm.h"
#include "system.h"
#include "gui.h"

#define __NAME 			"DRAQ"
#define __MSG_EINIT_TR      	100
#define __MSG_EINIT_GUI     	101
#define __MSG_EINIT_TRWM    	102
#define __MSG_ELOOP_NOMAIN  	103

#define STATIC_NO

class CGuiObj {
    private:
        CGuiObj *mbody;
    public:
        CGuiObj( ) { }
        CGuiObj( CGuiObj *body )
        {
            mbody = body;
        }
        ~CGuiObj( ) { }
        
        CGuiObj *GetBody( )
        {
            return mbody;
        }
};

class CGuiPoint2D {
    private:
        int mx, my;
    public:
        CGuiPoint2D( unsigned short int x,
                    unsigned short int y )
        {
            mx = x;
            my = y;
        }
};

enum _CGUI_EVENT_TYPES
{
    CGUI_EVENT_MOTION,
    CGUI_EVENT_BUTTONDOWN,
    CGUI_EVENT_BUTTONUP,
    CGUI_EVENT_EXPOSE
};

class CGuiEvent : public CGuiObj {
    public:
        int type;
};

class CGuiWidgetAttributes
{
    public:
        unsigned short int	width;
        unsigned short int	height;
        unsigned short int	x;
        unsigned short int	y;
        
        CGuiWidgetAttributes( )
        {
            width   = 0;
            height  = 0;
            x       = 0;
            y       = 0;
        }
        
        CGuiWidgetAttributes(
            int _width, int _height,
            int _x, int _y
        )
        {
            width   = _width;
            height  = _height;
            x       = _x;
            y       = _y;
        }
};

/**
 *  CGuiApp class
 */

typedef struct _cguiapp_data
{
    void        *(*init)(void*obj);
} _cguiapp_data;

class CGuiApp : public CGuiObj {
    private:
        struct _cguiapp_data data;
    public:
        static CGuiApp *mapp;
        
        CGuiApp( );
        ~CGuiApp( );
        int OnExit( int code );
};

void *__cguiapp_data_init(void*obj)
{
    CGuiApp *app = (CGuiApp*)obj;
    
    if ( trop_Init() != 0 )
    {
        printf("%s%dE error initializing tropical layer\n",
               __NAME,__MSG_EINIT_TR);
        app->OnExit(2);
    }
    if ( gui_Init() != 0 )
    {
        printf("%s%dE error initializing gui layer\n",
               __NAME,__MSG_EINIT_GUI);
        app->OnExit(2);
    }
    if ( tr_WmInit() != 0 )
    {
        printf("%s%dE error initializing tropical wm layer\n",
               __NAME,__MSG_EINIT_TRWM);
        app->OnExit(2);
    }
    
    return NULL;
}

struct _cguiapp_data *_cguiapp_data_init( struct _cguiapp_data *data )
{
    if (!data) data = (struct _cguiapp_data*)malloc(sizeof(struct _cguiapp_data));
    data->init = __cguiapp_data_init;
    return data;
}

CGuiApp::CGuiApp( )
{
    _cguiapp_data_init(&data);
}

CGuiApp::~CGuiApp( ) { }
        
int CGuiApp::OnExit( int code )
{
    exit(code);
}
/**
 *  CGuiApp class end
 */

/**
 *  CGuiWidget class
 */
class CGuiWidget : public CGuiObj {
    public:
        static CGuiWidget *mainWidget;
        struct gui_Widget *lowWidget;
        
        CGuiWidget        *parent;
        
        CGuiWidgetAttributes attr;
        
        CGuiWidget( CGuiWidget *_parent ) :
            CGuiObj( this )
        {
            lowWidget = gui_NewWidget( );
            if (_parent) gui_WidgetInit( lowWidget,_parent->lowWidget );
            else
            { 
                gui_WidgetInit( lowWidget, NULL );
                mainWidget = this;
            }
            
            parent      = _parent;
        }
        ~CGuiWidget( )
        {
        
        }
        
        CGuiWidget *GetMain( )
        {
            return mainWidget;
        }
        
        int Configure( CGuiWidgetAttributes *_attr )
        {
            struct tr_Attr trAttr;
            
            attr.width      = _attr->width;
            attr.height     = _attr->height;
            attr.x          = _attr->x;
            attr.y          = _attr->y;
            
            trAttr.w = attr.width;
            trAttr.h = attr.height;
            trAttr.x = attr.x;
            trAttr.y = attr.y;
            
            _set_gui_object_attr(lowWidget,&trAttr);
            
            return 0;
        }
        
        int StartEventLoop( )
        {
            if (!parent)
            {
                
            }
            else
            {
                printf("%s%dW can't start event loop from a sub widget\n",
                       __NAME,__MSG_ELOOP_NOMAIN);
                return -1;
            }
            return 0;
        }
    protected:
        virtual void OnDraw( ) { };
        virtual void OnButtonDown( ) { };
        virtual void OnMotion( ) { };
        virtual void OnExpose( ) { };
};
/**
 *  CGuiWidget class end
 */

class CGuiWnd : public CGuiWidget {
    private:
        CGuiWidget     *widget;
    public:
        CGuiWnd( CGuiWidget *parent ) :
            CGuiWidget(parent)
        {
        }
        ~CGuiWnd( )
        {
            
        }
        
    protected:
        virtual void OnDraw( ) { }
        virtual void OnButtonDown( ) { }
        virtual void OnMotion( ) { }
        virtual void OnExpose( ) { }
};

CGuiWidget *CGuiWidget::mainWidget = NULL;

typedef struct glObject
{
    long id;
    void *custom;
    
    void *(*add)(struct glObject *pool, struct glObject *obj);
    void *(*freeAll)( struct glObject *pool );
    void *(*setCustom)( struct glObject *obj, void *custom );
    
    struct glObject *next;
} glObject;

void *_glObjectFreeAllCall( struct glObject *pool )
{
    struct glObject *current = NULL;
    struct glObject *next = NULL;
    current = pool;
    while(current)
    {
        next = current->next;
        free(current);
        current = next;
    }
    return NULL;
}

void *_glObjectSetCustomCall( struct glObject *obj, void *custom )
{
    if (!obj||!custom) return NULL;
    obj->custom = custom;
    return NULL;
}

void *_glObjectAddCall( struct glObject *pool, struct glObject *obj )
{
    struct glObject *current = NULL;
    if (!obj)
        obj = (struct glObject*)malloc(sizeof(struct glObject));
    
    obj->add 		= _glObjectAddCall;
    obj->freeAll 	= _glObjectFreeAllCall;
    obj->setCustom	= _glObjectSetCustomCall;
    obj->custom 	= NULL;
    obj->next		= NULL;
    
    current = pool;
    while(current)
    {
        if (current->next==NULL)
        {
            current->next = obj;
            break;
        }
        current = current->next;
    }
        
    return obj;
}

struct glObject *glObjectNew( struct glObject *pool, struct glObject *obj )
{
    return (struct glObject*)_glObjectAddCall(pool,obj);
}

class CGlScene : public CGuiWidget {
    private:
        struct glObject *gloPool;
    public:
        
        void freePool( )
        {
            if (gloPool)
            {
                gloPool->freeAll(gloPool);
            }
        }
        
        void addGlObj( struct glObject *glo )
        {
            if (!gloPool)	gloPool = glObjectNew(NULL,NULL);
            else		gloPool->add(gloPool,glo);
        }
        
        CGlScene( CGuiWidget *parent ) :
            CGuiWidget(parent)
        {
            gloPool = NULL;
        }
        ~CGlScene( )
        {
            freePool();
        }
    protected:
        virtual void OnDraw( ) { }
        virtual void OnButtonDown( ) { }
        virtual void OnMotion( ) { }
        virtual void OnExpose( ) { }
};

class CGuiLabel : public CGuiWidget {
    public:
        CGuiLabel( CGuiWidget *parent ) : 
            CGuiWidget(parent)
        {
        }
        ~CGuiLabel( )
        {
        }
};

#ifdef STATIC
    int main( )
    {
        printf("initializing CGuiApp\n");
        CGuiApp *app = new CGuiApp();
        CGuiWnd *mainwnd = new CGuiWnd( NULL );
        return 0;
    }
#endif
