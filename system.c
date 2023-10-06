/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "system.h"
#include "gui.h"
#include "xml.h"
#include "interface.h"
#include "global.h"
#include "config.h"

#ifdef JPEG
  #include "jpeglib.h"
  #include "jpegint.h"
  #include "jerror.h"
  #include "jconfig.h"
#endif

/** DataModel, DataModelDescriptor - deprecated
 */

/**
 *  DataModel & DataModelDescriptor
 *
 *  in UI objects a DataModel may come in handy when creating a list
 *  of objects of whoms attributes we wish to set in an automated way
 */
struct trop_DataModelDescriptor *trop_DataModelDescriptorInit(
  struct trop_DataModelDescriptor *desc
)
{
  if (!desc)
    desc = (struct trop_DataModelDescriptor*)malloc(sizeof(struct trop_DataModelDescriptor));
  desc->key 		= NULL;
  desc->value 		= NULL;
  desc->len_key 	= 0;
  desc->len_value 	= 0;
  desc->next 		= NULL;
  return desc;
}

struct trop_DataModelDescriptor *_trop_data_model_descriptor_add_pair(
  struct trop_DataModelDescriptor *desc,
  unsigned char *key,
  unsigned char *value,
  int numValue
)
{
  struct trop_DataModelDescriptor *current = NULL;

  if (!key||!value) return NULL;
  
  current = trop_DataModelDescriptorInit(NULL);
  current->next = desc;
  current->key = (unsigned char*)malloc(strlen(key)+1);
  strcpy(current->key,key);
  
  current->value = NULL;
  current->nValue = -1;
  current->len_value = 0;
  
  if (value)
  {
    current->value = (unsigned char*)malloc(strlen(value)+1);
    strcpy(current->value,value);
    current->len_value = strlen(value);
  }
  
  if (numValue!=-1)
  {
    current->nValue = numValue;
  }
  
  current->len_key = strlen(key);
  
  desc = current;
  
  return desc;
}

struct trop_DataModelDescriptor *trop_DataModelDescriptorAddPair(
  struct trop_DataModelDescriptor *desc,
  unsigned char *key,
  unsigned char *value
)
{
  return _trop_data_model_descriptor_add_pair(desc,key,value,-1);
}

struct trop_DataModelDescriptor *trop_DataModelDescriptorAddPairNum(
  struct trop_DataModelDescriptor *desc,
  unsigned char *key,
  int value
)
{
  return _trop_data_model_descriptor_add_pair(desc,key,NULL,value);
}

unsigned char *trop_DataModelDescriptorGetValue(
  struct trop_DataModelDescriptor *desc,
  unsigned char *key
)
{
  struct trop_DataModelDescriptor *current;
  unsigned char *value = NULL;
  int len_key;
  
  if (!desc||!key) return NULL;
 
  len_key = strlen(key);
  
  current = desc;
  while(current)
  {
    if (
      current->key!=NULL &&
      current->len_key == len_key
      )
    {
      if (memcmp(current->key,key,len_key)==0)
      {
        value = current->value;
        break;
      }
    }
    current = current->next;
  }
  
  return value;
}

struct trop_DataModel *trop_DataModelInit(
  struct trop_DataModel *model
)
{
  if (!model)
    model = (struct trop_DataModel*)malloc(sizeof(struct trop_DataModel));
  model->csv = NULL;
  model->descriptor = NULL;
  return model;
}

/**
 *  generates the data model from CSV data
 */
struct trop_DataModel *trop_DataModelGetFromCsv(
  struct trop_DataModel *model,
  unsigned char *csvFilename
)
{
  if (!csvFilename) return NULL;
  
  if (!model)	model = trop_DataModelInit(NULL);
  else		trop_DataModelInit(model);
  
  model->csv = trop_CsvInit(model->csv,csvFilename);
  
  return model;
}

int trop_DataModelGetDataAtPosition(
  struct trop_DataModel *model,
  int posList,
  int posMember,
  int maxLen,
  unsigned char *data,
  int *lenData
)
{
  if (!model||posList<0||posMember<0) return -1;
  
  #ifdef _DEBUG
    if (DEBUG==TRUE) 
      printf(":trop_DataModelGetDataAtPosition\n");
  #endif
  
  if (model->csv)
  {
    if ( trop_CsvGetDataAtPosition(model->csv,posList,posMember,maxLen,data,lenData) != 0 )
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf(":trop_DataModelGetDataAtPosition could not retrieve member\n");
      #endif
      return -1;
    }
  }
  
  return 0;
}

int trop_DataModelGetListSize(
  struct trop_DataModel *model
)
{
  if (!model) return 0;
  
  if (model->csv)
  {
    return trop_CsvGetListSize(model->csv);
  }
  return 0;
}

int trop_DataModelGetListMemberSize(
  struct trop_DataModel *model,
  int posList
)
{
  if (!model) return 0;
  
  if (model->csv)
  {
    return trop_CsvGetListMemberSize(model->csv,posList);
  }
  return 0;
}

#ifdef USEFT

  static char *testtext = (char*)"hello";

  static void event_font_change( int delta ) { }
  int _fttrop_draw_string( const char *string, int x, int y ) { return 0; }

  void *_ft_thread_cycle( )
  {
    #ifdef _FT_TYPE_CUSTOM
      grEvent event;
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("_ft_thread redraw\n");
      #endif
      
      FTDemo_Display_Clear(ftdisplay);
      
      _fttrop_draw_string("hello.. 00",20,20);
      
      _fttrop_draw_string("hello",
        ftdisplay->bitmap->width / 2,
        ftdisplay->bitmap->rows / 2);
      
      grRefreshRectangle(ftdisplay->surface,0,0,100,40);
    #endif
  }

  int _ft_init( )
  {
    
    return 0;
  }
#endif

int trop_Init( )
{
  _trop_conn_in = malloc(TROP_CONNBUF);
  _trop_conn_out = malloc(TROP_CONNBUF);

  trop_MutexInit(&_m_lock);
  #ifdef USESSL
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":ssl init\n");
    #endif
    SSL_load_error_strings();
    SSL_library_init();
  #endif
  
  #ifdef USEX
    gui_InitSkin(&skin);
    lst_widgets = NULL;
  #endif
  
  #ifdef USETCL
    trop_MutexInit(&tcl_lock);
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":init TCL\n");
    #endif
    interp = Tcl_CreateInterp();
    if ( Tcl_Init(interp) != TCL_OK )
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf(" :init TCL failed\n");
      #endif
      return -1;
    }
    if ( Tk_Init(interp) != TCL_OK )
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf(" :init TK failed\n");
      #endif
      return -1;
    }
  #endif
  
  return 0;
}

#ifdef USETCL
  void *_tcl_event_thread(void*data)
  {
    Tk_MainLoop();
  }

  int _tcl_init( )
  {
    char *screen = getenv("DISPLAY");
    if (!screen) screen = ":0.0";
    Tk_Window main = Tk_MainWindow(interp);
    tkwin = main;
    if (!Tk_IsMapped(tkwin))
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf(" :map tk window\n");
      #endif
      Tk_MapWindow(tkwin);
      Tk_ResizeWindow(tkwin,480,320);
      Window id = Tk_WindowId(tkwin);
      
      char *script01 = \
        "label .l1 -text inte -bg black -fg white\npack .l1\n";
      Tcl_Eval(interp,script01);
      
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf(" :tk window X id '%d'\n",id); 
      #endif
      struct trop_Atom a;
      a.call = _tcl_event_thread;
      a.data = NULL;
      trop_AtomInit(&a);
    }
    return 0;
  }
#endif

int trop_Deinit( )
{
  #ifdef USETCL
    if (interp)
    {
      trop_MutexLock(&tcl_lock);
      Tcl_DeleteInterp(interp);
      trop_MutexUnlock(&tcl_lock);
    }
  #endif
  _trop_termination( );
}

#ifdef USEX

  BOOL gui_WidgetIsViewable( struct gui_Widget *w )
  {
    if (w->attr.state == 0) return TRUE;
    return FALSE;
  }

  BOOL gui_WidgetAreaOverlap( struct gui_WidgetAttributes *attr, int x, int y, int w, int h )
  {
    if ( x > attr->x + attr->w || // test exposial to widget position
      y > attr->y + attr->h ||
      x + w < attr->x ||
      y + h < attr->y )
    {
      return FALSE;
    }
    return TRUE;
  }

  BOOL gui_WidgetAreaOverlapEx( int x1, int y1, int w1, int h1, int x, int y, int w, int h )
  {
    if ( x > x1 + w1 || // test exposial to widget position
      y > y1 + h1 ||
      x + w < x1 ||
      y + h < y1 )
    {
      return FALSE;
    }
    return TRUE;
  }

  /** an implementation of a buffer holding the surface of a Widget 
   */
  inline int gui_WidgetCreateSurface(
    gui_Widget *wi
  )
  {
    if (!wi) return -1;
    if (wi->_surface) return 0;
    
    wi->_surface = gui_SurfaceInitEx(gui_GetMain(),NULL,wi->attr.w,wi->attr.h);
    
    return 0;
  }

  /** copy a widgets surface area to the main surface 
   */
  int gui_WidgetSurfaceToMain(
    gui_Widget *wi,
    int x, int y,
    int w, int h,
    int px, int py,
    int round
  )
  {
    if (!wi) return -1;
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("gui_WidgetSurfaceToMain\n");
    #endif
    
    gui_DrawImageRound(
      wi->_surface->image,
      0, 0,
      x+px, y+py,
      w, h,
      round
    );
    return 0;
  }
  
  /** general widget expose, the positioning is relative to parent
   */
  int gui_WidgetExpose( struct gui_Widget *wi, int x, int y, int w, int h )
  { 
    struct gui_Widget    	*parent;
    int	        		px; // parent
    int  			py;
    struct gui_object	        *gobj;
    struct gui_GC       	*gc;
    BOOL			btrans;
    int			        round;
    
    int                         tmpx;
    int                         tmpy;
    int                         tmpw;
    int                         tmph;
    
    static long                  count = 0;

    #ifndef _WIDGET_EXPOSE
      return 0;
    #endif

    count++;
    
    #ifdef _DEBUG
      printf(":gui_WidgetExpose count(%d) __bounds\n",count);
    #endif
    if (gui_WidgetIsViewable(wi) == FALSE)
      return -1;
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":gui_WidgetExpose viewable\n");
    #endif
    
    parent	= wi->parent;
    px 		= 0; // if parent is toplevel, ignore it's position
    py 		= 0;
    if (gui_GetMain()->parent != wi->parent)
    {
      px = wi->parent->attr.x;
      py = wi->parent->attr.y;
    }
    
    if (!parent)
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf(":gui_WidgetExpose (not processing main widget)\n");
      #endif
      return 0;
    }
    
    #ifdef _DEBUG
      if (parent)
      {
        if (DEBUG==TRUE) printf("\twidget expose (%dx%d+%d+%d) in (%dx%d+%d+%d)\n",
          w,h,x,y,
          parent->attr.w,parent->attr.h,parent->attr.x,parent->attr.y);
      }
    #endif
    
    if (x>=wi->parent->attr.w || y>=wi->parent->attr.h) return -1;
    if (y+h<=0) return -1;
    
    btrans = FALSE; // we cannot use Widget surface buffers on transparent widgets
    round = 0;
    gobj = wi->obj;
    while(gobj!=NULL)
    {
      if (gobj->id == GOID_COLOR)
      {
        gc = (struct gui_GC*)gobj->data;
        if (gc->colorl!=NULL)
        {
          if (gc->colorl->value==C_TRANS_VALG)
            btrans = TRUE;
        }
      }
      if (gobj->id == GCFG_ROUND)
      {
        round = gobj->value;
      }
      gobj = gobj->next;
    }
    
      tmpx = gui_bounds.x;
      tmpy = gui_bounds.y;
      tmpw = gui_bounds.width;
      tmph = gui_bounds.height;
      gui_BoundsSet(&gui_bounds,px,py,parent->attr.w,parent->attr.h);
    
      _gui_widget_expose_objects(
        wi,
        x, y,
        w, h,
        px, py
      );
    
      gui_BoundsSet(&gui_bounds,tmpx,tmpy,tmpw,tmph);
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":gui_WidgetExpose end\n");
    #endif
    
    return 0;
  }
  
  struct gui_Image *gui_ImagePick( char *name )
  {
    struct gui_Image *image = NULL;
    struct gui_Image *_image = NULL;
    struct trop_List *current = lst_images;
    while(current != NULL)
    {
      _image = (struct gui_Image*)current->ptr;
      if (_image != NULL)
      {
        if (strcmp((char*)_image->name,name) == 0)
        {
          image = _image;
        }
      }
      current = current->next;
    }
    return image;
  }
  
  int _gui_image_register( char *filename, char *name, int x, int y )
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":_gui_image_register\n");
    #endif
    if (filename == NULL || name == NULL)
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf(":gui_ImageRegister bad arguments\n");
      #endif
      return -1;
    }
    struct gui_Image *image = (struct gui_Image*)malloc(sizeof(struct gui_Image));
    strcpy((char*)image->filename,filename);
    strcpy((char*)image->name,name);
    gui_CreateImage(gui_GetMain(),image);
    
    if (x>0&&y>0)
    {
      image->x = x;
      image->y = y;
    }
    lst_images = trop_ListAdd(lst_images,image);
    return 0;
  }
  
  /** create an entry in the lst_images from an image
   */
  int gui_ImageRegister( char *filename, char *name, int x, int y )
  {
    return _gui_image_register(filename,name,x,y);
  }

  int gui_ImageRegisterSimple( char *filename, char *name )
  {
    return _gui_image_register(filename,name,-1,-1);
  }
  
  int form_MenuButtonPress( struct gui_Event *event )
  {
    struct gui_object *gobj	= event->widget->obj;
    struct gui_Widget *menu	= event->widget;
    struct gui_Widget *button	= NULL;
    struct gui_Event _event;
    int nhit = 0;
    while(gobj != NULL)
    {
      if (gobj->id == FORM_BUTTON)
      {
        button = (struct gui_Widget*)gobj->data;
        if (event->x >= (button->attr.x-menu->attr.x) &&
          event->x <= (button->attr.x-menu->attr.x)+button->attr.w &&
          event->y >= (button->attr.y-menu->attr.y) &&
          event->y <= (button->attr.y-menu->attr.y)+button->attr.h)
        {
          if (DEBUG == TRUE) printf("\tmenu.buttonPress (%d %d) %d\n",
            button->attr.x,button->attr.y,nhit);
          if (button->attr.onclick != NULL)
          {
            _event.widget = button;
            _event.x = event->x - (button->attr.x-menu->attr.x);
            _event.y = event->y - (button->attr.y-menu->attr.y);
            button->attr.onclick(&_event);
          }
          nhit++;
        }
      }
      gobj = gobj->next;
    }
    return 0;
  }
  
  int form_SlidebarButtonPress( struct gui_Event *event )
  {
    #ifdef _DEBUG
      if (DEBUG == TRUE) printf(":form_SlidebarButtonPress %d %d w(%d %d)\n",
        event->x, event->y, event->widget->attr.w, event->widget->attr.h);
    #endif
    struct gui_object *gobj 		= event->widget->obj;
    struct gui_Widget *button 		= NULL;
    int nb = 0;
    while(gobj != NULL)
    {
      if (gobj->id == FORM_BUTTON)
      {
        button = (struct gui_Widget*)gobj->data;
        
        if (nb==0)
        {
          if (event->y < event->widget->attr.h/2)
            button->attr.onclick(event);
        }
        else if (nb==1)
        {
          if (event->y > event->widget->attr.h/2)
            button->attr.onclick(event);
        }
        nb++;
      }
      gobj = gobj->next;
    }
    return 0;
  }
  
  int form_OverlayButtonPress( struct gui_Event *event )
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":form_OverlayButtonPress\n");
    #endif
    return 0;
  }
  
  /** expose progress bar 
   */
  int form_ProgressExpose( struct gui_Widget *w )
  {
    if (!w) return -1;
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("\t:progress expose\n");
    #endif
    gui_WidgetExpose(
      w,
      w->attr.x,
      w->attr.y,
      w->attr.w,
      w->attr.h);
    return 0;
  }
  
  int form_SlidebarExpose( struct gui_Widget *main, struct gui_Widget *w )
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("\t:slidebar expose\n");
    #endif
    struct gui_object *gobj 	= NULL;
    struct gui_object *_gobj	= NULL;
    struct gui_Image *image	= NULL;
    struct gui_GC *gc		= NULL;
    struct gui_Widget *button	= NULL;
    struct form_Menu *form	= (struct form_Menu*)w->form;

    gobj = (struct gui_object*)w->obj;
    int nb = 0;
    int x, y, _x, _y;
    while(gobj != NULL)
    {
      if (gobj->id == FORM_BUTTON)
      {
        button = (struct gui_Widget*)gobj->data;
        
        gui_WidgetExpose(
          button,
          button->attr.x,
          button->attr.y,
          button->attr.w,
          button->attr.h);
        
        image = NULL;
        if (nb == 0)
        {
          image = gui_ImagePick(IID_UP);
        }
        else
        {
          image = gui_ImagePick(IID_DOWN);
        }
        if (image != NULL)
        {
          _x += (w->attr.w - image->w) / 2;
          _y += (w->attr.h - image->h) / 2;
          if (_x < 0) _x = x;
          if (_y < 0) _y = y;
          #ifdef _USE_GUI_XLIB_INTERNALS
            XCopyArea(main->dpy,image->p,main->id,*main->gc,
              0,0,image->w,image->h,
              w->attr.x + _x,
              w->attr.y + _y);
          #else
            // todo
          #endif
        }
        nb++;
      }
      gobj = gobj->next;
    }
    
    return 0;
  }

  /** go back, move to past scene layout 
   */
  int gui_SceneBack( )
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":gui_SceneBack()\n");
    #endif
    struct gui_object *gobj = gobj_scene_history;
    struct gui_object *glast = NULL;
    if (gobj == NULL) return -1;
    while(gobj != NULL)
    {
      if (gobj->next == NULL)
      {
        if (gobj->data != NULL)
          gui_SceneSet(gobj->data);
        if (glast != NULL)
        {
          free(gobj);
          glast->next = NULL;
          break;
        }
        else
        {
          free(gobj);
          gobj_scene_history = NULL;
          break;
        }
      }
      glast = gobj;
      gobj = gobj->next;
    }
    return 0;
  }

  void *_gui_scene_back(void*data)
  {
    gui_SceneBack();
  }

  /**
   * saves the current scene to history
   *   1. save scene
   *   2. set new scene 
   */
  int gui_SceneSave( )
  {
    if (gobj_scene_history == NULL)
    {
      gobj_scene_history = (struct gui_object *)malloc(sizeof(struct gui_object));
      gobj_scene_history->data = scene;
      gobj_scene_history->next = NULL;
    }
    else
    {
      struct gui_object *gobj = gobj_scene_history;
      while(gobj != NULL)
      {	
        if (gobj->next == NULL)
        {
          gobj->next = (struct gui_object *)malloc(sizeof(struct gui_object));
          gobj->next->data = scene;
          gobj->next->next = NULL;
          break;
        }
        gobj = gobj->next;
      }
    }
    return 0;
  }

  int form_TitlebarInit( struct gui_Widget *w )
  {
    w->attr.w = gui_GetMain()->attr.w-(gui_GetMain()->attr.w/10);
    w->attr.h = gui_GetMain()->attr.h/12;
    w->attr.x = 0;
    w->attr.y = 0;
    w->attr.form = FORM_TITLEBAR;
    
    struct gui_Widget *back = gui_NewWidget();
    gui_WidgetInit(back,w);
    back->attr.x = 0;
    back->attr.y = 0;
    back->attr.h = w->attr.h;
    back->attr.w = w->attr.w/7;
    back->attr.form = FORM_BUTTON;
    back->attr.onclick = _gui_scene_back;
    #ifdef _USE_SKIN
    #else
      gui_WidgetSetColor(back,"#bbbbbb");
    #endif
    gui_WidgetMap(back);
    
    struct gui_object *current = w->obj;
    if (current == NULL)
    {
      w->obj = (struct gui_object *)malloc(sizeof(struct gui_object));
      w->obj->id = FORM_BUTTON;
      w->obj->data = back;
      w->obj->next = NULL;
    }
    else
    {
      current = (struct gui_object *)malloc(sizeof(struct gui_object));
      current->next = w->obj;
      current->id = FORM_BUTTON;
      current->data = back;
      w->obj = current;
    }
    
    return 0;
  }
  
  /** get absolute position of Widget 
   */
  int gui_WidgetGetAbsPos( struct gui_Widget *w, int *x, int *y )
  {
    *x = w->attr.x;
    *y = w->attr.y;
    gui_Widget *parent = w->parent;
    while(parent!=NULL)
    {
      if (parent->parent != NULL)
      {
        *x += parent->attr.x;
        *y += parent->attr.y;
      }
      else break;
      parent = parent->parent;
    }
    return 0;
  }
  
  /** expose images of a widget 
   */
  int gui_WidgetExposeImages( struct gui_Widget *w )
  {
    if (!w) return -1;
    struct gui_object *gobj	= NULL;
    struct gui_Image *image	= NULL;
    
    int ax;
    int ay;
    
    gui_WidgetGetAbsPos( w, &ax, &ay );
    
    gobj = w->obj;
    while(gobj != NULL)
    {
      if (gobj->id == GOID_IMAGE)
      {
        image = (struct gui_Image*)gobj->data;
        gui_DrawImage(
          image,
          image->x+ax,
          image->y+ay,
          image->w,
          image->h);
      }
      gobj = gobj->next;
    }
    
    return 0;
  }
  
  int form_TitlebarExpose( struct gui_Widget *w )
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":form_TitlebarExpose\n");
    #endif
    struct gui_object *gobj	= NULL;
    struct gui_object *_gobj	= NULL;
    struct gui_Widget *button	= NULL;
    struct gui_Image *image	= NULL;
    struct gui_GC *gc		= NULL;
    
    gobj = w->obj;
    while(gobj != NULL)
    {
      if (gobj->id == FORM_BUTTON)
      {
        button = (struct gui_Widget*)gobj->data;
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf(":form_TitlebarExpose.button\n");
        #endif
        _gobj = button->obj;
        while(_gobj != NULL)
        {
          if (_gobj->id == GOID_COLOR)
          {
            gc = (struct gui_GC*)_gobj->data;
            gui_DrawRectangle(gc,
              button->attr.x,
              button->attr.y,
              button->attr.w,
              button->attr.h);
          }
          _gobj = _gobj->next;
        }
        _gobj = button->obj;
        while(_gobj != NULL)
        {
          if (_gobj->id == GOID_IMAGE)
          {
            image = (struct gui_Image*)_gobj->data;
            #ifdef _USE_GUI_XLIB_INTERNALS
              XCopyArea(gui_GetMain()->dpy,image->p,gui_GetMain()->id,
                *gui_GetMain()->gc,
                0,0,image->w,image->h,
                button->attr.x,
                button->attr.y);
            #endif
          }
          _gobj = _gobj->next;
        }
      }
      gobj = gobj->next;
    }
    return 0;
  }
  
  int form_TitlebarButtonPress( struct gui_Event *event )
  {
    #ifdef _DEBUG
      if (DEBUG == TRUE) printf(":form_TitlebarButtonPress %d %d\n",
        event->x, event->y);
    #endif
    struct gui_object *gobj 	= event->widget->obj;
    struct gui_Widget *button 	= NULL;
    while(gobj != NULL)
    {
      if (gobj->id == FORM_BUTTON)
      {
        button = (struct gui_Widget*)gobj->data;   
        if (event->x > event->widget->attr.x + button->attr.x &&
          event->x < event->widget->attr.x + button->attr.w)
        {
          button->attr.onclick(event);
        }
      }
      gobj = gobj->next;
    }
    return 0;
  }
  
  int _form_MenuButtonCount( struct gui_Widget *menu ) // neds sync
  {
    static struct gui_object *gobj;
    static int ret;
    gobj = menu->obj;
    ret = 0;
    while(gobj != NULL)
    {
      if (gobj->id == FORM_BUTTON) ret++;
      gobj = gobj->next;
    }
    return ret;
  }
  
  void *form_MenuExposeCall( void*obj, int x, int y, int w, int h )
  {
    form_MenuExpose((struct gui_Widget*)obj,gui_GetMain());
    return NULL;
  }
  
  /** - deprecated, use canvas instead
   * a menu has a grid computation - first fill X, then fill Y
   */
  #define FILL_MARGINX 20
  #define FILL_MARGINY 20
  int form_MenuExpose( struct gui_Widget *main, struct gui_Widget *w )
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("\t:menu expose\n");
    #endif
    struct gui_object *gobj 	= NULL;
    struct gui_object *_gobj	= NULL;
    struct gui_Image *image	= NULL;
    struct gui_GC *gc		= NULL;
    struct gui_Font *font	= NULL;
    struct gui_Text *text	= NULL;
    struct gui_Widget *button	= NULL;
    struct form_Menu *form	= (struct form_Menu*)w->form;
    struct trop_List *gcurrent	= NULL; // grid
    struct gui_Grid *grid	= NULL;
    
    BOOL bskip_image		= FALSE;
    
    int i, x, y, nbutton = 0;
    int vw, vh, vx, vy;
    int gridsize = 0, lastsave = 0, beginsave = 0;
    int numbuttons = 0;
    int gridn = 0;
    
    form->xfill = 0;
    form->yfill = 0;
    
    #ifdef _DEBUG
      if (DEBUG == TRUE)
        printf(":menu members displacement %d %d\n",form->xmove,form->ymove);
    #endif
    
    gui_WidgetExpose(w,w->attr.x,w->attr.y,w->attr.w,w->attr.h);
    
    numbuttons = _form_MenuButtonCount( w );
    
    gcurrent = form->grid;
    while(gcurrent!=NULL)
    {
      ((gui_Grid*)gcurrent->ptr)->child = NULL;
      if (gcurrent==form->grid) 
        beginsave = ((gui_Grid*)form->grid->ptr)->begin;
      gridsize++;
      gcurrent = gcurrent->next;
    }
    
    gobj = w->obj;
    while(gobj != NULL)
    {
      if (gobj->id == FORM_BUTTON)
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("\t:menu button expose\n");
        #endif
        button = (struct gui_Widget*)gobj->data;
        
        if (gridsize>0)
        {
          button->attr.x = gui_GetMain()->attr.w; // todo
          button->attr.y = gui_GetMain()->attr.h;
          gcurrent = NULL;
          if (nbutton>=((gui_Grid*)form->grid->ptr)->begin)
          {
            gcurrent = form->grid;
            for (i = 0; i < gridn; i++)
            {
              gcurrent = gcurrent->next;
            }
            if (gcurrent)
            {
              grid = gcurrent->ptr;
              grid->child = button;
              button->attr.x = grid->x+((grid->w-button->attr.w)/2);
              button->attr.y = grid->y+((grid->h-button->attr.h)/2);
              button->attr.x += w->attr.x;
              button->attr.y += w->attr.y;
            
              if (gridn!=gridsize) gridn++;
            }
          }
        }
        else
        {
          button->attr.x = form->xfill + w->attr.x + form->xmove; // absolute for toplevel calculations 'Button Press'
          button->attr.y = form->yfill + w->attr.y + form->ymove;
        }
        
        nbutton++;
        
        if (gridsize>0 && gcurrent==NULL)
        {
          goto SKIP_BUTTON;
        }
        
        #ifdef _DEBUG
          if (DEBUG==TRUE && gridsize>0) 
            printf(":button_draw at (%d %d)\t(%d %d)\tg(%d %d)\n",
            button->attr.x-w->attr.x,
            button->attr.y-w->attr.y,
            button->attr.w,
            button->attr.h,
            grid->x,
            grid->y);
        #endif
        
        gui_WidgetExpose(button,
          button->attr.x-w->attr.x,
          button->attr.y-w->attr.y,
          button->attr.w,
          button->attr.h);
          
      goto SKIP_BUTTON; // a section is not reached if not called
      SKIP_BUTTON:
        if (form->grid==NULL)
        {
          if (form->xfill + button->attr.w + FILL_MARGINX >= w->attr.w )
          {
            form->xfill = 0;
            form->yfill += button->attr.h;
          }
          else
          {
            form->xfill += button->attr.w;
          }
        } 
      }
      gobj = gobj->next;
    }
    
    #ifdef _DEBUG
      if (DEBUG == TRUE) printf("\t:menu button expose end\n");
    #endif
    return 0;
  }
  
  int form_OverlayExpose( struct gui_Widget *w )
  {
    struct gui_Widget *button 	= NULL;
    struct gui_object *gobj 	= w->obj;
    struct gui_object *_gobj 	= NULL;
    struct gui_GC *gc		= NULL;
    while(gobj != NULL)
    {
      if (gobj->id == FORM_BUTTON)
      {
        button = (struct gui_Widget*)gobj->data;
        _gobj = button->obj;
        while(_gobj != NULL)
        {
          if (_gobj->id == GOID_COLOR)
          {
            #ifdef _USE_GUI_XLIB_INTERNALS
              gc = (struct gui_GC*)_gobj->data;
              XFillRectangle(gui_GetMain()->dpy,gui_GetMain()->id,
                gc->gc,
                button->attr.x+w->attr.x,
                button->attr.y+w->attr.y,
                button->attr.w,
                button->attr.h);
            #endif
          }
          _gobj = _gobj->next;
        }
      }
      gobj = gobj->next;
    }
    return 0;
  }
  
  int form_OverlayInit( struct gui_Widget *w )
  {
    w->attr.w = gui_GetMain()->attr.w/4*3;
    w->attr.h = gui_GetMain()->attr.h/2;
    w->attr.x = (gui_GetMain()->attr.w/2)-(w->attr.w/2);
    w->attr.y = (gui_GetMain()->attr.h/2)-(w->attr.h/2);
    gui_WidgetToTop(w);
    return 0;
  }
  
  /*
    initialize the Menu form structure members 
  */
  int form_MenuInit( struct gui_Widget *w ) // todo
  {
    if (!w) return -1;
    ((struct form_Menu*)w->form)->grid_ew = -1;
    ((struct form_Menu*)w->form)->grid_eh = -1;
    ((struct form_Menu*)w->form)->grid = NULL;
    w->attr.onexposeEx = form_MenuExposeCall;
    return 0;
  }
  
  /*
    set a grid, where ew and eh are the element size
  */
  int form_MenuSetGrid( struct gui_Widget *w, int ew, int eh )
  {
    if (!w) return -1;
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":form_MenuSetGrid\n");
    #endif
    ((struct form_Menu*)w->form)->grid_ew = ew;
    ((struct form_Menu*)w->form)->grid_eh = eh;
    struct gui_Grid *grid = NULL;
    int gx = w->attr.w/ew;
    int gy = w->attr.h/eh;
    int _gx, _gy, ex = 0, ey = 0;
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":form_MenuSetGrid (%dx%d)\n",gx,gy);
    #endif
    for (_gy = 0; _gy < gy; _gy++)
    {
      for (_gx = 0; _gx < gx; _gx++)
      {
        grid = (struct gui_Grid*)malloc(sizeof(struct gui_Grid));
        if (_gy == 0 && _gx == 0)
        {
          grid->step = 0;
          grid->move = -1;
          grid->begin = 0;
        }
        grid->w = ew;
        grid->h = eh;
        grid->x = ex;
        grid->y = ey;
        ex+=ew;
        ((struct form_Menu*)w->form)->grid = trop_ListAdd(((struct form_Menu*)w->form)->grid,grid);
      }
      ey+=eh;
      ex = 0;
    }
    if (DEBUG==TRUE)
    {
      struct trop_List *current = ((struct form_Menu*)w->form)->grid;
      while(current)
      {
        grid = current->ptr;
        #ifdef _DEBUG
          if (DEBUG==TRUE)
            printf(":menu_grid elem(%d,%d)\n",grid->x,grid->y);
        #endif
        current = current->next;
      }
    }
    return 0;
  }
  
  struct gui_Grid *gui_GetGrid( struct gui_Widget *w )
  {
    if (!w) return NULL;
    if (w->attr.form!=FORM_MENU) return NULL;
    if (((struct form_Menu*)w->form)->grid!=NULL)
      return (gui_Grid*)((struct form_Menu*)w->form)->grid->ptr;
    else
      return NULL;
  }
  
  /** set the stepping of the grid movement 
   */
  int gui_SetGridStep( struct gui_Widget *w, int step )
  {
    struct gui_Grid *grid = gui_GetGrid(w);
    if (grid!=NULL)
    {
      grid->step = step;
    }
    return 0;
  }
  
  int form_MenuMoveElements( struct gui_Widget *w, int x, int y )
  {
    struct form_Menu *form = (struct form_Menu*)w->form;
    if (form == NULL) return -1;
    form->xmove += x;
    form->ymove += y;
    return 0;
  }
  
  /** move menu by the height of the highest menu button ( at the Y axis )
   */
  int form_MenuMoveElementsExY( struct gui_Widget *w, int direction )
  {
    struct form_Menu *form 	= (struct form_Menu*)w->form;
    if (form == NULL) return -1;
    struct gui_object *gobj 	= NULL;
    struct gui_Widget *button 	= NULL;
    gobj 			= w->obj;
    int ymove 		= 0;
    int size 		= 0;
    int highest 	= -1;
    int lowest 		= w->attr.h;
    while(gobj != NULL)
    {
      if (gobj->id == FORM_BUTTON)
      {
        button = (struct gui_Widget*)gobj->data;
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("xl.button %d %d\n",button->attr.x,button->attr.y);
        #endif
        if (size < button->attr.h)
          size = button->attr.h;
        
        if (highest == -1 || ((button->attr.y-w->attr.y) + button->attr.h > highest))
          highest = (button->attr.y-w->attr.y) + button->attr.h; 
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf(":xl %d\n",button->attr.y);
        #endif
        if ((button->attr.y-w->attr.y) < lowest)
          lowest = button->attr.y-w->attr.y;
      }
      gobj = gobj->next;
    }
    if (direction == DDOWN)
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf(":down %d %d\n",size,highest);
      #endif
      if (highest -size < w->attr.h)
        size = highest - w->attr.h;
      else if (highest <= w->attr.h)
        size = 0;
      ymove = -size;
    }
    else
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf(":up %d %d\n",size,lowest);
      #endif
      if (lowest + size > 0)
        size = -lowest;
      else if (lowest == 0)
        size = 0; 
      ymove = size;
    }
    form->ymove += ymove;
    return 0;
  }
  
  int _form_MenuMoveUp( struct gui_Widget *w )
  {
    static struct trop_List *current = NULL;
    static int move = 0;
    if (((form_Menu*)w->form)->grid==NULL)
      form_MenuMoveElementsExY( w, DUP );
    else
    {
      move = 0;
      current = ((form_Menu*)w->form)->grid;
      while(current!=NULL)
      {
        if (current==((form_Menu*)w->form)->grid)
        {
          if (((gui_Grid*)current->ptr)->step!=0)
          {
            move = ((gui_Grid*)current->ptr)->step;
            break;
          }
        }        
        move++;
        current = current->next;
      }
      ((gui_Grid*)((form_Menu*)w->form)->grid->ptr)->move = DUP;
      if (((gui_Grid*)((form_Menu*)w->form)->grid->ptr)->begin > 0)
      {
        if (((gui_Grid*)((form_Menu*)w->form)->grid->ptr)->begin > move-1)
          ((gui_Grid*)((form_Menu*)w->form)->grid->ptr)->begin -= move;
        else
        {
        }
      }
    }
    gui_TriggerExpose( gui_GetMain() );
    return 0;
  }
  
  BOOL _form_MenuIsMovableDown( struct gui_Widget *w ) // todo, to .h
  {
    struct trop_List *current = ((form_Menu*)w->form)->grid;
    while(current!=NULL)
    {
      if (((gui_Grid*)current->ptr)->child==NULL) return FALSE;
      current = current->next;
    }
    return TRUE;
  }
  
  int _form_MenuMoveDown( struct gui_Widget *w )
  {
    static struct trop_List *current = NULL;
    static int move = 0;
    if (((form_Menu*)w->form)->grid==NULL)
      form_MenuMoveElementsExY( w, DDOWN );
    else
    {
      move = 0;
      current = ((form_Menu*)w->form)->grid;
      while(current!=NULL)
      {
        if (current==((form_Menu*)w->form)->grid)
        {
          if (((gui_Grid*)current->ptr)->step!=0)
          {
            move = ((gui_Grid*)current->ptr)->step;
            break;
          }
        }
        move++;
        current = current->next;
      }
      if (_form_MenuIsMovableDown(w) == TRUE)
      {
        ((gui_Grid*)((form_Menu*)w->form)->grid->ptr)->move = DDOWN;
        ((gui_Grid*)((form_Menu*)w->form)->grid->ptr)->begin += move;
      }
    }
    gui_TriggerExpose( gui_GetMain() );
    return 0;
  }
  
  void *form_MenuMoveUp( void *data )
  {
    #ifdef _DEBUG
      if (DEBUG == TRUE) printf(":form_MenuMoveUp\n");
    #endif
    struct gui_Event *event = (struct gui_Event*)data;
    struct form_Slidebar *sbar = (struct form_Slidebar*)event->widget->form;
    _form_MenuMoveUp(sbar->relay);
  }
  
  void *form_MenuMoveDown( void *data )
  {
    #ifdef _DEBUG
      if (DEBUG == TRUE) printf(":form_MenuMoveDown\n");
    #endif
    struct gui_Event *event = (struct gui_Event*)data;
    struct form_Slidebar *sbar = (struct form_Slidebar*)event->widget->form;
    _form_MenuMoveDown(sbar->relay);
  }
  
  int form_MenuResetElementsPosition( struct gui_Widget *w )
  {
    struct form_Menu *form = (struct form_Menu*)w->form;
    if (form == NULL) return -1;
    form->xmove = 0;
    form->ymove = 0;
    return 0;
  }
  
  int form_AddButton( struct gui_Widget *w, struct gui_Widget *b )
  {
    struct gui_object *current = w->obj;
    if (current == NULL)
    {
      w->obj = (struct gui_object *)malloc(sizeof(struct gui_object));
      w->obj->id = FORM_BUTTON;
      w->obj->data = b;
      w->obj->next = NULL;
    }
    else
    {
      current = (struct gui_object *)malloc(sizeof(struct gui_object));
      current->next = w->obj;
      current->id = FORM_BUTTON;
      current->data = b;
      w->obj = current;
    }
  }
  
  /** progress bar init 
   */
  int form_ProgressInit( struct gui_Widget *w )
  {
    if (!w) return -1;
    w->attr.form = FORM_PROGRESS;
    w->form = (struct form_Progress*)malloc(sizeof(struct form_Progress));
    ((struct form_Progress*)w->form)->high = -1;
    ((struct form_Progress*)w->form)->value = -1;
    return 0;
  }
  
  int form_ProgressSetValue( struct gui_Widget *w, int high, int value )
  {
    if (!w || high<-1 || value<-1) return -1;
    ((struct form_Progress*)w->form)->high = high;
    ((struct form_Progress*)w->form)->value = value;
    return 0;
  }
  
  int form_ProgressInitEx( struct gui_Widget *parent, struct gui_Widget *w )
  {
    if (!parent || !w) return -1;
    return 0;
  }
  
  int form_SlidebarSetGeometry( struct gui_Widget *main, struct gui_Widget *w )
  {
    w->attr.y = 0;
    w->attr.x = (int)(main->attr.w*0.9);
    w->attr.h = main->attr.h;
    w->attr.w = main->attr.w/10;
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":slidebar geometry %d %d %d %d\n",
        w->attr.x, w->attr.y, w->attr.w, w->attr.h );
    #endif
    return 0;
  }
  
  int form_SlidebarInit( struct gui_Widget *main, struct gui_Widget *w )
  {
    #ifdef _DEBUG
      if (DEBUG == TRUE) printf(":form_SlidebarInit\n");
    #endif
    struct gui_Widget *b00 = gui_NewWidget();
    struct gui_Widget *b01 = gui_NewWidget();

    b00->attr.w = w->attr.w;
    b00->attr.h = w->attr.h/2;
    b00->attr.x = 0;
    b00->attr.y = 0;
    b01->attr.w = w->attr.w;
    b01->attr.h = w->attr.h/2;
    b01->attr.x = 0;
    b01->attr.y = b00->attr.h;
    
    b00->attr.form = FORM_BUTTON;
    b01->attr.form = FORM_BUTTON;
    
    b00->attr.onclick = &form_MenuMoveDown;
    b01->attr.onclick = &form_MenuMoveUp;
    
    gui_WidgetInit(b00,w);
    gui_WidgetInit(b01,w);
    
    gui_WidgetMap(b00);
    gui_WidgetMap(b01);
    
    #ifdef _USE_SKIN
    #else
      gui_WidgetSetColor(b00,"#ffffff");
      gui_WidgetSetColor(b01,"#dfdfdf");
    #endif
    
    form_AddButton(w,b00);
    form_AddButton(w,b01);
    
    return 0;
  }
  
  int form_SlidebarSetRelay( struct gui_Widget *w, struct gui_Widget *relay )
  {
    struct form_Slidebar *sbar = (struct form_Slidebar*)w->form;
    sbar->relay = relay;
    return 0;
  }
  
  int gui_WidgetsRemoveAllObj( struct gui_Widget *w )
  {
    struct gui_object *current = w->obj;
    while(current != NULL)
    {
      if (current->id == GOID_TEXT)
      {
        
      }
      current = current->next;
    }
    return 0;
  }
  
  /**
   *  init GC by Hex color value 
  */
  struct gui_GC *gui_InitGCEx( struct gui_GC *gc, char *hex )
  {
    if (!gui_GetMain() || !hex) return NULL;
    if (gc==NULL)
      gc = (struct gui_GC*)malloc(sizeof(struct gui_GC));
      
    gc->colorl = (struct gui_ColorLow*)malloc(sizeof(struct gui_ColorLow));
    if (strlen(hex)==strlen("trans"))
    {
      if (strcmp(hex,"trans")==0)
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("__trans gui_InitGCEx\n");
        #endif
        gc->colorl->value = C_TRANS_VALG;
        return gc;
      }
    }
    
    _hex2unsignedChar(
      hex,
      &gc->colorl->r,
      &gc->colorl->g,
      &gc->colorl->b,
      &gc->colorl->a
    );
    
    return gc;
  }

  int _init_main_gc( struct gui_Widget *w )
  {
    static BOOL brun = FALSE;
    
    if (brun==TRUE) return 0;
    brun = TRUE;
    
    if (!w) return -1;
    
    return 0;
  }
 
  #ifdef JPEG
    void __jpeg_cb_error_exit(j_common_ptr cinfo)
    {
      printf("jpeg_error\n");
    }
  #endif

  void *_test_jpeg_handling( char *filename )
  {
    #ifdef JPEG
      struct jpeg_decompress_struct cinfo;
      struct jpeg_decompress_struct *ciptr;
      struct jpeg_error_mgr *emgr = NULL;
      FILE *fd = NULL;
      JSAMPLE *lb;
      ciptr = &cinfo;
      
      if ((fd=fopen((char*)filename,"rb"))==NULL)
      {
        printf("failed to open jpeg (%s)\n",(char*)filename);
        return NULL;
      }
      else
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) 
            printf(":opened '%s'\n",(char*)filename);
        #endif
      }
      emgr = malloc(sizeof(struct jpeg_error_mgr));
      if (!emgr)
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) 
            printf(":could not allocate jpeg error manager\n");
        #endif
        return NULL;
      }
      ciptr->err = jpeg_std_error(emgr);
      
      jpeg_create_decompress(ciptr);
      jpeg_stdio_src(ciptr,fd);
      jpeg_read_header(ciptr,1);
      ciptr->out_color_space = JCS_RGB;
      jpeg_start_decompress(ciptr);
    #endif
    
    return NULL;
  }

  /** create a gui_Image from a jpeg image 
   */
  struct gui_Image *_gui_jpeg_to_image( struct gui_Image *i ) // jpeg
  {
    #ifdef JPEG
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("_gui_jpeg_to_image\n");
      #endif
      if (i==NULL)
      {
        return NULL;
      }
    
      struct jpeg_decompress_struct cinfo;
      struct jpeg_decompress_struct *ciptr;
      struct jpeg_error_mgr *emgr = NULL;
      FILE *fd = NULL;
      JSAMPLE *lb;
      ciptr = &cinfo;
      
      #ifdef _USE_GUI_XLIB_INTERNALS
        if (gui_GetMain()) // todo: move
        {
          if (gui_GetMain()->gc == NULL && gui_GetMain()->gcv == NULL)
          {
            _init_main_gc(gui_GetMain());
          }
        }
      #endif
      
      if ((fd=fopen((char*)i->filename,"rb"))==NULL)
      {
        printf("failed to open jpeg (%s)\n",(char*)i->filename);
        return NULL;
      }
      else
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) 
            printf(":opened '%s'\n",(char*)i->filename);
        #endif
      }
      emgr = malloc(sizeof(struct jpeg_error_mgr));
      if (!emgr)
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) 
            printf(":could not allocate jpeg error manager\n");
        #endif
        return NULL;
      }
      ciptr->err = jpeg_std_error(emgr);
      #ifndef _DEBUG
        emgr->error_exit = __jpeg_cb_error_exit;
      #endif
      
      jpeg_create_decompress(ciptr);
      jpeg_stdio_src(ciptr,fd);
      jpeg_read_header(ciptr,1);
      ciptr->out_color_space = JCS_RGB;
      jpeg_start_decompress(ciptr);

      static int h, w, cc, xs, yn;
      static unsigned char pixel[4];
      pixel[3] = 0xff;
      
      w = ciptr->output_width;
      h = ciptr->output_height;
      cc = ciptr->output_components;
      i->w = w;
      i->h = h;
      i->x = 0;
      i->y = 0;

      #ifdef _DEBUG
        if (DEBUG==TRUE)
          printf("jpeg_image_attr output_height(%d) comp(%d)\n",
            ciptr->output_height,
            cc);
      #endif
      
      #ifdef _USE_GUI_XLIB_INTERNALS
        i->ximage = _gui_surface_init_ex(NULL,data,w,h);
      #else
        #ifdef _DEBUG
          printf("jpeg_image_attr h(%d) w(%d)\n",w,h);
        #endif
        i->data = malloc(h*w*4); // todo, memory handling
      #endif
      
      #ifdef _DEBUG
        printf("a00\n");
      #endif
      
      lb = (JSAMPLE *)(*ciptr->mem->alloc_small)(
        (j_common_ptr)ciptr,
        JPOOL_PERMANENT,
        cc*w);
            
      yn = 0;
      while(ciptr->output_scanline < ciptr->output_height)
      {
        jpeg_read_scanlines(ciptr,&lb,1);
        for (xs = 0; xs < w; xs++)
        {
          pixel[2] = *(lb+3*xs);
          pixel[1] = *(lb+3*xs+1);
          pixel[0] = *(lb+3*xs+2);
          #ifdef _USE_GUI_XLIB_INTERNALS
            XPutPixel(i->ximage,xs,yn,*(long*)(&pixel));
          #else
            __put_pixel( i->data, w, h,
                         xs,yn,
                         bigInterface->bpp/8,
                         (char*)&pixel );
          #endif
        }
        yn++;
      }
      
      jpeg_finish_decompress(ciptr);
      jpeg_destroy_decompress(ciptr);
      
      fclose(fd);
      
      #ifdef _DEBUG
        printf("a01\n");
      #endif
      
      #ifdef _DEBUG
        if (DEBUG==TRUE)
          printf("read jpeg file finish\n");
      #endif
      
      return i;
    #else
      return NULL;
    #endif
  }
  
  int gui_ImageFreeContent( struct gui_Image *i )
  {
    if (!i) return -1;
    
    free(i->data);
    i->data = NULL;
    
    return 0;
  }
  
  int gui_ImageFree( struct gui_Image *i )
  {
    if ( gui_ImageFreeContent(i) != 0 ) return -1;
    free(i);
    return 0;
  }
  
  /**
   *  is an interface for loading different kinds of images
   */
  int gui_CreateImage( struct gui_Widget *main, struct gui_Image *i )
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":gui_CreateImage\n");
    #endif
    if (main==NULL) main = gui_GetMain();
    
    if (!i)
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf(":gui_CreateImage image=NULL\n");
      #endif
    }
    
    if (DEBUG==TRUE) printf(":opening (%s)\n",(char*)i->filename);
    
    i->round = 0;
    i->data = NULL;
    
    if (
      strstr((char*)i->filename,".jpg") != NULL ||
      strstr((char*)i->filename,".jpeg") != NULL ||
      strstr((char*)i->filename,".JPG") != NULL ||
      strstr((char*)i->filename,".JPEG") != NULL
      )
    {
      i->type = IMG_JPG;
      if (_gui_jpeg_to_image(i)==NULL)
      {
        printf("Can't process jpeg image (%s)\n",(char*)i->filename);
        return -1;
      }
      return 0;
    }
    else if 
    (
      strstr((char*)i->filename,".xpm") != NULL ||
      strstr((char*)i->filename,".XPM") != NULL
    )
    {
      i->type = IMG_XPM;
    }
    
    /**
     *  todo, implement support for XPM, embed libxpm
    */
    #ifdef _USE_GUI_XLIB_INTERNALS
    
      FILE *fimage = fopen((char*)i->filename,"r");
      if (!fimage)
      {
        printf("Can't open image file (%s)\n",(char*)i->filename);
        goto IMAGE_ERROR;
      }
      fclose(fimage);
      XpmAttributes attr;
      attr.visual = DefaultVisual(main->dpy,main->screen);
      attr.colormap = XDefaultColormap(main->dpy,main->screen);
      attr.valuemask = XpmVisual|XpmColormap;
    
      int status = XpmReadFileToPixmap(main->dpy,RootWindow(main->dpy,main->screen),
        (char*)i->filename,&i->p,&i->m,&attr);
      if (status == XpmSuccess)
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf(":status == XpmSuccess\n");
        #endif
        i->w = attr.width;
        i->h = attr.height;
        i->x = 0;
        i->y = 0;
        i->ximage = XGetImage(
          gui_GetMain()->dpy,
          i->p,
          0,0,
          i->w,i->h,
          AllPlanes,
          ZPixmap);
      }
      else
        goto IMAGE_ERROR;
      return 0;
    IMAGE_ERROR:
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf(":statux != XpmSuccess\n");
      #endif
      i->w = 0;
      i->h = 0;
      i->x = 0;
      i->y = 0;
      i->ximage = NULL;
    #endif
    return -1;
  }
  
  /** image to be displayed with round edges 
  */
  int gui_ImageSetRound( struct gui_Image *i, int round )
  {
    if (!i || round<1) return -1;
    i->round = round;
    return 0;
  }
  
  int gui_ImagePosition( struct gui_Image *i, int x, int y )
  {
    i->x = x;
    i->y = y;
    return 0;
  }
  
  struct gui_Image *gui_ImagePosCenter( struct gui_Image *image, struct gui_Widget *wi )
  {
    if (!image||!wi) return NULL;
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("gui_ImagePosCenter (%d-%d)/2\n",wi->attr.w,image->w);
    #endif
    image->x = (wi->attr.w-image->w)/2;
    image->y = (wi->attr.h-image->h)/2;
    return image;
  }
  
  struct gui_Image *gui_ImagePosLeft( struct gui_Image *image, struct gui_Widget *wi )
  {
    if (!image||!wi) return NULL;
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("gui_ImagePosLeft (%d-%d)/2\n",wi->attr.w,image->w);
    #endif
    image->x = 0;
    image->y = 0;
    return image;
  }
  
  #define _RESTRICT_IMAGE // only one image for one gui_Widget
  int _gui_widget_add_gobj(
    struct gui_Widget *w,
    void *gobj,
    unsigned int value,
    int id
  )
  {
    struct gui_object *current = w->obj;
    struct gui_object *next = NULL;
    struct gui_object *prev = w->obj;
    
    #ifdef _RESTRICT_IMAGE
      if (id==GOID_IMAGE)
      {
        while(current)
        {
          if (current->id==GOID_IMAGE)
          {
            current->data = gobj;
            return 0;
          }
          current = current->next;
        }
      }
    #endif
    
    current = w->obj;
    if (current == NULL)
    {
      w->obj = (struct gui_object *)malloc(sizeof(struct gui_object));
      w->obj->id = id;
      w->obj->data = gobj;
      w->obj->value = value;
      w->obj->next = NULL;
    }
    else
    {
      current = (struct gui_object *)malloc(sizeof(struct gui_object));
      current->next = w->obj;
      current->id = id;
      current->data = gobj;
      current->value = value;
      w->obj = current;
    }
    return 0;
  }

  int form_MenuAddButton( struct gui_Widget *w, struct gui_Widget *b )
  { return _gui_widget_add_gobj(w,b,0,FORM_BUTTON); }

  
  int gui_WidgetAddStr( struct gui_Widget *w, char *str )
  {
    struct gui_object *current;
    struct gui_Str *gstr;
    
    if (!w||!str) return -1;
    
    gstr = (struct gui_Str*)malloc(sizeof(struct gui_Str*));
    if (strlen(str)<GUI_STR_LEN)
      strcpy((char*)gstr->data,str);
    else
    {
      strncpy((char*)gstr->data,str,GUI_STR_LEN);
      gstr->data[GUI_STR_LEN] = 0x00;
    }
    
    return _gui_widget_add_gobj(w,gstr,0,GCFG_STR);
  }
  
  /** we interpret a color as hex when there is a '#' in the beginning 
  */
  int gui_WidgetSetColor( struct gui_Widget *w, char *name )
  {
    static struct gui_object *current 	= NULL;
    static struct gui_GC *gc 		= NULL;
    static BOOL	btrans;
    
    #ifdef _DEBUG
      printf("gui_WidgetSetColor\n");
    #endif
    
    if (!w||!name) return -1;
    
    btrans = FALSE;
    
    if (name[0] != '#')
    {
      if (strlen(name)==strlen("trans"))
      {
        if (strcmp(name,"trans")==0)
        {
          #ifdef _DEBUG
            if (DEBUG==TRUE) printf("__trans gui_WidgetSetColor\n");
          #endif
          btrans = TRUE;
        }
      }
    }
    
    /**
     *  setup existing
    */
    current = w->obj;
    while(current != NULL)
    {
      if (current->id == GOID_COLOR)
      {
        gc = (struct gui_GC*)current->data;
        
        free(gc->colorl);
        gc->colorl = (struct gui_ColorLow*)malloc(sizeof(struct gui_ColorLow));
        
        if (btrans == TRUE)
        {
          gc->colorl->value = C_TRANS_VALG;
          return 0;
        }
        
          if (name[0]=='#')
            gui_InitGCEx(gc,name+1); 
          else
            gui_InitGCEx(gc,name);
        
        return 0;
      }
      current = current->next;
    }
    /**
     *  create new
    */
    gc = (struct gui_GC*)malloc(sizeof(struct gui_GC));
    
    if (name[0] == '#' || btrans == TRUE)
    {
      if (name[0]=='#')
        gui_InitGCEx(gc,name+1); 
      else
        gui_InitGCEx(gc,name);
    }
    
    return _gui_widget_add_gobj(w,gc,0,GOID_COLOR);
  }
  
  int gui_WidgetResetColor( struct gui_Widget *widget )
  {
    if (!widget)
    {
      return 1;
    }
    
    struct gui_object *current 	= widget->obj;
    struct gui_object *prev = NULL;
    struct gui_object *next = NULL;
    while(current)
    {
      next = current->next;
      if (current->id==GOID_COLOR)
      {
        if (prev)
          prev->next = next;
        else
          widget->obj = next;
        break;
      }
      prev = current;
      current = next;
    }
    
    return 0;
  }
  
  /** todo - refactoring, set the 'to' color
   */
  int gui_WidgetSetColor2( struct gui_Widget *w, char *name )
  {
    static struct gui_object *current 	= NULL;
    static struct gui_GC *gc 		= NULL;
    static BOOL	btrans;
    
    if (!w||!name) return -1;
    
    btrans = FALSE;
    
    if (name[0] != '#')
    {
      if (strlen(name)==strlen("trans"))
      {
        if (strcmp(name,"trans")==0)
        {
          #ifdef _DEBUG
            if (DEBUG==TRUE) printf("__trans gui_WidgetSetColor\n");
          #endif
          btrans = TRUE;
        }
      }
    }
    
    /**
     *  setup existing
    */
    current = w->obj;
    while(current != NULL)
    {
      if (current->id == GOID_COLOR2)
      {
        gc = (struct gui_GC*)current->data;
        
        free(gc->colorl);
        gc->colorl = (struct gui_ColorLow*)malloc(sizeof(struct gui_ColorLow));
        
        if (btrans == TRUE)
        {
          gc->colorl->value = C_TRANS_VALG;
          return 0;
        }
        
          if (name[0]=='#')
            gui_InitGCEx(gc,name+1); 
          else
            gui_InitGCEx(gc,name);
        
        return 0;
      }
      current = current->next;
    }
    /**
     *  create new
    */
    gc = (struct gui_GC*)malloc(sizeof(struct gui_GC));
    
    if (name[0] == '#' || btrans == TRUE)
    {
      if (name[0]=='#')
        gui_InitGCEx(gc,name+1); 
      else
        gui_InitGCEx(gc,name);
    }
    
    return _gui_widget_add_gobj(w,gc,0,GOID_COLOR2);
  }
  
  struct gui_Scene *gui_NewScene( )
  {
    struct gui_Scene *_scene = (struct gui_Scene*)malloc(sizeof(struct gui_Scene));
    _scene->list = NULL;
    gui_SceneAddMember(_scene,gui_GetMain());
    return _scene;
  }
  
  struct gui_Scene *gui_SceneCurrent( )
  {
    return scene;
  }
  
  int gui_SceneNone( )
  {
    scene = NULL;
    return 0;
  }
  
  int gui_SceneSet( struct gui_Scene *_scene )
  {
    scene = _scene;
    return 0;
  }
  
  int gui_SceneAddMember( struct gui_Scene *_scene, struct gui_Widget *member )
  {
    if (!_scene || !member) return -1;
    _scene->list = trop_ListAdd(_scene->list,member);
    return 0;
  }
  
  BOOL gui_SceneIsWidget( struct gui_Scene *_scene, struct gui_Widget *w )
  {
    if (_scene == NULL)
      return TRUE;
    struct trop_List *list = _scene->list;
    while(list != NULL)
    {
      if (list->ptr == w)
        return TRUE;
      list = list->next;
    }
    return FALSE;
  }

  /** extended scene manipulation based on xml setup 
  */
  struct gui_SceneEx *gui_SceneExInit( struct gui_SceneEx *s, char *name )
  {
    static char filename[256];
    if (!name) return NULL;
    if (!s)
    {
      s = (struct gui_SceneEx*)malloc(sizeof(struct gui_SceneEx));
      s->list = NULL;
    }
    strcpy((char*)s->name,name);
    sprintf((char*)filename,"scenes/%s.xml",name);
    /*
    FILE *fd = NULL;
    if (!(fd = fopen((char*)filename,"r")))
    {
      return NULL;
    }
    fclose(fd);
    */
    return s;
  }
/*
  struct gui_Color *gui_NewColor( char *name )
  {
    struct gui_Color *color;
    color = (struct gui_Color*)malloc(sizeof(struct gui_Color));
    if ( gui_InitGCEx(&color->gc,name) != 0 )
    {
      free(color);
      return NULL;
    }
    return color;
  }
*/  
  /** largely ineffective to hold the whole structure / only holding objects is better
  */
  struct gui_PixBuf *gui_NewPixBuf( int w, int h )
  {
    if (w < 1 || h < 1)
      return NULL;
    if (w > 4096) w = 4096;
    if (h > 4096) h = 4096;
    struct gui_PixBuf *pb;
    pb = (struct gui_PixBuf*)malloc(sizeof(struct gui_PixBuf));
    pb->w = w;
    pb->h = h;
    pb->pixel = (struct gui_Pixel*)malloc(sizeof(struct gui_Pixel));
    pb->pixel->next = NULL;
    struct gui_Pixel *current = pb->pixel;
    int i;
    for (i = 1; i < w*h; i++)
    {
      current->next = (struct gui_Pixel*)malloc(sizeof(struct gui_Pixel));
      current = current->next;
      current->color = NULL;
    }
    current->next = NULL;
    return pb;
  }
  
  /* 
    draw the pixel buffer to the window 'to the main widget'
  */
  int gui_DrawPixBuf( struct gui_Widget *main, struct gui_PixBuf *pb )
  {
    int x = 1, y = 1;
    struct gui_Pixel *current = pb->pixel;
    while(current != NULL)
    {
      #ifdef _USE_GUI_XLIB_INTERNALS
        if (current->color != NULL)
          XDrawPoint(main->dpy,main->id,current->color->gc.gc,x,y);
      #endif
      current = current->next;
      x++;
      if (x > main->attr.w)
      {
        x = 1;
        y++;
      }
    }
    return 0;
  }
  
  int _rec_expose_free_bits( struct rec_Expose *rec )
  {
    static struct _rec_expose_bit *cbit 	= NULL;
    static struct _rec_expose_bit *_cbit	= NULL;
    if (!rec) return -1;
    cbit = rec->bits;
    while(cbit!=NULL)
    {
      if (cbit->type == REC_EXP_RECT || cbit->type == REC_EXP_TXT ||
        cbit->type == REC_EXP_NONE)
      {
        free(cbit->sub);
      }
      else if (cbit->type == REC_EXP_IMG)
      {
      }
      _cbit = cbit;
      cbit = cbit->next;
      free(_cbit);
    }
    rec->bits = NULL;
    return 0;
  }
  
  struct rec_Expose *rec_ExposeInit( struct rec_Expose *rec )
  {
    if (rec==NULL)
    {
      rec = (struct rec_Expose*)malloc(sizeof(struct rec_Expose));
      rec->bits = NULL;
    }
    else
    {
      if (rec->bits!=NULL)
      {
        _rec_expose_free_bits(rec);
      }
    }
    return rec;
  }
  
  int _rec_expose_numbits( struct rec_Expose *rec )
  {
    struct _rec_expose_bit *cbit	= NULL;
    static int num = 0;
    if (!rec) return -1;
    num = 0;
    cbit = rec->bits;
    while(cbit != NULL)
    {
      num++;
      cbit = cbit->next;
    }
    return num;
  }
  
  int rec_ExposePrint( struct rec_Expose *rec )
  {
    struct _rec_expose_bit *cbit	= NULL;
    struct _rec_expose_rect *crect	= NULL;
    struct _rec_expose_txt *ctxt	= NULL;
    if (!rec) return -1;
    cbit = rec->bits;
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("exposure record listing:\n");
    #endif
    while(cbit != NULL)
    {
      if (cbit->type == REC_EXP_RECT)
      {
        crect = cbit->sub;
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("\trect (%d %d) (%d %d)\n",crect->x,crect->y,crect->w,crect->h);
        #endif
      }
      else if (cbit->type == REC_EXP_TXT)
      {
        ctxt = cbit->sub;
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("\ttxt (%d %d) (%s)\n",ctxt->x,ctxt->y,(char*)ctxt->str);
        #endif
      }
      cbit = cbit->next;
    }
    return 0;
  }
  
  int rec_ExposePlayXML( char *str )
  {
    return 0;
  }
  
  /**
   * takes the contents of an xml with a recorded exposure sequence
   * and reconstructs it (paints it)
   */
  int rec_ExposePlayXMLFile( char *filename )
  {
    static BOOL		binit = TRUE;
    static char		content[65536];
    static char		line[256], tmp[32];
    static xml_Inst	xml;
    static xml_Object	*o = NULL;
    static xml_Bit	*b = NULL;
    FILE		*fd = NULL;
    static int 		len, i, abs = 0, bits = 0;
    
    static struct gui_GC	*gc = NULL;
    static int			x, y, w, h;
    
    if (binit==TRUE)
    {
      binit = FALSE;
      gc = (struct gui_GC*)malloc(sizeof(struct gui_GC));
      gui_InitGCEx((struct gui_GC*)gc,(char*)"#ffffff");
      #ifdef _USE_GUI_XLIB_INTERNALS
        gc->color = NULL;
      #endif
      gc->colorl = (struct gui_ColorLow*)malloc(sizeof(struct gui_ColorLow));
      gc->colorl->value = 0;
    }
    else
    {
      xml_Free(&xml);
    }
    if ((fd = fopen(filename,"r")) != NULL)
    {
      xml.data = NULL;
      content[0] = 0x00;
      while(1)
      {
        if ( fgets((char*)line,255,fd) != NULL)
        {
          len = strlen((char*)line);
          line[len] = 0x00;
          if (content[0] == 0x00)
            strcpy((char*)content,(char*)line);
          else
            strcat((char*)content,(char*)line);
        }
        else
         break;
      }
      fclose(fd);
      
      xml.data = (void*)content;
      xml_Init(&xml);
      bits = 0;
      o = xml_GetObject(&xml,"rec");
      if (o != NULL)
      {
        b = xml_GetObjectBit(o,"bits");
        if (b!=NULL)
        {
          bits = atoi(b->value);
        }
      }
      for (i = 0; i < bits; i++)
      {
        sprintf((char*)tmp,"%d",i);
        o = xml_GetObject(&xml,(char*)tmp);
        if (o != NULL)
        {
          b = xml_GetObjectBit(o,"type");
          if (b != NULL)
          {
            x = y = w = h = -1;
            if (strcmp(b->value,"rect") == 0)
            {
              b = xml_GetObjectBit(o,"x");
              if (b != NULL) x = atoi(b->value);
              b = xml_GetObjectBit(o,"y");
              if (b != NULL) y = atoi(b->value);
              b = xml_GetObjectBit(o,"w");
              if (b != NULL) w = atoi(b->value);
              b = xml_GetObjectBit(o,"h");
              if (b != NULL) h = atoi(b->value);
              b = xml_GetObjectBit(o,"pixel");
              if (b != NULL) gc->colorl->value = atoi(b->value);
              #ifdef _DEBUG
                if (DEBUG==TRUE) printf("draw_rectangle (%d %d) (%d %d) (%d)\n",
                  x, y, w, h, gc->colorl->value);
              #endif
              gui_DrawRectangle((struct gui_GC*)gc,x,y,w,h);
            }
            else if (strcmp(b->value,"text") == 0)
            {
              b = xml_GetObjectBit(o,"x");
              if (b != NULL) x = atoi(b->value);
              b = xml_GetObjectBit(o,"y");
              if (b != NULL) y = atoi(b->value);
              b = xml_GetObjectBit(o,"w");
              if (b != NULL) w = atoi(b->value);
              b = xml_GetObjectBit(o,"h");
              if (b != NULL) h = atoi(b->value);
              b = xml_GetObjectBit(o,"text");
              if (b!=NULL)
              {
                gui_DrawTextEx(b->value,x,y,w,h,2);
              }
            }
          }
        }
      }
    }
    else
    {
      xml.obj = NULL;
      xml.data = NULL;
    }
    return 0;
  }
  
  int rec_ExposeToXML( struct rec_Expose *rec, char *str )
  {
    static struct _rec_expose_bit *cbit		= NULL;
    static struct _rec_expose_rect *crect	= NULL;
    static struct _rec_expose_txt *ctxt		= NULL;
    static char line[256];
    str[0] = 0x00;
    if (!rec) return -1;
    sprintf(str,"<rec bits=\"%d\">\n",_rec_expose_numbits(rec));
    cbit = rec->bits;
    int id = 0;
    while(cbit != NULL)
    {
      if (cbit->type == REC_EXP_RECT)
      {
        crect = cbit->sub;
        sprintf((char*)line,
          "<%d type=\"rect\" x=\"%d\" y=\"%d\" w=\"%d\" h=\"%d\" pixel=\"%d\">\n",
          id,crect->x,crect->y,crect->w,crect->h,crect->pixel);
        strcat(str,(char*)line);
      }
      else if (cbit->type == REC_EXP_TXT)
      {
        ctxt = cbit->sub;
        sprintf((char*)line,
          "<%d type=\"text\" x=\"%d\" y=\"%d\" w=\"%d\" h=\"%d\" text=\"%s\">\n",
          id,ctxt->x,ctxt->y,ctxt->w,ctxt->h,ctxt->str);
        strcat(str,(char*)line);
      }
      cbit = cbit->next;
      id++;
    }
    if (DEBUG==TRUE)
    {
      if (str[0] != 0x00)
      {
        printf("rec xml\n%s\n",str);
      }
    }
    return 0;
  }
  
  int rec_ExposeLogXML( char *str, char *filename )
  {
    if (!str) return -1;
    FILE *fd = NULL;
    if ((fd = fopen(filename,"w")) != NULL)
    {
      fwrite(str,strlen(str),1,fd);
      fclose(fd);
    }
    return 0;
  }
  
  int rec_ExposeAddTxt(
    struct rec_Expose *rec, 
    int x, int y, 
    int w, int h,
    char *str
  )
  {
    if (!rec || !str) return -1;
    if (strlen(str)==0) return -1;
    static struct _rec_expose_bit *cbit		= NULL;
    static struct _rec_expose_txt *ctxt		= NULL;
    if (!rec || w < 1 || h < 1) return -1;
    if (rec->bits==NULL)
    {
      rec->bits = (struct _rec_expose_bit*)malloc(sizeof(struct _rec_expose_bit));
      cbit = rec->bits;
    }
    else
    {
      cbit = rec->bits;
      while(cbit!=NULL)
      {
        if (cbit->next==NULL)
        {
          cbit->next = (struct _rec_expose_bit*)malloc(sizeof(struct _rec_expose_bit));
          cbit = cbit->next;
          break;
        }
        cbit = cbit->next;
      }
    }
    cbit->next = NULL;
    cbit->type = REC_EXP_TXT;
    cbit->sub = (struct _rec_expose_txt*)malloc(sizeof(struct _rec_expose_txt));
    ctxt = cbit->sub;
    ctxt->x = x;
    ctxt->y = y;
    ctxt->w = w;
    ctxt->h = h;
    if (strlen(str)>255)
    {
      strncpy((char*)ctxt->str,str,255);
      ctxt->str[255] = 0x00;
    }
    else
      strcpy((char*)ctxt->str,str);
    return 0;
  }
  
  int rec_ExposeAddRect( 
    struct rec_Expose *rec, 
    int x, int y, 
    int w, int h, 
    unsigned long pixel 
    )
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":rec_ExposeAddRect\n");
    #endif
    static struct _rec_expose_bit *cbit		= NULL;
    static struct _rec_expose_rect *crect	= NULL;
    if (!rec || w < 1 || h < 1) return -1;
    if (rec->bits==NULL)
    {
      rec->bits = (struct _rec_expose_bit*)malloc(sizeof(struct _rec_expose_bit));
      cbit = rec->bits;
    }
    else
    {
      cbit = rec->bits;
      while(cbit!=NULL)
      {
        if (cbit->next==NULL)
        {
          cbit->next = (struct _rec_expose_bit*)malloc(sizeof(struct _rec_expose_bit));
          cbit = cbit->next;
          break;
        }
        cbit = cbit->next;
      }
    }
    cbit->next = NULL;
    cbit->type = REC_EXP_RECT;
    cbit->sub = (struct _rec_expose_rect*)malloc(sizeof(struct _rec_expose_rect));
    crect = cbit->sub;
    crect->x = x;
    crect->y = y;
    crect->w = w;
    crect->h = h;
    crect->pixel = pixel;
    return 0;
  }

  /** adding misc objects associated with a Widget 
   */
  int gui_WidgetAddCustomValue( struct gui_Widget *w, int value )
  {
    static struct gui_object *current = NULL;
    
    if (!w) return -1;
    
    current = w->obj;
    if (current == NULL)
    {
      w->obj = (struct gui_object *)malloc(sizeof(struct gui_object));
      w->obj->id = GOID_CUSTOM;
      w->obj->value = value;
      w->obj->data = NULL;
      w->obj->next = NULL;
    }
    else
    {
      current = (struct gui_object *)malloc(sizeof(struct gui_object));
      current->next = w->obj;
      current->id = GOID_CUSTOM;
      current->value = value;
      current->data = NULL;
      w->obj = current;
    }
    return 0;
  }

int gui_Clear( )
{
  memset(
    (void*)bigInterface->surfaceData,
    0x00,
    bigInterface->width*bigInterface->height*4
    );
  return 0;
}

#endif
