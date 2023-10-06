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

#ifdef USEX

  #define _EDITABLE_TEXT_MARGIN	4

  int form_EditableInit( struct gui_Widget *w )
  {
    static struct form_Editable *editable;
    
    if (!w) return -1;
    
    w->form = (struct form_Editable*)malloc(sizeof(struct form_Editable));
    editable = w->form;
    editable->value[0] = 0x00;
    editable->vlen = 0;
    
    return 0;
  }

  int form_EditableExpose(
    struct gui_Widget *wi,
    int x, int y,
    int w, int h
    )
  {
    static struct form_Editable *editable;
    static int 		strx;
    static int 		stry;
    static int 		strw;
    static int 		strh;
    
    if (!w) return -1;
    
    editable = wi->form;
    
    gui_WidgetExpose(wi,x,y,w,h);
    
    if (editable->vlen > 0)
    {
      gui_TextGetSize((char*)editable->value,&strw,&strh);
      if (strh < gui_GetMain()->attr.ftsize*2) strh = gui_GetMain()->attr.ftsize*2;
      if (strh > wi->attr.h) strh = wi->attr.h;
    
      gui_DrawTextEx(
        (char*)editable->value,
        x+_EDITABLE_TEXT_MARGIN,
        y-_EDITABLE_TEXT_MARGIN,
        strw,strh,DEFA_TXTR
      );
    }
    
    return 0;
  }
  
  int form_EditableRemoveChar( 
    struct gui_Widget *w 
  )
  {
    static struct form_Editable *editable;
    
    if (!w) return -1;
    
    editable = w->form;
    if (editable->vlen==0) return 0;
    
    editable->vlen--;
    editable->value[editable->vlen] = 0x00;
    
    gui_TriggerExpose(gui_GetMain());
    
    return 0;
  }
  
  int form_EditableAppendChar(  
    struct gui_Widget *w,
    char c
  )
  {
    static struct form_Editable *editable;
    
    if (!w) return -1;
    
    editable = w->form;
    editable->value[editable->vlen] = c;
    editable->vlen++;
    editable->value[editable->vlen] = 0x00;
    
    gui_TriggerExpose(gui_GetMain());
    
    return 0;
  }
  
  int form_EditableSetListener(
    struct gui_Widget *wi,
    struct gui_Widget *rwi
  )
  {
    return(gui_WidgetAddRelayEx(wi,rwi));
  }
  
#endif
