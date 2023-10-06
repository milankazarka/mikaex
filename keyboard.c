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
#include "global.h"

#ifdef USEX
  #define _KBD_KEY_MARGIN 3

  int form_KeyboardInit( struct gui_Widget *w )
  {
    static int			strw;
    static int			strh;
    static int			side;
    static struct gui_TextSim	*ts = NULL;
    
    if (!w) return -1;
    
    side = gui_GetMain()->attr.ftsize*2;
    
    /** todo - create a full keyboard
     */
    ts = (struct gui_TextSim*)malloc(sizeof(struct gui_TextSim));
    ts->str = NULL;
    gui_WidgetAddTextSimpleD(w,ts,"Q",PLACE_FILL);
    ts->pos.x = _KBD_KEY_MARGIN;
    ts->pos.y = _KBD_KEY_MARGIN;
    ts->pos.w = side;
    ts->pos.h = side;
    ts = (struct gui_TextSim*)malloc(sizeof(struct gui_TextSim));
    ts->str = NULL;
    gui_WidgetAddTextSimpleD(w,ts,"W",PLACE_FILL);
    ts->pos.x = _KBD_KEY_MARGIN*12;
    ts->pos.y = _KBD_KEY_MARGIN;
    ts->pos.w = side;
    ts->pos.h = side;
    
    return 0;
  }
  
  int form_KeyboardExpose( 
    struct gui_Widget *wi,
    int w, int h,
    int x, int y
    )
  {
  
    if (!wi) return -1;
    
    gui_WidgetExpose(wi,w,h,x,y);
    
    return 0;
  }
  
  int form_KeyboardButtonPressEx(
    struct gui_Event *ev
    )
  {
    return 0;
  }
  
  int form_KeyboardButtonPress(
    struct gui_Event *ev
    )
  {
    static struct gui_object	*gobj	= NULL;
    static struct gui_TextSim	*ts	= NULL;
    static struct gui_Str	*gstr	= NULL;
    static char			*str	= NULL;
    static char			key	= 0x00;
    struct gui_Event		rev; // relay event (todo, add relaying upstream)
    
    if (!ev) return -1;
    if (!ev->widget) return -1;
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("form_KeyboardButtonPress\n");
    #endif
    
    gobj = ev->widget->obj;
    while(gobj!=NULL)
    {
      if (
        gobj->id==GOID_TEXTSIMD ||
        gobj->id==GOID_TEXTSIMC ||
        gobj->id==GOID_TEXTSIMB ||
        gobj->id==GOID_TEXTSIM
        )
      {
        ts = (struct gui_TextSim*)gobj->data;
        #ifdef _DEBUG
          if (DEBUG==TRUE)
          {
             printf("form_KeyboardButtonPress obj(%d,%d,%d,%d) ev(%d,%d)\n",
               ts->pos.x, ts->pos.y,
               ts->pos.w, ts->pos.h,
               ev->x, ev->y
               ); 
          }
        #endif
        if (
          ts!=NULL &&
          (ts->pos.x < ev->x && ts->pos.x+ts->pos.w > ev->x &&
          ts->pos.y < ev->y && ts->pos.y+ts->pos.h > ev->y )
          )
        {
          str = ts->str;
          if (str)
          {
            if (strlen(str)>0)
            {
              key = str[0];
              rev.type = GEVENT_KEY;
              rev.widget = ev->widget;
              rev.key = key;
              gui_WidgetDoEvent(rev.widget,&rev);
            } 
          }
        }
      }
      gobj = gobj->next;
    }
    
    return 0;
  }
#endif
