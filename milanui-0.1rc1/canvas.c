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
#include "ai.h"

#ifdef USEX

  #define _CANVAS_EFFECTS
  
  struct _can_content *_can_content_init(
    struct _can_content *content
  )
  {
    if (content==NULL)
    {
      content = (struct _can_content*)malloc(sizeof(struct _can_content));
    }
    content->fillx 	= DEFA_CSMAR; // todo, duplicate init
    content->bottomy	= DEFA_CSMARTB;
    content->filly 	= content->bottomy;
    content->bottomy	+= DEFA_CSMAR;
    content->w 		= 0;
    content->h 		= 0;
    content->movey 	= 0;
    lst_can_content = trop_ListAdd(lst_can_content,content);
    return content;
  }
 
  struct _can_content *form_CanvasGetContentEx(
    struct gui_Widget *wi
  )
  {
    static struct form_Canvas *can = NULL;
    
    if (!wi) return NULL;
    if (wi->form==NULL) return NULL;
    can = wi->form;
    
    return can->content;
  }
  
  struct _can_obj *form_CanvasGetContent(
    struct gui_Widget *wi
  )
  {
    if (!wi) return NULL;
    else return ((struct form_Canvas*)wi->form)->can_pool;
  }
 
  int form_CanvasSetContentEx(
    struct gui_Widget *wi,
    struct _can_content *content
  )
  {
    static struct form_Canvas *can = NULL;
    
    if (!wi||!content) return -1;
    
    can->content = content;
    
    return 0;
  }
  
  /*
    sets the canvases objects 
  */
  int form_CanvasSetContent(  
    struct gui_Widget *wi,
    struct _can_obj *obj
  )
  {
    if (!wi || !obj) return -1;
    return 0;
  }
  
  int form_CanvasMoveCheck( struct gui_Widget *wi, int *movex, int movey )
  {
    if (!wi||!movex||!movey) return -1;
    return 0;
  }
  
  /**
   * 'zotrvacnost' when sliding 
   */
  int form_CanvasEffectExtraSlide( struct gui_Widget *wi )
  {
    static struct form_Canvas *can = NULL;
    static struct _can_obj *cano = NULL;
    static struct interface_event_t ievent;
    static int i;
    
    static int val;
    static int yhigh;
    static int abs_move;
    static int mid_move;
    static int nmem;
    
    if (!wi) return -1;
    
    val 		= 0;
    yhigh		= -1;
    abs_move 		= 0;
    mid_move 		= 0;
    nmem 		= 0;
    
    can = wi->form;
    
    for (i = 0; i < CAN_VRMOVE; i++)
    {
      abs_move += abs(can->vrmove[i]);
      if (can->vrmove[i]!=0) nmem++;
    }
    
    if (nmem<2) return -1;
    
    if (nmem>0)
      mid_move = abs_move / nmem;
    
    if (can->vrmove[0]<0)
      mid_move=-mid_move;
    
    for (i = 0; i < nmem; i++)
      val += mid_move/(nmem/1.5);
    
    if ((can->movey>0 && val>0) || can->movey+val>0) // upper limit
    {
      //if (can->movey<0) val = -can->movey;
      //else return -1;
      return -1;
    }
    
    cano = can->can_pool;
    while(cano!=NULL)
    {
      if (yhigh < cano->pos.y+cano->pos.h)
        yhigh = cano->pos.y+cano->pos.h;
      cano = cano->next;
    }
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("CanvasMove yhigh(%d)\n",yhigh);
    #endif
    if (can->movey+yhigh+DEFA_CSMARTB < wi->attr.h && val<0) // lower limit 
    {
      return -1;
    }
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("__canvas EffectExtraSlide mid(%d) nmem(%d)\n",
        mid_move,nmem);
    #endif
    
    for (i = 0; i < nmem; i++)
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("__canvas EffectExtraSlide expose\n");
      #endif
      
      if ( form_CanvasMove(wi,mid_move) != 0 )
        break;
      
      //can->movey += mid_move;
      mid_move -= mid_move/(nmem/1.5);
      #ifdef _USE_GUI_XLIB_INTERNALS
        gui_EventExpose(NULL);
      #else
        ievent.type = INTERFACE_EVENT_EXPOSE;
        ievent.x = 0;
        ievent.y = 0;
        ievent.width = 640;
        ievent.height = 480;
        //bigInterface->addGuiEvent(bigInterface,&ievent);
        gui_WidgetPushEvent(&ievent);
      #endif
      usleep(5000);
    }
    
    return 0;
  }
  
  int form_CanvasMotion( struct gui_Event *event )
  {
    static struct form_Canvas *can = NULL;
    static int lastx = -1;
    static int lasty = -1;
    static int i;
    
    if (event==NULL)
    {
      lastx = -1;
      lasty = -1;
      return 0;
    }
    
    can = event->widget->form;
    
    #ifdef _CANVAS_EFFECTS
      for (i = CAN_VRMOVE-1; i > -1; i--)
      {
        if (i==CAN_VRMOVE-1) continue;
        can->vrmove[i+1] = can->vrmove[i];
      }
    #endif
    
    if (lastx!=-1&&lasty!=-1)
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf(":canvas_move (%d)\n",event->y-lasty);
      #endif
      form_CanvasMove(event->widget,event->y-lasty);
      
      #ifdef _CANVAS_EFFECTS
        can->vrmove[0] = event->y-lasty;
      #endif
      
      expose = event->widget;
      gui_TriggerExposeWidget(gui_GetMain(),event->widget);
    }
    lastx = event->x;
    lasty = event->y;
    return 0;
  }
  
  int form_CanvasRelease( struct gui_Event *event )
  {
    form_CanvasMotion(NULL);
    #ifdef _CANVAS_EFFECTS
      form_CanvasEffectExtraSlide( event->widget );
    #endif
    return 0;
  }
  
  #define _DETAIL_NO
  struct _can_obj *_ai_gui_position_to_object_ex( struct gui_Event *event )
  {
    struct _can_obj 		*object = NULL;
    static struct form_Canvas 	*can = NULL;
    static struct _can_obj 	*cano = NULL;
    
    int movey = 0;
     
    can = event->widget->form;	// get canvas parent
    cano = can->can_pool;	// first canvas object
    if (!cano) return NULL;
    
    while(cano)
    {
      if (cano->bstatic==FALSE) movey = can->movey;
      else			movey = 0;
      
      if (
        (cano->pos.y+movey < event->y && 
        cano->pos.y+cano->pos.h+movey > event->y) 
        &&
        (cano->pos.x+movey < event->x &&
        cano->pos.x+cano->pos.w > event->x)
        )
      {
        object = cano;
        break;
      }
      
      cano = cano->next;
    }
    
    return object;
  }
  
  /**  button press when applied _AI logic, the other methods
   *   should be marked as 'deprecated', need time to rewrite
   */
  int _ai_form_canvas_button_press_ex( struct gui_Event *event )
  {
    struct form_Canvas 	*can = NULL;
    struct _can_obj 	*cano = NULL;
    static int 		movey = 0;
    static int 		i;
    struct _can_obj *active = NULL;
    struct _can_obj *smallest = NULL;
  
    if (!event) 	return -1;
    if (!event->widget) return -1;
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("_ai_form_canvas_button_press_ex\n");
    #endif
    
    can = event->widget->form;
    cano = can->can_pool;
    
    #ifdef _CANVAS_EFFECTS
      for (i = 0; i < CAN_VRMOVE; i++)
      {
        can->vrmove[i] = 0;
        can->svrmove[i].l = 0;
        can->svrmove[i].t = 0;
      }
    #endif
    
    int x = event->x;
    int y = event->y;
    
    int nx;
    int ny;
    
    struct gui_Event bogus;
    memcpy((void*)&bogus,(void*)event,sizeof(struct gui_Event));
    
    for(
      ny = y-(IO_TOUCH_FRAME*IO_TOUCH_STEP);
      ny < y+(IO_TOUCH_FRAME*IO_TOUCH_STEP); ny++
    )
    {
      nx = x-(IO_TOUCH_FRAME*IO_TOUCH_STEP);
      for(nx; nx < x+(IO_TOUCH_FRAME*IO_TOUCH_STEP); nx++)
      {
        bogus.x = nx;
        bogus.y = ny;
        
        active = _ai_gui_position_to_object_ex(&bogus);
        
        if (!smallest && active)
        {
          if (active->widget->attr.onclick)
            smallest = active;
        }
        else if (smallest!=active && active)
        {
          if (
            (smallest->widget->attr.w*smallest->widget->attr.h)>
            (active->widget->attr.w*active->widget->attr.h)
          )
          {
            if (active->widget->attr.onclick)
              smallest = active;
          }
          else if (
            (smallest->widget->attr.w*smallest->widget->attr.h)==
            (active->widget->attr.w*active->widget->attr.h)
          )
          {
            /** which object is closer, if they are of the same size
             */
            int activex1 = active->widget->attr.x+(active->widget->attr.w/2);
            int activey1 = active->widget->attr.y+(active->widget->attr.h/2);
            
            int smallestx1 = smallest->widget->attr.x+(smallest->widget->attr.w/2);
            int smallesty1 = smallest->widget->attr.y+(smallest->widget->attr.h/2);
          
            /** we take in account here the real sensor touch (evaluate)
             */
            if (
              abs(x-activex1)*abs(y-activey1) <
              abs(x-smallestx1)*abs(y-smallesty1)
            )
            {
              if (active->widget->attr.onclick)
                smallest = active;
            }
          }
        }
        
      }
    }
    
          if (smallest)
          {
            cano = smallest;
            
            event->sub = event->widget;
            event->widget = cano->widget;
            
            if (cano->bstatic==FALSE) 	movey = can->movey;
            else			movey = 0;
            
            switch(cano->widget->attr.form)
            {
              case FORM_LBENTRY:
                event->x = event->x - cano->pos.x;
                event->y = event->y - (cano->pos.y+movey);
                form_LBEntryButtonPress(event);
                return 0;
              break;
              case FORM_KEYBOARD:
                event->x = event->x - cano->pos.x;
                event->y = event->y - (cano->pos.y+movey);
                form_KeyboardButtonPress(event);
                return 0;
              break;
              default:
                #ifdef _DEBUG
                  if (DEBUG==TRUE)
                    printf("_ai canvas press (%dx%d) widget(%p) #2\n",
                      event->x,event->y,event->widget);
                #endif
                
                gui_WidgetButtonPress(event);
                return 0;
              break;
            }
          }
    
    return 0;
  }
  
  /**
   *  Button Press in canvas widget 
   */
  int form_CanvasButtonPress( struct gui_Event *event )
  {
    #ifdef _AI  
      struct gui_Event bogus;
      memcpy((void*)&bogus,(void*)event,sizeof(struct gui_Event));
      int ret = _ai_form_canvas_button_press_ex((struct gui_Event*)&bogus);
      return ret;
    #else
      static struct form_Canvas *can = NULL;
      static struct _can_obj *cano = NULL;
      static int movey = 0;
      static int i;
    
      if (!event) return -1;
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf(":form_CanvasButtonPress\n");
      #endif
    
      can = event->widget->form;
      cano = can->can_pool;
    
      #ifdef _CANVAS_EFFECTS
        for (i = 0; i < CAN_VRMOVE; i++)
        {
          can->vrmove[i] = 0;
          can->svrmove[i].l = 0;
          can->svrmove[i].t = 0;
        }
      #endif
    
      while(cano!=NULL)
      {
        if (cano->bstatic==FALSE) movey = can->movey;
        else			movey = 0;
      
        if (
          (cano->pos.y+movey < event->y && 
          cano->pos.y+cano->pos.h+movey > event->y) 
          &&
          (cano->pos.x+movey < event->x &&
          cano->pos.x+cano->pos.w > event->x)
          )
        {
          event->sub = event->widget;
          event->widget = cano->widget;
          if (cano->widget->attr.form == FORM_LBENTRY)
          {
            event->x = event->x - cano->pos.x;
            event->y = event->y - (cano->pos.y+movey);
            form_LBEntryButtonPress(event);
            break;
          }
          else if (cano->widget->attr.form == FORM_KEYBOARD)
          {
            event->x = event->x - cano->pos.x;
            event->y = event->y - (cano->pos.y+movey);
            form_KeyboardButtonPress(event);
            break;
          }
          else
          {
            #ifdef _DEBUG
              printf("_ai canvas press (%dx%d) widget(%p) #2\n",
                event->x,event->y,event->widget);
            #endif
      
            gui_WidgetButtonPress(event);
            break;
          }
        }
      
        cano = cano->next;
      }
    
      return 0;
    #endif
  }
  
  int _form_canvas_expose_slidebar( struct gui_Widget *w, int ylow, int yhigh )
  {
    static int abs_height = 0;
    static int abs_slide_h = 0;
    static int abs_slide_y = 0;
    static struct gui_Surface *dummy_surface = NULL;
    static struct form_Canvas *can = NULL;
    if (!w) return -1;
    
    can = (struct form_Canvas*)w->form;
    if (can)
    {
      if (can->movetype==CAN_MOV_NONE) return 0;
    }
    
    if (dummy_surface==NULL)
    {
      dummy_surface = (struct gui_Surface*)malloc(sizeof(struct gui_Surface));
      dummy_surface->image = (struct gui_Image*)malloc(sizeof(struct gui_Image));
      #ifdef _USE_GUI_XLIB_INTERNALS
        dummy_surface->image->ximage = gui_GetMain()->surface;
      #else
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf(":warning! - canvas not supporting dummy_surface\n");
        #endif
      #endif
    }
    abs_height = yhigh-ylow;
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":abs_height(%d)\n",abs_height);
    #endif
    abs_slide_h = (int)((float)w->attr.h/(float)((float)abs_height/(float)w->attr.h));
    abs_slide_y = (int)((float)((float)abs(ylow)/(float)abs_height)*(float)w->attr.h);
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":abs_slide_h (%d) cons(%d > %d)\n",abs_slide_h,
        w->attr.y+abs_slide_y+abs_slide_h,w->attr.y+w->attr.h);
    #endif
    if (w->attr.y+abs_slide_y+abs_slide_h > w->attr.y+w->attr.h)
    {
      abs_slide_h -= (w->attr.y+abs_slide_y+abs_slide_h) - (w->attr.y+w->attr.h);
    }
    // todo - rectangle color dependant on background
    gui_DrawRoundRectangle(
      NULL,
      w->attr.x+w->attr.w-6,
      abs_slide_y+w->attr.y,
      5,abs_slide_h,
      1
    );
    return 0;
  }

  /**
   * ListboxEntry cleanup 
   */
  int form_LBEntryFree( struct gui_Widget *wi )
  {
    static struct form_ListboxEntry *lbe = NULL;
    if (!wi) return -1;
    if (!wi->form) return -1;
    lbe = wi->form;
    if (lbe->str_big!=NULL) 
    {
      free(lbe->str_big);
      lbe->str_big = NULL;
    }
    if (lbe->str_small!=NULL)
    {
      free(lbe->str_small);
      lbe->str_small = NULL;
    }
    free(lbe);
    wi->form = NULL;
    return 0;
  }

  /*
    expose Listbox Entry, can be inserted in Canvas 
    TODO - make fully compatible with gui_WidgetExpose
  
    - create one unified widget drawing routine
  */
  int form_LBEntryExpose( struct gui_Widget *wi, 
    int x, int y, 
    int w, int h )
  {
    static struct form_ListboxEntry *lbe = NULL;
    static int vw, vh, vx, vy, px, py;
    static struct gui_Widget *parent = NULL;
    static struct gui_Image *arrow = NULL;
    
    static int tmpx;
    static int tmpy;
    static int tmpw;
    static int tmph;
    
    #ifdef _DEBUG
      if (DEBUG==TRUE)
        printf("form_LBEntryExpose (%d) (%d)\n",wi->attr.w,wi->attr.h);
    #endif
    if (!wi) return -1;
    
    if (wi->attr.w<100 || wi->attr.h<20) return -1; // we have a minumum size which to accept
    parent = wi->parent;
    if (x>=parent->attr.w || y>=parent->attr.h) return -1;
    
    if (parent != gui_GetMain()->parent)
    {
      px = parent->attr.x;
      py = parent->attr.y;
    }
    
    //if (y+parent->attr.y+h > parent->attr.y+parent->attr.h) return -1;
    
    lbe = (struct form_ListboxEntry*)wi->form;
    vh = gui_GetMain()->attr.ftsize+2;
    vw = wi->attr.w-(wi->attr.h*2);
    
    gui_WidgetExpose(
      wi,
      x, y,
      w, h
    );
    
    vx = x+parent->attr.x;
    vy = y+parent->attr.y;
    
    tmpx = gui_bounds.x;
    tmpy = gui_bounds.y;
    tmpw = gui_bounds.width;
    tmph = gui_bounds.height;
    gui_BoundsSet(
              &gui_bounds,
              parent->attr.x,parent->attr.y,
              parent->attr.w,parent->attr.h);
    
    vx = x+parent->attr.x+wi->attr.h;
    vy = y+parent->attr.y;
    
    if (lbe->str_big)
    {
      if (
        vx > parent->attr.x &&
        vy > parent->attr.y &&
        vx+vw < parent->attr.x+parent->attr.w &&
        vy+vh < parent->attr.y+parent->attr.h
      )
      {
        gui_DrawTextEx(
          (char*)lbe->str_big,
          vx,vy,
          vw,vh,DEFA_TXTR);
      }
    }
    
    if (lbe->str_small)
    {
      vy += (wi->attr.h/2);
      
      if (
        vx > parent->attr.x &&
        vy > parent->attr.y &&
        vx+vw < parent->attr.x+parent->attr.w &&
        vy+vh < parent->attr.y+parent->attr.h
      )
      {
        gui_DrawTextEx(
          (char*)lbe->str_small,
          vx,vy,
          vw,vh,DEFA_TXTR);
      }
    }
    
    if (lbe->image)
    {
      vx = x+parent->attr.x;
      vy = y+parent->attr.y;
    
      gui_DrawImage(
        lbe->image,
        vx+DEFA_ROUND,
        vy+DEFA_ROUND,
        wi->attr.h-(DEFA_ROUND*2),
        wi->attr.h-(DEFA_ROUND*2));
    }
    
    arrow = gui_ImagePick("IMG_ARROW");
    
    if (arrow)
    {
      if (arrow->w>wi->attr.h) 	vw = wi->attr.h;
      else			vw = arrow->w;
    
      if (arrow->h>wi->attr.h)	vh = wi->attr.h;
      else			vh = arrow->h;  
      
      if (arrow!=NULL)
      {
        vx = x+parent->attr.x+wi->attr.w-wi->attr.h;
        gui_DrawImage(
          arrow,
          vx+DEFA_ROUND, 
          vy+DEFA_ROUND,
          vw-(DEFA_ROUND*2),
          vh-(DEFA_ROUND*2)
        );
      }
    }
    
    gui_BoundsSet(&gui_bounds,tmpx,tmpy,tmpw,tmph);
    
    return 0;
  }
  
  #define _RESTRICY_BUTTON_PRESS_NO
  int form_LBEntryButtonPress( struct gui_Event *event )
  {
    static struct form_Canvas *can = NULL;
    if (!event) return -1;
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":form_LBEntryButtonPress (%d) (%d)\n",event->x,event->y);
    #endif
    can = event->widget->form;
    #ifdef _RESTRICT_BUTTON_PRESS
      if (event->x > event->widget->attr.w-event->widget->attr.h)
    #endif
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf(":form_LBEntryButtonPress go\n");
      #endif
      gui_WidgetButtonPress(event);
    }
    return 0;
  }
  
  void *form_CanvasExposeCall(
                              void *obj,
                              int x, int y,
                              int w, int h
                              )
  {
    form_CanvasExpose((struct gui_Widget*)obj);
    
    return NULL;
  }
  
  int form_CanvasExpose( struct gui_Widget *w )
  {
    static struct form_Canvas *can = NULL;
    static struct _can_obj *cano = NULL;
    static int ylow = 0;
    static int yhigh = 0;
    static int movey = 0;
    static int n;
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":form_CanvasExpose\n");
    #endif
    if (!w) return -1;
    gui_WidgetExpose(w,
      w->attr.x,
      w->attr.y,
      w->attr.w,
      w->attr.h);
    can = w->form;
    ylow = 999000;
    yhigh = -999000;
    
    for (n = GUI_LEVEL_MIN; n < GUI_LEVELS; n++)
    {
     cano = can->can_pool;
     while(cano!=NULL)
     {
      
      if (cano->widget)
      {
        if (cano->widget->attr.level!=n)
        {
          cano = cano->next;
          continue;
        }
      }
     
      #ifdef _DEBUG
        if (DEBUG==TRUE) 
          printf(":canvas object expose (%dx%d) at(%d %d) (%p)\n",
            cano->pos.w,cano->pos.h,cano->pos.x,cano->pos.y,cano);
      #endif
      
      if (cano->widget!=NULL)
      {
        if (cano->bstatic == TRUE)	movey = 0;
        else				movey = can->movey;
        
        if (cano->pos.y+movey>-1)
        {
          if (cano->widget->attr.form == FORM_LBENTRY)
          {
            form_LBEntryExpose(
              cano->widget,
              cano->pos.x,
              cano->pos.y+movey,
              cano->pos.w,
              cano->pos.h
              );
          }
          else if (cano->widget->attr.form == FORM_EDITABLE)
          {
            form_EditableExpose(
              cano->widget,
              cano->pos.x,
              cano->pos.y+movey,
              cano->pos.w,
              cano->pos.h
              );
          }
          else
          {
            gui_WidgetExpose(
              cano->widget,
              cano->pos.x,
              cano->pos.y+movey,
              cano->pos.w,
              cano->pos.h
            );
          }	
        } // else abstract canvas object
      }
      
      if (yhigh < cano->pos.y+can->movey+cano->pos.h)
        yhigh = cano->pos.y+can->movey+cano->pos.h;
      if (ylow > cano->pos.y+can->movey)
        ylow = cano->pos.y+can->movey;
      cano = cano->next;
     }
    }
    if (can->movetype!=CAN_MOV_NONE)
      _form_canvas_expose_slidebar(w,ylow,yhigh);
    return 0;
  }
  
  int form_CanvasMove( struct gui_Widget *w, int val )
  {
    static struct form_Canvas *can = NULL;
    static struct _can_obj *cano = NULL;
    static int yhigh;
    static int i;
    
    if (!w) return -1;
    
    can = w->form;
    if (can->movetype==CAN_MOV_NONE) return -1;
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("CanvasMove move_by (%d)\n",val);
    #endif
    
    yhigh = -1;
    
    if ((can->movey>0 && val>0) || can->movey+val>0) // upper limit
    {
      if (can->movey<0) val = -can->movey;
      else return -1;
    }
    
    cano = can->can_pool;
    while(cano!=NULL)
    {
      if (yhigh < cano->pos.y+cano->pos.h)
        yhigh = cano->pos.y+cano->pos.h;
      cano = cano->next;
    }
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("CanvasMove yhigh(%d)\n",yhigh);
    #endif
    if (can->movey+yhigh+DEFA_CSMARTB < w->attr.h && val<0) // lower limit 
    {
      return -1;
    }
    
    for (i = CAN_VRMOVE-1; i > -1; i--) // movement vectors cache
    {
      if (i==CAN_VRMOVE-1) continue;
      can->vrmove[i+1] = can->vrmove[i];
    }
    can->vrmove[0] = val;
    
    if (can->movey+yhigh+DEFA_CSMARTB+val < w->attr.h && val<0)
    {
      val += w->attr.h - (can->movey+yhigh+DEFA_CSMARTB+val);
    }
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("CanvasMove move_by 2 movey(%d) val(%d)\n",can->movey,val);
    #endif
    can->movey += val;
    return 0;
  }
  
  /**
   * add newline - abstract canvas object without binding to a gui_Widget 
   */
  int form_CanvasAddNewline( struct gui_Widget *w )
  {
    static struct form_Canvas *can = NULL;
    static struct _can_obj *current = NULL;
    if (!w) return -1;
    can = w->form;
    if (!can) return -1;
    
    if (can->can_pool==NULL)
    {
      can->can_pool = (struct _can_obj*)malloc(sizeof(struct _can_obj));
      current = can->can_pool;
    }
    else
    {
      current = can->can_pool;
      while(current->next!=NULL)
      {
        current = current->next;
      }
      current->next = (struct _can_obj*)malloc(sizeof(struct _can_obj));
      current = current->next;
    }
    current->pos.w = 0;
    current->pos.h = 0;
    
    can->fillx = DEFA_CSMAR;
    can->filly = can->bottomy+DEFA_CSMAR;
    
    current->pos.x = can->fillx;
    current->pos.y = can->filly;
    
    can->filly+=DEFA_CSMAR;
    
    current->next = NULL;
    current->widget = NULL;
    current->type = CAN_BR;
    
    return 0;
  }
  
  /**
   * todo - refactoring 
   */
  int _form_canvas_add_object( struct gui_Widget *w, struct gui_Widget *child, BOOL bstatic )
  {
    static struct form_Canvas *can = NULL;
    static struct _can_obj *current = NULL;
    
    if (!w || !child) return -1;
    can = w->form;
    if (!can) return -1;
    
    if (can->can_pool==NULL)
    {
      can->can_pool = (struct _can_obj*)malloc(sizeof(struct _can_obj));
      current = can->can_pool;
    }
    else
    {
      current = can->can_pool;
      while(current->next!=NULL)
      {
        current = current->next;
      }
      current->next = (struct _can_obj*)malloc(sizeof(struct _can_obj));
      current = current->next;
    }
    
    if (child->attr.form == FORM_LBENTRY)
    {
      child->attr.w = w->attr.w-DEFA_CSMARRL;
    }
    
    current->pos.w = child->attr.w;
    current->pos.h = child->attr.h;
    
    // todo handle special cases
    
    current->bstatic = bstatic;
    
    if (bstatic==FALSE)
    {
      if ((can->fillx+child->attr.w) > w->attr.w-DEFA_CSMAR) // start next line
      {
        can->fillx = child->attr.w+DEFA_CSMAR+DEFA_CSMAR;
        can->filly = can->bottomy+DEFA_CSMAR;
        current->pos.x = DEFA_CSMAR;
        current->pos.y = can->filly;
        can->bottomy = can->filly+current->pos.h + DEFA_CSMAR;
      }
      else // continue current line
      {
        current->pos.x = can->fillx;
        current->pos.y = can->filly;
        can->fillx += child->attr.w+DEFA_CSMAR;
        if (can->filly+current->pos.h > can->bottomy)
        {
          can->bottomy = can->filly+current->pos.h+DEFA_CSMAR; // +DEFA_CSMAR
        }
      }
    }
    else
    {
      current->pos.x = child->attr.x;
      current->pos.y = child->attr.y;
    }
    
    if (child->parent!=NULL) child->parent = w;
    current->next = NULL;
    current->widget = child;
    current->type = CAN_OBJ;
    return 0;
  }
  
  int form_CanvasAddObject( struct gui_Widget *w, struct gui_Widget *child )
  {
    return _form_canvas_add_object(w,child,FALSE);
  }

  /** in this call the position of the child is not ignored and it is not changed on sliding 
   */
  int form_CanvasAddObjectStatic( struct gui_Widget *w, struct gui_Widget *child )
  {
    return _form_canvas_add_object(w,child,TRUE);
  }
  
  /**
   *  remove canvas objects in more ways
   */
  int _form_canvas_remove_all_ex(
    struct gui_Widget *wi,
    BOOL brecursive,
    int form_type
  )
  {
    struct _can_obj     *cano     = NULL;
    struct _can_obj     *canoLast = NULL;
    struct _can_obj     *canoNext = NULL;
    struct form_Canvas  *can      = NULL;
    
    if (!wi) return -1;
    if (wi->attr.form!=FORM_CANVAS) return -1;
    
    can = wi->form;
    cano = can->can_pool;
    canoLast = NULL;
    can->can_pool = NULL;
    
    while(cano)
    {
        canoNext = cano->next;
        
        if ( // remove only type form_type object
            cano->widget->attr.form!=form_type &&
            form_type != -1
            )
        {
          if (canoLast==NULL)
            can->can_pool = cano;
          else
            canoLast->next = cano;
          canoLast = cano;
          canoLast->next = NULL;
          cano = canoNext;
          continue;
        }
      
        if (brecursive==TRUE)
        {
          if (cano->widget)
          {
            gui_WidgetReleaseEx(cano->widget);
          }
        }
        else
        {
          
        }
        
        free(cano);
        
        cano = canoNext;
    }
      
    return 0;
  }
  
  /**
   *  remove all canvas object refferences
   */
  int form_CanvasRemoveAllObjRef( struct gui_Widget *wi )
  {
    return _form_canvas_remove_all_ex(wi,FALSE,-1);
  }
  
  /**
   *  remove all canvas objects and refferences recursivelly
   */
  int form_CanvasRemoveAllObj( struct gui_Widget *wi )
  {
    return _form_canvas_remove_all_ex(wi,TRUE,-1);
  }

  int form_CanvasRemoveAllObjRefType( struct gui_Widget *wi, int form_type )
  {
    return _form_canvas_remove_all_ex(wi,FALSE,form_type);
  }

  #define _CANVAS_SURFACE
  int _form_canvas_init( struct gui_Widget *w, BOOL autopos )
  {
    static struct form_Canvas *can = NULL;
    if (!w) return -1;
    if (autopos==TRUE || 
      ( w->attr.w == 0 && w->attr.h == 0 ) )
    {
      
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("CANVAS auto size canvas\n");
      #endif
      
      //w->attr.x = gui_GetMain()->attr.h*0.10;
      //w->attr.y = gui_GetMain()->attr.w*0.05;
      //w->attr.h = gui_GetMain()->attr.h*0.90;
      //w->attr.w = gui_GetMain()->attr.w*0.90;
      
      //w->attr.x = 0;
      //w->attr.y = 0;
      
      //w->attr.h = gui_GetMain()->attr.h;
      //w->attr.w = gui_GetMain()->attr.w;
    
      //w->attr.h = 340;
      //w->attr.w = 440;
    }
    else
    {
      w->attr.x = 0;
      w->attr.y = 0;
      w->attr.w = 0;
      w->attr.h = 0;
    }
    
    w->attr.onexposeEx = form_CanvasExposeCall;
    
    gui_WidgetSetColor(w,DEFA_CSBG);
    w->form 		= (struct form_Canvas*)malloc(sizeof(struct form_Canvas));
    can 		= (struct form_Canvas*)w->form;
    can->fillx 		= DEFA_CSMAR;
    can->bottomy 	= DEFA_CSMARTB;
    can->filly 		= can->bottomy;
    can->bottomy	+= DEFA_CSMAR;
    can->w 		= 0;
    can->h 		= 0;
    can->can_pool 	= NULL;
    can->movey 		= 0;
    can->movetype 	= CAN_MOV_SLIDE;
    can->content	= NULL;
    
    #ifdef _CANVAS_SURFACE
      can->surface = gui_SurfaceInitEx( gui_GetMain(),NULL,w->attr.w,w->attr.h);
    #else
      can->surface = NULL;
    #endif
    
    // test
    form_CanvasDataModelInitFromCSV(w,"test.csv");
    
    return 0;
  }
  
  int form_CanvasInit( struct gui_Widget *w )
  {
    return _form_canvas_init(w,TRUE);
  }

  void *_can_wup_press( void *data )
  {
    static struct gui_Event *event = NULL;
    event = (struct gui_Event*)data;
    event->widget = event->sub;
    event->x = 0;
    event->y = 0;
    form_CanvasMotion(event);
    event->x = 0;
    event->y = 100;
    form_CanvasMotion(event);
    form_CanvasRelease(event);
  }

  void *_can_wdown_press( void *data )
  {
    static struct gui_Event *event = NULL;
    event = (gui_Event*)data;
    event->widget = event->sub;
    event->x = 0;
    event->y = 100;
    form_CanvasMotion(event);
    event->x = 0;
    event->y = 0;
    form_CanvasMotion(event);
    form_CanvasRelease(event);
  }

  int form_CanvasSetMov( struct gui_Widget *w, int movetype )
  {
    static struct form_Canvas *can = NULL;
    if (!w) return -1;
    
    can = (struct form_Canvas*)w->form;
    can->movetype = movetype;
    
    if (movetype==CAN_MOV_NONE)
      return 0;
    
    struct gui_Image *img_aup = NULL;
    struct gui_Image *img_adown = NULL;
    
    int imgx = 0;
    int imgy = 0;
    
    gui_ImageRegisterSimple(FIMG_AUP,"ARROW_UP");
    gui_ImageRegisterSimple(FIMG_ADOWN,"ARROW_DOWN");
    
    img_aup = gui_ImagePick("ARROW_UP");
    img_adown = gui_ImagePick("ARROW_DOWN");
    
    if (!img_aup||!img_adown)
    {
      printf("Could not find arrow images\n");
    }
    else
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE)
        {
          printf("arrow images: (%dx%d) (%dx%d)\n",
            img_aup->w,img_aup->h,
            img_adown->w,img_adown->h
          );
        }
      #endif
    }
    
    if (movetype==CAN_MOV_SCROLL)
    {
      can->wup = gui_NewWidget();
      gui_WidgetInit(can->wup,gui_GetMain());
      gui_WidgetMap(can->wup);
      gui_WidgetSetColor(can->wup,"trans"); // "trans");
      gui_WidgetSetRound(can->wup,2);
      if (img_aup) gui_WidgetAddImage(can->wup,img_aup);
      
      struct gui_GlPoly *glpolyUp;
      glpolyUp = gui_GlPolyInitFromFile(gl,NULL,"arrowUp");
      if (glpolyUp)
      {
        glpolyUp->trans.width 	= DEFA_CSMARRL-30;
        glpolyUp->trans.height 	= DEFA_CSMARRL-30;
        gui_GlPolyTransformProtoEdges(glpolyUp);
        gui_GlColorSet(gl,&glpolyUp->color,0xcc,0xcc,0xfa,0xba);
        gui_WidgetAddGlPoly(can->wup,glpolyUp);
      }
      
      can->wup->attr.onclick = _can_wup_press;
      can->wup->attr.form = FORM_BUTTON;
      can->wup->attr.x = (w->attr.w-DEFA_CSMARRL)+20;
      can->wup->attr.y = (w->attr.h/2)-(DEFA_CSMARRL+10);
      can->wup->attr.w = DEFA_CSMARRL-30;
      can->wup->attr.h = DEFA_CSMARRL-30;
      if (img_aup) gui_ImagePosCenter(img_aup,can->wup);
      form_CanvasAddObjectStatic(w,can->wup);
      
      can->wdown = gui_NewWidget();
      gui_WidgetInit(can->wdown,gui_GetMain());
      gui_WidgetMap(can->wdown);
      gui_WidgetSetColor(can->wdown,"trans"); // "trans");
      gui_WidgetSetRound(can->wdown,2);
      if (img_adown) gui_WidgetAddImage(can->wdown,img_adown);
      
      struct gui_GlPoly *glpolyDown;
      glpolyDown = gui_GlPolyInitFromFile(gl,NULL,"arrowDown");
      if (glpolyDown)
      {
        glpolyDown->trans.width 	= DEFA_CSMARRL-30;
        glpolyDown->trans.height 	= DEFA_CSMARRL-30;
        gui_GlPolyTransformProtoEdges(glpolyDown);
        gui_GlColorSet(gl,&glpolyDown->color,0xcc,0xcc,0xfa,0xba);
        gui_WidgetAddGlPoly(can->wdown,glpolyDown);
      }
      
      can->wdown->attr.onclick = _can_wdown_press;
      can->wdown->attr.form = FORM_BUTTON;
      can->wdown->attr.x = (w->attr.w-DEFA_CSMARRL)+20;
      can->wdown->attr.y = (w->attr.h/2)+10;
      can->wdown->attr.w = DEFA_CSMARRL-30;
      can->wdown->attr.h = DEFA_CSMARRL-30;
      if (img_adown) gui_ImagePosCenter(img_adown,can->wdown);
      form_CanvasAddObjectStatic(w,can->wdown);
    }
    return 0;
  }

  /** listbox entry, usable in general canvas
   */
  int form_LBEntryInit( struct gui_Widget *w )
  {
    static struct form_ListboxEntry *lbe;
    if (!w) return -1;
    w->form = (struct form_ListboxEntry*)malloc(sizeof(struct form_ListboxEntry));
    lbe = (struct form_ListboxEntry*)w->form;
    lbe->str_big 	= NULL;
    lbe->str_small 	= NULL;
    lbe->image		= NULL;
    w->attr.form 	= FORM_LBENTRY;
    gui_WidgetSetColor(w,DEFA_LBEBG);
    gui_WidgetSetRound(w,DEFA_ROUND);
    return 0;
  }
  
  /** we can set the short and long desc separately, using NULL on which is not set
   */
  int form_LBEntryConfigure( struct gui_Widget *w, char *strbig, char *strsmall )
  {
    static struct form_ListboxEntry *lbe = NULL;
    if (!w) return -1;
    lbe = (struct form_ListboxEntry*)w->form;
    if (strbig)
    {
      lbe->str_big = (char*)malloc(strlen(strbig)+1);
      strcpy(lbe->str_big,strbig);
    }
    if (strsmall)
    {
      lbe->str_small = (char*)malloc(strlen(strsmall)+1);
      strcpy(lbe->str_small,strsmall);
    }
    return 0;
  }
  
  int form_LBEntrySetImage( struct gui_Widget *w, struct gui_Image *image )
  {
    static struct form_ListboxEntry *lbe = NULL;
    if (!w||!image) return -1;
    gui_ImageSetRound(image,DEFA_ROUND);
    lbe = (struct form_ListboxEntry*)w->form;
    lbe->image = image;
    return 0;
  }

  int form_LBEntryRelease( struct gui_Widget *w )
  {
    static struct form_ListboxEntry *lbe = NULL;
    if (!w) return -1;
    lbe = (struct form_ListboxEntry*)w->form;
    if (!lbe) return -1;
    
    if (lbe->str_big)
    {
      free(lbe->str_big);
      lbe->str_big = NULL;
    }
    if (lbe->str_small)
    {
      free(lbe->str_small);
      lbe->str_small = NULL;
    }
    
    lbe->image = NULL;
    
    free(lbe);
    w->form = NULL;
    
    return 0;
  }

  int form_CanvasAddObjByRss( struct gui_Widget *w, struct trop_Rss *rss )
  {
    static struct form_Canvas	*canvas;
    static struct trop_List	*rcurrent;
    static struct trop_RssItem	*rssitem;

    static struct gui_Widget	*wtext;
    static struct gui_Widget	*wimage;
    
    if (!w) return -1;
    if (!w->form) return -1;
    
    canvas = (struct form_Canvas*)w->form;
   
    rcurrent = rss->items;
    while(rcurrent)
    {
      rssitem = (struct trop_RssItem*)rcurrent->ptr;
      
      wtext = gui_NewWidget();
      wtext->attr.x = 0;
      wtext->attr.y = 0;
      wtext->attr.w = w->attr.w*0.7;
      wtext->attr.h = 40;
      wtext->attr.form = FORM_TEXT;
      gui_WidgetInit(wtext,w);
      gui_WidgetSetColor(wtext,"#ffffff");
      gui_WidgetSetRound(wtext,2);
      gui_WidgetSetDynamic(wtext);
      gui_WidgetMap(wtext);
      form_TextInit(wtext);
      //form_TextAddText(wtext,(char*)rssitem->desc);
      form_TextAddHtmlText(wtext,(char*)rssitem->desc);
      form_CanvasAddObject(w,wtext);
      
      rcurrent = rcurrent->next;
      break;
    }
    
    return 0;
  }

#endif
