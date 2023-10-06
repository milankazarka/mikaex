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
#include "ai.h"

  /**
   *  transform coordinates according to surface rotation 
   */
  void _transform_coord( int *x, int *y )
  {
    static int _y;
    
    if (rotation == ROT_DOWN)
    {
      *x = gui_GetMain()->attr.w-*x;
      *y = gui_GetMain()->attr.h-*y;
    }
    else if (rotation == ROT_RIGHT)
    {
      _y = *y;
      *y = *x;
      *x = gui_GetMain()->attr.w-_y;
    }
    else if (rotation == ROT_LEFT)
    {
      _y = *y;
      *y = gui_GetMain()->attr.h-*x;
      *x = _y;
    }
    #ifdef _DEBUG
      if (DEBUG==TRUE)
        printf("_transform_coord (%d %d)\n",*x,*y);
    #endif
  }

/**
 * initialize GUI elements 
 */
int gui_Init( )
{
  rotation 		= ROT_NONE;
  expose		= NULL;
  scene			= NULL;
  lst_images		= NULL;
  lst_txtcache		= NULL;
  lst_widgets		= NULL;
  bredrawbg		= TRUE; // not redrawing the background at every expose is effective
  
  rec_expose		= NULL;
  _lst_goid_custom	= NULL;
  #ifdef USECAIRO
    cairo_surface		= NULL;
    cairo			= NULL;
  #endif
}

  int gui_InitSkin( struct xml_Inst *xml )
  {
    struct xml_Object *o = NULL;
    struct xml_Bit *b = NULL;
    strcpy((char*)xml->filename,"skin.xml");
    xml_Init(xml);
    o = xml_GetObject(xml,"img_scroll_up");
    if (o!=NULL)
    {
      b = xml_GetObjectBit(o,"file");
      gui_ImageRegisterSimple(b->value,"img_scroll_up");
    }
    return 0;
  }
  
  int gui_WidgetInitAttr( struct gui_Widget *w )
  {
    w->otype	= GO_STATIC;
    
    w->queue 	= NULL;
    w->obj 	= NULL;
    
    w->dataModel		= NULL;
    
    w->attr.state		= 0;
    w->attr.bvisible 		= FALSE;
    w->attr.onclick		= NULL;
    w->attr.onbuttondown	= NULL;
    w->attr.onbuttonup		= NULL;
    w->attr.onclickEx           = NULL;
    w->attr.onover		= NULL;
    w->attr.onexpose		= NULL;
    w->attr.onexposeEx          = NULL;
    w->attr.onmotion		= NULL;

    w->attr.aonclick		= NULL;

    w->attr.x			= 0;
    w->attr.y			= 0;
    w->attr.w			= 0;
    w->attr.h			= 0;

    w->attr.bvirtual		= FALSE;
    w->attr.vx			= 0;
    w->attr.vy			= 0;

    w->attr.level		= GUI_LEVEL_MAX;

    w->calls.my			= NULL;

    w->attr.motion.t		= -1;
    w->attr.motion.x		= -1;
    w->attr.motion.y		= -1;

    w->relay			= NULL;
  
    w->attr.form		= FORM_NONE;
    
    w->attr.hierarchy		= -1;
    
    w->attr.ftsize		= DEFA_FTSIZE;
    
    w->_surface			= NULL; // object surface buffer
    
    w->attr.name[0]		= 0x00;
    
    return 0;
  }
  
  #define _OVERRIDE // override_redirect
  int gui_WidgetInit( struct gui_Widget *w, struct gui_Widget *parent )
  {
    if (parent == NULL)
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("gui_WidgetInit parent\n");
      #endif
      
      #ifdef USETCL
        _tcl_init();
        Tk_CreateEventHandler(tkwin,
          ExposureMask|ButtonPressMask|ButtonReleaseMask|
          PropertyChangeMask|SubstructureNotifyMask|
          ResizeRedirectMask|
          GCFunction|StructureNotifyMask|
          FocusChangeMask|VisibilityChangeMask|NoEventMask|
          SubstructureRedirectMask,
          _tkEventProc,0);
        w->id = Tk_WindowId(tkwin);
        XResizeWindow(w->dpy,w->id,w->attr.w,w->attr.h);
      #else
          #ifdef _DEBUG
            if (DEBUG==TRUE) printf("__interface before Init\n");
          #endif
          interfaceInit(NULL);
          #ifdef _DEBUG
            if (DEBUG==TRUE) printf(":interfaceInit end\n");
          #endif
          if (!bigInterface)
          {
            printf("multiplatform interface could not be created\n");
            exit(2);
          }
          bigInterface->x 	= w->attr.x;
          bigInterface->y 	= w->attr.y;
          bigInterface->width 	= w->attr.w;
          bigInterface->height 	= w->attr.h;
          
          #ifdef _DEBUG
            if (DEBUG==TRUE) printf("__interface w(%d) h(%d) in guiMain.c\n",
                      bigInterface->width,bigInterface->height);
          #endif
          bigInterface->bpp = 32;
          
          gui_BoundsSetEx(&gui_bounds,&w->attr); // drawing bounds
          
          interfaceStart(bigInterface);
          /**
           *  compatibility with the old interface:
          */
          w->id = 0;
          bigInterface->getHWWindowId(bigInterface,&w->id);
          if (w->id==0)
          {
            #ifdef _DEBUG
                if (DEBUG==TRUE) printf("could not retrieve HW Window ID\n");
            #endif
          }
          else
          {
            #ifdef _DEBUG
                if (DEBUG==TRUE) printf("retrieved HW Window ID (%d)\n",w->id);
            #endif
          }
        
        glpoly = gui_GlPolyInitFromFile(gl,NULL,"arrowUp");
        glpoly->trans.width = 200;
        glpoly->trans.height = 160;
        gui_GlPolyTransformProtoEdges(glpoly);
        gui_GlColorSet(gl,&glpoly->color,0xcc,0xcc,0xfa,0xda);
        
        #ifdef USECAIRO
          #ifdef _DEBUG
            if (DEBUG==TRUE) 
              printf("gui_WidgetInit init cairo\n");
          #endif
          cairo_gui_surface = gui_SurfaceInitEx(w,NULL,w->attr.w,w->attr.h);
          cairo_surface = cairo_image_surface_create_for_data(
            //cairo_gui_surface->data,
            bigInterface->surfaceData,
            CAIRO_FORMAT_ARGB32,
            w->attr.w,w->attr.h,
            4*w->attr.w
          );
          cairo = cairo_create(cairo_surface);
          if (!cairo)
          {
            #ifdef _DEBUG
                if (DEBUG==TRUE) printf(":Could not initialize Cairo, exiting\n");
            #else
                printf(":Could not initialize GL, exiting\n");
            #endif
            exit(2);
          }
          cairo_select_font_face(cairo, "Arial",
                CAIRO_FONT_SLANT_NORMAL,
                CAIRO_FONT_WEIGHT_NORMAL);
        #endif
      
      #endif
     
      /**  default font, if no font is set for widget
       *   todo - Cleanup
       */
      w->glfont = gui_GlFontInit( gl, NULL, DEFA_FT );
      w->glfont->color.rgba[0] = 0xa0;
      w->glfont->color.rgba[1] = 0xa0;
      w->glfont->color.rgba[2] = 0xa0;
      w->glfont->color.rgba[3] = 220;
      w->glfont->height = DEFA_FTSIZE;
      w->glfont->width = DEFA_FTSIZE;
      
      if (REC_ENABLE==1)
      {
        rec_expose	= rec_ExposeInit(NULL);
      }
      
      lst_can_content = NULL; // canvas content list empty
    }
    else // has parent
    {
      if (parent->attr.form == FORM_NONE && parent != gui_GetMain()) // if parent has no FORM defined, only allow it to be toplevel
      {
        w->attr.state = MSG_WBADLEVEL;
        return MSG_WBADLEVEL;
      }
      w->glfont = gui_GlFontInit(gl,NULL,DEFA_FT);
      w->glfont->color.rgba[0] = 0xff;
      w->glfont->color.rgba[1] = 0xff;
      w->glfont->color.rgba[2] = 0xff;
      w->glfont->color.rgba[3] = 220;
      w->glfont->height = DEFA_FTSIZE;
      w->glfont->width = DEFA_FTSIZE;
    }
    
    w->parent  = parent;

    if (w->attr.form == FORM_MENU)
    {
      struct form_Menu *menu = NULL;
      w->form = (struct form_Menu*)malloc(sizeof(struct form_Menu));
      menu = (struct form_Menu*)w->form;
      menu->xfill = 0;
      menu->yfill = 0;
      menu->ymove = 0; // enables scrolling menu elements
      menu->xmove = 0;
    }
    else if (w->attr.form == FORM_SLIDEBAR)
    {
      w->form = (struct form_Slidebar*)malloc(sizeof(struct form_Slidebar));
      struct form_Slidebar *sbar = (struct form_Slidebar*)w->form;
      sbar->relay = NULL;
    }
    
    w->next = NULL;
    
    if (lst_widgets == NULL)
    {
      lst_widgets = w;
    }
    else
    {
      gui_Widget *current = lst_widgets;
      while(current != NULL)
      {
        if (current->next == NULL)
        {
          current->next = w;
          break;
        }
        current = current->next;
      }
    }
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":init window end\n");
    #endif
    
    return 0;
  }
  
  /** data models are deprecated
   */
  
  /**
   *  Data Model
   *  generate data model data holder through a csv file
   */
  int gui_WidgetInitDataModelCSV(
    struct gui_Widget *widget,
    unsigned char *filename
  )
  {
    if (!widget||!filename) return -1;
    //widget->dataModel = trop_DataModelInit(NULL);
    //widget->dataModel->csv = trop_CsvInit(NULL,filename);
    
    widget->dataModel = trop_DataModelGetFromCsv(NULL,filename);
    
    return 0;
  }
  
  int gui_WidgetSetAction( struct gui_Widget *w, struct trop_Action *action )
  {
    w->attr.aonclick = action;
    return 0;
  }
  
  /** map a widget, set it to visible
   */
  int gui_WidgetMap( struct gui_Widget *w )
  {
    w->attr.bvisible = TRUE;
    w->attr.hierarchy = 1;
    if (w->parent == NULL)
    {
      bigInterface->mapMainWindow(bigInterface);
    }
    
    #ifdef _DEBUG
      printf("gui_WidgetMap\n");
    #endif
    
    return 0;
  }
  
  /** unmap a widget, set it to non-visible
   */
  int gui_WidgetUnMap( struct gui_Widget *w )
  {
    #ifdef _DEBUG
      printf("gui_WidgetUnMap\n");
    #endif
    w->attr.bvisible = FALSE;
    // todo expose
    return 0;
  }
  
  void _gui_WidgetResetMotion( struct gui_Widget *w )
  {
    w->attr.motion.t	= -1;
    w->attr.motion.x	= -1;
    w->attr.motion.y	= -1;
  }

  /**
   * manual triggering of gui events 
   */
  int gui_TriggerExpose( struct gui_Widget *main )
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) 
        printf(":gui_TriggerExpose\n");
    #endif
    bigInterface->triggerExpose(bigInterface);
    return 0;
  }
  
  /** redraw only a certain widget
   */
  int gui_TriggerExposeWidget( struct gui_Widget *main, struct gui_Widget *w )
  {
    gui_TriggerExpose(main);
    return 0;
  }

  /** todo - rewrite the low level gui_Widget manipulation
   */
  
  /** get the real position of a widget in the tree
   */
  int gui_WidgetGetRealPosition( 
    struct gui_Widget *wi,
    int *x, int *y 
    )
  {
    static struct gui_Widget *parent = NULL;
    if (wi==NULL) return -1;
    
    *x = wi->attr.x;
    *y = wi->attr.y;
    
    parent = wi->parent;
    while(parent!=gui_GetMain())
    {
      *x += parent->attr.x;
      *y += parent->attr.y;
      parent = parent->parent;
    }
    
    return 0;
  }

  /** expose object which make up the widget 
   */
  int __gui_widget_expose_objects(
    struct gui_Widget *wi,
    struct gui_Surface *surface,
    int x, int y,
    int w, int h,
    int _px, int _py
  )
  {
    #ifdef _GUI_SUBWIDGET
      struct gui_object 	*gobj 		= NULL;
      struct gui_Image 		*image		= NULL;
      struct gui_Widget		*wsub		= NULL;
      struct gui_GC 		*gc		= NULL;
      int 	        	gcfg_round	= 0;
      int			gcfg_gradient	= 0;
      struct 			_gui_bound   	bound;
    
      gobj 		= NULL;
      image		= NULL;
      wsub		= NULL;
      gc		= NULL;
      gcfg_round	= 0;
      gcfg_gradient	= 0;
    
      int			vx;
      int			vy;
      int			vw;
      int			vh;
      int			px; // backwards compatibility
      int			py;
      int	                subx;
      int 	                suby;
      int			hideh;
    #else
      static struct gui_object 	*gobj 		= NULL;
      static struct gui_Image 	*image		= NULL;
      static struct gui_Widget	*wsub		= NULL;
      static struct gui_GC 	*gc		= NULL;
      static int 	        	gcfg_round	= 0;
      static int			gcfg_gradient	= 0;
      static struct 		_gui_bound   	bound;
    
      gobj 		= NULL;
      image		= NULL;
      wsub		= NULL;
      gc			= NULL;
      gcfg_round		= 0;
      gcfg_gradient	= 0;
    
      static int			vx;
      static int			vy;
      static int			vw;
      static int			vh;
      static int			px; // backwards compatibility
      static int			py;
      static int                  subx;
      static int                  suby;
      static int			hideh;
    #endif
    
    if (wi==NULL) return -1;
    
    wCurrent = wi;
    if (wCurrent->attr.bvisible==FALSE)
    {
      #ifdef _DEBUG
        printf("__gui_widget_expose_objects skip widget\n");
      #endif
      return 0;
    }
    
    px = _px;
    py = _py;
    
    gcfg_round 		= 0;
    gcfg_gradient 	= 0;
    
    gobj = wi->obj;
    while(gobj != NULL)
    {
      switch(gobj->id)
      {
        case GCFG_ROUND:
          gcfg_round = gobj->value;
        break;
        case GCFG_GRADIENT:
          gcfg_gradient = gobj->value;
        break;
      }
      gobj = gobj->next;
    }
    
    gobj = wi->obj;
    while(gobj != NULL)
    {
      if (gobj->id == GOID_COLOR)
      {
        vw = w;
        vh = h;
        vx = x;
        vy = y;
        
        if (vh<=1)
        {
          gobj = gobj->next;
          continue;
        }
        if (vx+vw>=wi->parent->attr.w) vw = wi->parent->attr.w - vx; // overpainting
        if (vy+vh>=wi->parent->attr.h) vh = wi->parent->attr.h - vy;
        
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("\twidget color expose\n");
        #endif
        gc = (struct gui_GC*)gobj->data;
        
        if (vx<0)
        {
          vw += vx;
          vx = 0;
        }
            
        if (vy<0)
        {
          vh += vy;
          vy = 0;
        }
        
        if (gcfg_round!=0) // todo - refactoring, one entry point
        {
          if (gcfg_gradient==0)
          { 
            gui_DrawRoundRectangle(
              gc,
              vx+px,
              vy+py,
              vw,vh,
              gcfg_round
              );
          }
          else
          {
            gui_DrawRoundRectangleGradient(
              gc,
              vx+px,
              vy+py,
              vw,vh,
              gcfg_round,
              gcfg_gradient
              );
          }
        }
        else
        {
          gui_DrawRectangle(
            gc,
            vx+px,
            vy+py,
            vw,vh
            );
        }
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("\twidget color expose.end\n");
        #endif
      }
      gobj = gobj->next;
    }
    
    gobj = wi->obj; // color to
    while(gobj != NULL)
    {
      if (gobj->id == GOID_COLOR2)
      {
        vw = w;
        vh = h/2;
        vx = x;
        vy = y+(h/2);
        
        if (vh<=1)
        {
          gobj = gobj->next;
          continue;
        }
        if (vx+vw>=wi->parent->attr.w) vw = wi->parent->attr.w - vx; // overpainting
        if (vy+vh>=wi->parent->attr.h) vh = wi->parent->attr.h - vy;
        
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("\twidget color expose\n");
        #endif
        gc = (struct gui_GC*)gobj->data;
        
        if (vx<0)
        {
          vw += vx;
          vx = 0;
        }
            
        if (vy<0)
        {
          vh += vy;
          vy = 0;
        }
        
        if (gcfg_round!=0) // todo - refactoring, one entry point
        {
          if (gcfg_gradient==0)
          { 
            gui_DrawRoundRectangle(
              gc,
              vx+px,
              vy+py,
              vw,vh,
              gcfg_round
              );
            if (vh-10>0) // todo
            {
              hideh=vh-10;
            
              gui_DrawRoundRectangle(
                gc,
                vx+px,
                vy+py,
                vw,hideh,
                0
                );
            }
          }
          else
          {
            gui_DrawRoundRectangleGradient(
              gc,
              vx+px,
              vy+py,
              vw,vh,
              gcfg_round,
              gcfg_gradient
              );
          }
        }
        else
        {
          gui_DrawRectangle(
            gc,
            vx+px,
            vy+py,
            vw,vh
            );
        }
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("\twidget color expose.end\n");
        #endif
      }
      gobj = gobj->next;
    }
    
    gobj = wi->obj;
    while(gobj != NULL)
    {
      if (gobj->id == GOID_BGALPHA)
      {
        vw = w;
        vh = h;
        vx = x;
        vy = y;
        
        if (vh<=0)
        {
          gobj = gobj->next;
          continue;
        }
        if (vx+vw>=wi->parent->attr.w) vw = wi->parent->attr.w - vx; // overpainting
        if (vy+vh>=wi->parent->attr.h) vh = wi->parent->attr.h - vy;
        
        gui_SurfaceMoveAlpha(
          px+vx,
          py+vy,
          vw,
          vh,
          gobj->value);
      }
      gobj = gobj->next;
    }
    
    gobj = wi->obj;
    while(gobj != NULL)
    {
      if (gobj->id == GOID_IMAGE)
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("\t:button image expose\n");
        #endif
        image = (struct gui_Image*)gobj->data;
                
        vw = image->w;
        vh = image->h;
        vx = image->x +x;
        vy = image->y +y;
        
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("\t\t:button image %dx%d+%d+%d\n",
            vw,vh,
            vx+px,vy+py
            );
        #endif
            
        // todo: image size correction
        
        if (surface==NULL)
        {    
          gui_DrawImage(
            image,
            vx+px,
            vy+py,
            vw,vh
            );
        }
        else
        {
          gui_DrawImageSurface(
            image,
            surface,
            vx-x,vy-y, // todo
            vw,vh
            );
        }
    
      }
      gobj = gobj->next;
    }
    
    gobj = wi->obj;
    while(gobj != NULL)
    {
      if (gobj->id == GOID_POLY)
      {
        struct gui_GlPoly *glpoly = (struct gui_GlPoly*)gobj->data;
        glpoly->trans.width = w;
        glpoly->trans.height = h;
        glpoly->trans.x = x + px;
        glpoly->trans.y = y + py;
        gui_GlPolyTransformProtoEdges(glpoly);
        gui_GlPolyDraw(gl,glpoly);
      }
      gobj = gobj->next;
    }
    
    gobj = wi->obj;
    while(gobj != NULL)
    {
      if (gobj->id == GOID_TEXTSIM)
      {
        vw = w;
        vh = gui_GetMain()->attr.ftsize+2;
        vx = x;
        vy = y+h-vh;
        
        if (
          px+vx+vw>=wi->parent->attr.w ||
          py+vy+vh>=wi->parent->attr.h ||
          px+vx < wi->parent->attr.x ||
          py+vy < wi->parent->attr.y
          )
        {
          gobj = gobj->next;
          continue;
        }
        
        #ifdef _DEBUG
          if (DEBUG==TRUE) 
            printf(":draw_text (%d %d) (%s)\n",vw,vh,(char*)gobj->data);
        #endif
                
        /*gui_DrawTextEx(
          (char*)gobj->data,
          px+vx,
          py+vy,
          vw,vh,DEFA_TXTR);*/
      }
      gobj = gobj->next;
    }
    gobj = wi->obj;
    while(gobj != NULL)
    {
      if (gobj->id == GOID_TEXTSIMB)
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("__text GOID_TEXTSIMB\n");
        #endif
        struct gui_TextSim *ts = gobj->data;
        vw = w;
        vh = gui_GetMain()->attr.ftsize+2;
        vx = x;
        
        if (ts->placement == PLACE_UP)
        {
          vy = 0;
        }
        else if (ts->placement == PLACE_DOWN)
        {
          vy = y+h-vh;
        }
        else if ( // todo, better text handling
          ts->placement == PLACE_CENTER ||
          ts->placement == PLACE_LEFT ||
          ts->placement == PLACE_RIGHT
          )
        {
          vy = ((y+h)/2)-(vh/2);
        }
        
        if (
          px+vx+vw>=wi->parent->attr.w ||
          py+vy+vh>=wi->parent->attr.h ||
          px+vx < wi->parent->attr.x ||
          py+vy < wi->parent->attr.y
          )
        {
          gobj = gobj->next;
          continue;
        }
        
        /*gui_DrawTextEx(
          (char*)ts->str,
          px+vx,
          py+vy,
          vw,vh,DEFA_TXTR);*/
      }
      gobj = gobj->next;
    }
    
    gobj = wi->obj;
    while(gobj != NULL)
    {
      if (gobj->id == GOID_TEXTSIMC)
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("__text GOID_TEXTSIMC\n");
        #endif
        struct gui_TextSim *ts = gobj->data;
        vw = w;
        vh = gui_GetMain()->attr.ftsize+2;
        vx = x;
        
        if (ts->placement==-1)
        {
          #ifdef _DEBUG
            printf("_draw_text\n");
          #endif
          /** absolute placement
           */
           vx = ts->pos.x;
           vy = ts->pos.y;
        }
        else
        {
          if (ts->placement == PLACE_UP)
          {
            vy = 0;
          }
          else if (ts->placement == PLACE_DOWN)
          {
            vy = y+h-vh;
          }
          else if ( // todo, better text handling
            ts->placement == PLACE_CENTER ||
            ts->placement == PLACE_LEFT ||
            ts->placement == PLACE_RIGHT
            )
          {
          //  vy = ((y+h)/2)-(vh/2);
            vy = y+h-vh;
          }
        }
        
        if (
          px+vx+vw>=wi->parent->attr.w ||
          py+vy+vh>=wi->parent->attr.h ||
          px+vx < wi->parent->attr.x ||
          py+vy < wi->parent->attr.y
          )
        {
        //  gobj = gobj->next;
        //  continue;
        }
        
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("__text before draw\n");
        #endif
        
        gui_DrawTextExA(
          ts,
          px+vx,
          py+vy,
          vw,vh,DEFA_TXTR);
      }
      gobj = gobj->next;
    }
    
    gobj = wi->obj;
    while(gobj != NULL)
    {
      if (gobj->id == GOID_TEXTSIMPOS)
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("__text GOID_TEXTSIMPOS\n");
        #endif
        struct gui_TextSim *ts = gobj->data;
        vw = w;
        vh = gui_GetMain()->attr.ftsize+2;
        vx = x;
        
        if (ts->placement==-1)
        {
          #ifdef _DEBUG
            printf("_draw_text\n");
          #endif
          /** absolute placement
           */
           vx = ts->pos.x+x;
           vy = ts->pos.y+y+vh;
        }
        else
        {
        }
        
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("__text before draw\n");
        #endif
        
        gui_DrawTextExA(
          ts,
          px+vx,
          py+vy,
          vw,vh,DEFA_TXTR);
      }
      gobj = gobj->next;
    }
    
    gobj = wi->obj;
    while(gobj != NULL)
    {
      if (gobj->id == GOID_TEXTSIMD)
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("__text GOID_TEXTSIMD\n");
        #endif
        struct gui_TextSim *ts = gobj->data;
        vw = w+ts->pos.w;
        vh = gui_GetMain()->attr.ftsize; //ts->pos.h;
        vx = x+ts->pos.x;
        vy = y+ts->pos.y;
        
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("__text before draw\n");
        #endif
        
        if (
          px+vx+vw>=wi->parent->attr.w ||
          py+vy+vh>=wi->parent->attr.h ||
          px+vx < wi->parent->attr.x ||
          py+vy < wi->parent->attr.y
          )
        {
        //  gobj = gobj->next;
        //  continue;
        }
        
        gui_DrawTextExA(
          ts,
          px+vx,
          py+vy,
          vw,vh,DEFA_TXTR);
      }
      gobj = gobj->next;
    }
    
    #ifdef _GUI_SUBWIDGET
      gobj = wi->obj; // subwidget functionality
      while(gobj != NULL)
      {
        if (gobj->id == GOID_WIDGET)
        {
          wsub = (struct gui_Widget*)gobj->data;
          int subx = wsub->attr.x;
          int suby = wsub->attr.y;
          if (!wsub)
          {
            gobj = gobj->next;
            continue;
          }
          #ifdef _DEBUG 
            printf("GOID_WIDGET found x(%d) y(%d) vx(%d) vy(%d)\n",
              px+x,py+y,vx,vy);
          #endif
          if (wsub->attr.onexposeEx)
          {
            wsub->attr.onexposeEx(
              wsub,
              vx+px+wsub->attr.x,
              vy+py+wsub->attr.y,
              wsub->attr.w,
              wsub->attr.h
              );
          }
          else if (wsub->attr.form == FORM_BUTTON)
          {
            gui_WidgetExposeEx(
              wsub,
              vx+px+wsub->attr.x,
              vy+py+wsub->attr.y,
              wsub->attr.w,
              wsub->attr.h);
          }
        }
        gobj = gobj->next;
      }
    #endif
    
    return 0;
  }
  
  int _gui_widget_expose_objects(
    struct gui_Widget *wi,
    int x, int y,
    int w, int h,
    int _px, int _py
  )
  {
    return __gui_widget_expose_objects(wi,NULL,x,y,w,h,_px,_py);
  }
  
  /** expose widget objects to a speciffic surface 
   */
  int _gui_widget_expose_objects_surface(
    struct gui_Widget *wi,
    struct gui_Surface *surface,	// surface
    int x, int y,
    int w, int h,
    int _px, int _py
  )
  {
    return __gui_widget_expose_objects(wi,surface,x,y,w,h,_px,_py);
  }
  
  /**
   *  widget expose with smart resolution of positioning 
   *  - if you wish the position to be calculated, use '-1' as position values
   */
  #define _USE_BOUNDS_NO
  int gui_WidgetExposeEx( 
    struct gui_Widget *wi,
    int x, int y,
    int w, int h
    )
  {
    static int		rx;
    static int		ry;
    static int		rw;
    static int		rh;
    static int		tmpx;
    static int		tmpy;
    static int		tmpw;
    static int		tmph;
    
    if (wi==NULL||w<1||h<1) return -1;
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":gui_WidgetExposeEx\n");
    #endif
    
    rx = x;
    ry = y;
    rw = wi->attr.w; // todo
    rh = wi->attr.h;
    if (x==-1&&y==-1&&w==-1&&h==-1)
    {
      if ( gui_WidgetGetRealPosition(wi,&rx,&ry) == -1 )
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("__widget position could not be calculated\n");
        #endif
        return -1;
      }
    }

    #ifdef _USE_BOUNDS
      tmpx = gui_bounds.x;
      tmpy = gui_bounds.y;
      tmpw = gui_bounds.width;
      tmph = gui_bounds.height;
      gui_BoundsSet(&gui_bounds,rx,ry,rw,rh);
    #endif
    _gui_widget_expose_objects(wi,rx,ry,rw,rh,0,0);
    #ifdef _USE_BOUNDS
      gui_BoundsSet(&gui_bounds,tmpx,tmpy,tmpw,tmph);
    #endif

    return 0;
  }
  
  struct gui_Widget *gui_NewWidget( ) // create new widgets using this function
  {
    struct gui_Widget *nw = (struct gui_Widget*)malloc(sizeof(struct gui_Widget));
    gui_WidgetInitAttr(nw);
    gui_SceneAddMember(scene,nw); // override if needed
    return nw;
  }

  int gui_WidgetSetDynamic( struct gui_Widget *w )
  {
    if (!w) return -1;
    w->otype = GO_DYNAMIC;
    return 0;
  }

  int gui_WidgetSetStatic( struct gui_Widget *w )
  {
    if (!w) return -1;
    w->otype = GO_STATIC;
    return 0;
  }

  int gui_WidgetSetName( struct gui_Widget *w, char *name )
  {
    if (!w||!name) return -1;
    if (strlen(name)<DEFA_TXTCACHE) strcpy((char*)w->attr.name,name);
    else 
    {
      strncpy((char*)w->attr.name,name,DEFA_TXTCACHE-1);
      w->attr.name[DEFA_TXTCACHE-1] = 0x00;
    }
    return 0;
  }

  char *gui_WidgetGetName( struct gui_Widget *w )
  {
    if (!w) return NULL;
    if (w->attr.name[0]==0x00) return NULL;
    else return (char*)w->attr.name;
  }
  
  /**  get the real position of a widget in the widget hierarchy
   *   start x=0, y=0
   */
  int _widget_get_real_position( struct gui_Widget *w, int *x, int *y )
  {
    #ifdef _DEBUG
      printf("_widget_get_real_position real_pre(%d/%d)\n",
        *x,*y);
    #endif
    *x += w->attr.x;
    *y += w->attr.y;
    #ifdef _DEBUG
      printf("\t_widget_get_real_position real(%d/%d)\n",
        *x,*y);
    #endif
    if (w->parent)
    {
      #ifdef _DEBUG
        printf("\t\t_widget_get_real_position ->parent(%d/%d)\n",
          w->parent->attr.x,w->parent->attr.y);
      #endif
      _widget_get_real_position(w->parent,x,y);
    }
    return 0;
  }
  
  /** user triggered ButtonPress
   */
  int gui_WidgetButtonPress( struct gui_Event *event )
  {
    if (!event) return -1;
    #ifdef _DEBUG
      printf("gui_WidgetButtonPress %d\n",event->type);
    #endif
    
    #ifdef _AI
      printf("_ai gui_WidgetButtonPress\n");
    #endif
    
    gui_WidgetDoRelaysStatic( event->widget, event );
    
    // move through the list of sub widgets and forward them
    // this touch event
    //
    #ifdef _GUI_SUBWIDGET
      struct gui_object *gobj = NULL;
      struct gui_Widget *wsub = NULL;
      gobj = event->widget->obj;
      struct gui_Event subEvent;
      int realx;
      int realy;
      while(gobj != NULL)
      {
        if (gobj->id == GOID_WIDGET)
        {
          wsub = (struct gui_Widget*)gobj->data;
          wsub->parent = event->widget;
          realx = realy = 0;
          _widget_get_real_position(wsub,&realx,&realy);
          if (
            event->x > realx &&
            event->y > realy &&
            realx+wsub->attr.w > event->x &&
            realy+wsub->attr.h > event->y
          )
          {
            #ifdef _DEBUG
              printf("gui_WidgetButtonPress check sub widget \
x/y(%d/%d) w/h(%d/%d) event(%d/%d) sub(%d/%d) real(%d,%d)\n",
                event->widget->attr.x,
                event->widget->attr.y,
                event->widget->attr.w,
                event->widget->attr.h,
                event->x,event->y,
                wsub->attr.x,wsub->attr.y,
                realx,realy);
            #endif
            subEvent.widget = wsub;
            subEvent.type = event->type;
            gui_WidgetButtonPress(&subEvent);
          }
        }
        gobj = gobj->next;
      }
    #endif
    
    switch(event->type)
    {
      case INTERFACE_EVENT_BDOWN:
        if (event->widget->attr.onbuttondown != NULL)
        {
          #ifdef _DEBUG
            if (DEBUG==TRUE)
              printf(":gui_WidgetButtonPress attr.onbuttondown ptr(%p)\n",
                event->widget->attr.onbuttondown);
          #endif
          event->widget->attr.onbuttondown(event);
        }
        break;
      case INTERFACE_EVENT_BUP:
        if (event->widget->attr.onbuttonup != NULL)
        {
          #ifdef _DEBUG
            if (DEBUG==TRUE)
              printf(":gui_WidgetButtonPress attr.onbuttonup ptr(%p)\n",
                event->widget->attr.onbuttonup);
          #endif
          event->widget->attr.onbuttonup(event);
        }
        else if (event->widget->attr.onclick != NULL)
        {
          #ifdef _DEBUG
            if (DEBUG==TRUE)
              printf(":gui_WidgetButtonPress attr.onclick ptr(%p)\n",
                event->widget->attr.onclick);
          #endif
          event->widget->attr.onclick(event); 
        }
        break;
    }
    return 0;
  }
  
  #define _WQ_ASYNC	0
  #define _WQ_SYNC	1
  #define _WQ_TYPE	_WQ_ASYNC
  int gui_WidgetQueue( struct gui_Widget *main )
  {
    if (_WQ_TYPE==_WQ_ASYNC)
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf(":gui_WidgetQueue ptr(%p)\n",main);
      #endif
      main->queue = (struct trop_Atom*)malloc(sizeof(struct trop_Atom));
      main->queue->call = _gui_WidgetQueue;
      main->queue->data = main;
      trop_AtomInit(main->queue);
    }
    else if (_WQ_TYPE==_WQ_SYNC)
    {
      _gui_WidgetQueue(main);
    }
    return 0;
  }

  int gui_WidgetAddRelay( struct gui_Widget *w, struct trop_Relay *r )
  {
    trop_ListAdd( w->relay, r );
    return 0;
  }
  
  int gui_WidgetRemoveAllRelayAtID( struct gui_Widget *w, int id )
  {
    struct trop_List *current = w->relay;
    struct trop_Relay *rel = NULL;
    while(current != NULL)
    {
      if (current->ptr != NULL)
      {
        rel = (struct trop_Relay*)current->ptr;
        if (rel->id == id)
        {
          rel->call = NULL;
          free(rel);
        }
      }
      current = current->next;
    }
    return 0;
  }
  
  /**
   *  mika: define that a GUI should be propagated to
   *  all members in a relay list in a widget which match the relay ID
   */
  int gui_WidgetDoRelay( struct gui_Widget *w, int id, void *data )
  {
    struct trop_List *current = w->relay;
    struct trop_Relay *rel = NULL;
    while(current != NULL)
    {
      if (current->ptr != NULL)
      {
        rel = (struct trop_Relay*)current->ptr;
        if (rel->id == id)
        {
          rel->call(data);	// more relays of same id possible
        }
      }
      current = current->next;
    }
    return 0;
  }
  
  /** get the main widget
   */
  struct gui_Widget *gui_GetMain( )
  {
    static gui_Widget *_main = NULL;
    if (_main) return _main;
    
    struct gui_Widget *main = lst_widgets;
    while(main != NULL)
    {
      if (main->parent == NULL)
      {
        _main = main;
        return main;
      }
      main = main->next;
    }
    return NULL;
  }
  
  /**
   *  Add a custom GOID object to the Widget - this can be a strong
   *  tool to simplify creating new widget types through a higher
   *  level interface
   */
  int gui_WidgetAddCustomPtr( struct gui_Widget *w, void *ptr )
  {
    static struct gui_object *current = NULL;
    
    if (!w||!ptr) return -1;
    
    current = w->obj;
    if (current == NULL)
    {
      w->obj = (struct gui_object *)malloc(sizeof(struct gui_object));
      w->obj->id = GOID_CUSTOM;
      w->obj->value = -1;
      w->obj->data = ptr;
      w->obj->next = NULL;
    }
    else
    {
      current = (struct gui_object *)malloc(sizeof(struct gui_object));
      current->next = w->obj;
      current->id = GOID_CUSTOM;
      current->value = -1;
      current->data = ptr;
      w->obj = current;
    }
    return 0;
  }
  
  /**
   * add text passing the gui_Text structure 
   */
  int gui_WidgetAddWidget( struct gui_Widget *p, struct gui_Widget *w )
  { return _gui_widget_add_gobj(p,w,0,GOID_WIDGET); }
  
  int gui_WidgetAddImage( struct gui_Widget *w, struct gui_Image *i )
  { return _gui_widget_add_gobj(w,i,0,GOID_IMAGE); }
  
  int gui_WidgetAddGlPoly( struct gui_Widget *w, struct gui_GlPoly *poly )
  { return _gui_widget_add_gobj(w,poly,0,GOID_POLY); }
  
  /**
   * add text passing the gui_Text structure 
   */
  int gui_WidgetAddText( struct gui_Widget *w, struct gui_Text *text )
  { return _gui_widget_add_gobj(w,text,0,GOID_TEXT); }

  /**
   * add text, simply passing the string data 
   */
  int gui_WidgetAddTextSimple( struct gui_Widget *w, char *str )
  { return _gui_widget_add_gobj(w,str,0,GOID_TEXTSIM); }

  /** change text at a position
   */
  int gui_WidgetChangeText( struct gui_Widget *w, char *str, int placement )
  {
    if (!w || !str)
      return 1;
    
    struct gui_object *current = w->obj;
    struct gui_object *otext = NULL;
    struct gui_TextSim *ts = NULL;
    while(current)
    {
      if (current->id==GOID_TEXTSIMC)
      {
        ts = (struct gui_TextSim*)current->data;
        if (ts->placement==placement)
        {
          otext = current;
          if (ts->str)
            free(ts->str);
          ts->str = (char*)malloc(strlen(str)+1);
          strcpy(ts->str,str);
        }
      }
      current = current->next;
    }
    
    if (!otext)
    {
      _gui_widget_add_text_simple(w,NULL,GOID_TEXTSIMC,str,placement);
    }
    
    return 0;
  }

  int _gui_widget_add_text_simple_pos(
                                  struct gui_Widget *w,
                                  struct gui_TextSim *_ts,
                                  int type, 
                                  char *str, int x, int y
                                  )
  {	// todo - refactoring
    static struct gui_TextSim *ts = NULL;
    struct gui_object *current = w->obj;
    
    #ifdef _DEBUG
      printf("_gui_widget_add_text_simple_pos %d,%d\n",x,y);
    #endif
    
    if (current == NULL)
    {
      w->obj = (struct gui_object *)malloc(sizeof(struct gui_object));
      w->obj->next = NULL;
    }
    else
    {
      current = (struct gui_object *)malloc(sizeof(struct gui_object));
      current->next = w->obj;
      w->obj = current;
    }
    if (_ts==NULL)
    {
      w->obj->data = (struct gui_TextSim*)malloc(sizeof(struct gui_TextSim)); // _free
    }
    else
    {
      w->obj->data = _ts;
    }
    ts = w->obj->data;
    w->obj->id = type;
    ts->str = (char*)malloc(strlen(str)+1);
    strcpy(ts->str,str);
    ts->placement = -1;
    
    if (_ts==NULL)
    {
      ts->pos.x	= x;
      ts->pos.y	= y;
      ts->pos.w	= 0;
      ts->pos.h	= 0;
    }
    
    return 0;
  }

  /** change text at an absolute position
   *  if there already is a text object at that position, we replace it
   */
  int gui_WidgetChangeTextPos( struct gui_Widget *w, char *str, int x, int y )
  {
    #ifdef _DEBUG
      printf("gui_WidgetChangeTextPos\n");
    #endif
    
    if (!w || !str)
    {
      #ifdef _DEBUG
        printf("gui_WidgetChangeTextPos ERROR invalid arguments\n");
      #endif
      return 1;
    }
    
    struct gui_object *current = w->obj;
    struct gui_object *otext = NULL;
    struct gui_TextSim *ts = NULL;
    while(current)
    {
      if (current->id==GOID_TEXTSIMPOS)
      {
        ts = (struct gui_TextSim*)current->data;
        if (
          ts->pos.x == x &&
          ts->pos.y == y
        )
        {
          otext = current;
          if (ts->str)
            free(ts->str);
          ts->str = (char*)malloc(strlen(str)+1);
          strcpy(ts->str,str);
        }
      }
      current = current->next;
    }
    
    if (!otext)
      _gui_widget_add_text_simple_pos(w,NULL,GOID_TEXTSIMPOS,str,x,y);
    
    return 0;
  }

  int _gui_widget_add_text_simple(
                                  struct gui_Widget *w,
                                  struct gui_TextSim *_ts,
                                  int type, 
                                  char *str, int placement
                                  )
  {	// todo - refactoring
    static struct gui_TextSim *ts = NULL;
    struct gui_object *current = w->obj;
    if (current == NULL)
    {
      w->obj = (struct gui_object *)malloc(sizeof(struct gui_object));
      w->obj->next = NULL;
    }
    else
    {
      current = (struct gui_object *)malloc(sizeof(struct gui_object));
      current->next = w->obj;
      w->obj = current;
    }
    if (_ts==NULL)
    {
      w->obj->data = (struct gui_TextSim*)malloc(sizeof(struct gui_TextSim)); // _free
    }
    else
    {
      w->obj->data = _ts;
    }
    ts = w->obj->data;
    w->obj->id = type;
    ts->str = (char*)malloc(strlen(str)+1);
    strcpy(ts->str,str);
    ts->placement = placement;
    
    if (_ts==NULL)
    {
      ts->pos.x	= 0;
      ts->pos.y	= 0;
      ts->pos.w	= 0;
      ts->pos.h	= 0;
    }
    
    return 0;
  }

  /**
   * add text, passing string data and position
   */
  int gui_WidgetAddTextSimpleB( struct gui_Widget *w, char *str, int placement )
  { return _gui_widget_add_text_simple(w,NULL,GOID_TEXTSIMB,str,placement); }

  int gui_WidgetAddTextSimpleC( struct gui_Widget *w, char *str, int placement )
  { return _gui_widget_add_text_simple(w,NULL,GOID_TEXTSIMC,str,placement); }

  /**
   * add preconfigured text object 
   */
  int gui_WidgetAddTextSimpleD( struct gui_Widget *w, struct gui_TextSim *ts, char *str,
    int placement )
  { return _gui_widget_add_text_simple(w,ts,GOID_TEXTSIMD,str,placement); }

  /**
   * button can have only one text object
   */
  int form_ButtonSetText( struct gui_Widget *w, char *str )
  { return _gui_widget_add_gobj(w,str,0,GOID_TEXTSIM); }
  
  /**
   * set graphical object on widget, in general 
   */
  int _gui_widget_set_goid( struct gui_Widget *w, int type, void *value, int nvalue )
  {
    struct gui_object *current = w->obj;
    if (current == NULL)
    {
      w->obj = (struct gui_object *)malloc(sizeof(struct gui_object));
      w->obj->id = type;
      if (value)
        w->obj->data = value;
      else
        w->obj->value = nvalue;
      w->obj->next = NULL;
    }
    else
    {
      current = (struct gui_object *)malloc(sizeof(struct gui_object));
      current->next = w->obj;
      current->id = type;
      if (value)
        current->data = value;
      else
        current->value = nvalue;
      w->obj = current;
    }
    return 0;
  }
  
  int gui_WidgetSetRound( struct gui_Widget *w, int rand )
  {
    struct gui_object *current = w->obj;
    
    while(current)
    {
      if (current->id==GCFG_ROUND) 
      {
        current->value = rand;
        return 0;
      }
      current = current->next;
    }
    
    return _gui_widget_add_gobj(w,NULL,rand,GCFG_ROUND);
  }

  BOOL gui_WidgetIsRound( struct gui_Widget *w )
  {
    struct gui_object *current = NULL;
    current = w->obj;
    
    while(current)
    {
      if (current->id==GCFG_ROUND) 
      {
        return TRUE;
      }
      current = current->next;
    }
    
    return FALSE;
  }
  
  int gui_WidgetSetGradient( struct gui_Widget *w, int grad )
  { return _gui_widget_add_gobj(w,NULL,grad,GCFG_GRADIENT); }
  
  /**
   * the color under the area of the widget is darkened/lightened 
   */
  int gui_WidgetSetBgAlpha( struct gui_Widget *w, int value )
  {
    if (!w) return -1;
    _gui_widget_set_goid(w,GOID_BGALPHA,NULL,value);
    return 0;
  }
  
  int gui_WidgetPaintRegion( struct gui_Widget *w,
    int x, int y, int x2, int y2 )
  {
    struct gui_Widget *main = NULL;
    main = lst_widgets;
    while(main != NULL)
    {
      if (main->parent == NULL)
        break;
      main = main->next;
    }
    
    if (main == NULL)
      return -1;
    
    //  todo - has been implemented using XFill*
    //
    return 0;
  }
  
  int gui_WidgetSetVirtual( struct gui_Widget *w, int vw, int vh )
  {
    if (!w)
      return -1;
    w->attr.vw = vw;
    w->attr.vh = vh;
    return 0;
  }
  
  int gui_WidgetMoveVirtual( struct gui_Widget *w, int vx, int vy )
  {
    if (!w)
      return -1;
    if (w->attr.vh == 0 || w->attr.vh == 0)
      return -1;
    
    if ( w->attr.vx + vx >= 0 )
      w->attr.vx += vx;
    else
      w->attr.vx = 0;
    
    if ( w->attr.vw + vy >= 0 )
      w->attr.vy += vy;
    else
      w->attr.vy = 0;
    
    if ( w->attr.vx + w->attr.vw < w->attr.x + w->attr.w )
      w->attr.vx += ( w->attr.vx + w->attr.vw ) - ( w->attr.x + w->attr.w );
    
    if ( w->attr.vy + w->attr.vh < w->attr.y + w->attr.h )
      w->attr.vy += ( w->attr.vy + w->attr.vh ) - ( w->attr.y + w->attr.h );
    
    return 0;
  }
  
  struct gui_Widget *_ai_gui_position_to_object( int x, int y )
  {
    static struct gui_Widget *c = NULL;
    static struct gui_Widget *w;
    c = gui_GetMain();
    w = NULL;
    while(c != NULL)
    {
      if (gui_SceneIsWidget(scene,c) == FALSE)
      {
        c = c->next;
        continue;
      }
      if (
          c->attr.bvisible == TRUE &&
          c->parent == gui_GetMain() &&
          c->attr.x < x && c->attr.x+c->attr.w > x &&
          c->attr.y < y && c->attr.y+c->attr.h > y
          )
      {
          #ifdef _DEBUG
            if (0) if (DEBUG==TRUE) printf("\tfount_widget\n");
          #endif
          w = c;
          if (c->attr.hierarchy == 0)
            break;
      }
      c = c->next;
    }
    return w;
  }
  
  struct gui_Widget *_ai_gui_widget_at( int x, int y )
  {
    static struct gui_Widget *current;
    static struct gui_Widget *smallest;
    
    current = NULL;
    smallest = NULL;
    
    static int nx;
    static int ny;
    
    for(
      ny = y-(IO_TOUCH_FRAME*IO_TOUCH_STEP);
      ny < y+(IO_TOUCH_FRAME*IO_TOUCH_STEP); ny++
    )
    {
      nx = x-(IO_TOUCH_FRAME*IO_TOUCH_STEP);
      for(nx; nx < x+(IO_TOUCH_FRAME*IO_TOUCH_STEP); nx++)
      {
        struct gui_Widget *active = _ai_gui_position_to_object(nx,ny);
        
        if (!smallest && active)
        {
          smallest = active;
          #ifdef _DEBUG
            if (DEBUG==TRUE)
              printf("\t_ai smallest found(%dx%d)\n",
                smallest->attr.w,smallest->attr.h);
          #endif
        }
        else if (smallest!=active && active)
        {
          if (
            (
            (smallest->attr.w*smallest->attr.h)>
            (active->attr.w*active->attr.h)
            )
            &&
            (smallest->attr.w!=0&&smallest->attr.h!=0&&
            active->attr.w!=0&&active->attr.h!=0
            )
          )
          {
            smallest = active;
            #ifdef _DEBUG
              if (DEBUG==TRUE)
                printf("\t_ai smallest found(%dx%d)\n",
                  smallest->attr.w,smallest->attr.h);
            #endif
          }
        }
      }
    }
    
    return smallest;
  }
  
  /** this in current implementation only get's the canvas
   */
  struct gui_Widget *gui_WidgetAt( int x, int y )
  {
    #ifdef _AI_0
      return _ai_gui_widget_at(x,y);
    #else
  
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf(":gui_WidgetAt\n");
      #endif
      struct gui_Widget *c = NULL;
      struct gui_Widget *w = NULL;
      c = gui_GetMain();
      while(c != NULL)
      {
        if (gui_SceneIsWidget(scene,c) == FALSE)
        {
          c = c->next;
          continue;
        }
        if (
            c->attr.bvisible == TRUE &&
            c->parent == gui_GetMain() &&
            c->attr.x < x && c->attr.x+c->attr.w > x &&
            c->attr.y < y && c->attr.y+c->attr.h > y
            )
        {
            #ifdef _DEBUG
              if (DEBUG==TRUE) printf("\tfount_widget\n");
            #endif
            w = c;
            if (c->attr.hierarchy == 0)
              break;
        }
        c = c->next;
      }
      return w;
    #endif
  }
  
  int gui_WidgetToTop( struct gui_Widget *w )
  {
    int highest = 0;
    struct gui_Widget *cw = lst_widgets;
    while(cw != NULL)
    {
      if (cw->attr.hierarchy == 0)
      {
        cw->attr.hierarchy = 1;
      }
      cw = cw->next;
    }
    if (w->attr.hierarchy != 0)
      w->attr.hierarchy = 0;
    return 0;
  }
  
  int gui_WidgetUnTop( struct gui_Widget *w )
  {
    w->attr.hierarchy = 1;
    return 0;
  }

  int gui_WidgetSetPosition(
    struct gui_Widget *widget,
    int x, int y,
    int w, int h
  )
  {
    if (!widget) return -1;
  
    widget->attr.x = x;
    widget->attr.y = y;
    widget->attr.w = w;
    widget->attr.h = h;
  
    return 0;
  }
  
  int gui_WidgetSetPositionRelative(
    struct gui_Widget *widget,
    struct gui_Widget *parent,
    double dx, double dy,
    double dw, double dh
  )
  {
    if (!widget||!parent) return -1;
  
    widget->attr.x = parent->attr.w*dx;
    widget->attr.y = parent->attr.h*dy;
    widget->attr.w = parent->attr.w*dw;
    widget->attr.h = parent->attr.h*dh;
  
    return 0;
  }

int gui_BoundsSet(
  struct gui_Bounds *bounds,
  int x, int y,
  int width, int height
)
{
  if (!bounds) return -1;
  #ifdef _DEBUG
    if (DEBUG==TRUE) printf(":gui_BoundsSet __bounds (%d,%d+%d+%d)\n",x,y,width,height);
  #endif
  bounds->x = x;
  bounds->y = y;
  bounds->width = width;
  bounds->height = height;
  return 0;
}

int gui_BoundsSetEx(
  struct gui_Bounds *bounds,
  struct gui_WidgetAttributes *attr
)
{
  if (!bounds||!attr) return -1;
  bounds->x = attr->x;
  bounds->y = attr->y;
  bounds->width = attr->w;
  bounds->height = attr->h;
  return 0;
}
  