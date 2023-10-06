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

struct form_Frame *form_FrameInit(
  struct form_Frame 	*frame,
  int			x,
  int			y,
  int			w,
  int			h
)
{
  if (!frame)
  {
    frame = (struct form_Frame*)malloc(sizeof(struct form_Frame));
    if (!frame)
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE)
          printf("form_FrameInit error - could not allocate frame's surface");
      #endif
    }
  }
  
  frame->surface = gui_SurfaceInit( gui_GetMain(), NULL, w, h );
  
  return frame;
}

int form_FrameDelete(
  struct form_Frame *frame
)
{
  if (!frame)
    return -1;
    
  free(frame->surface);
  free(frame);
    
  return 0;
}

int form_FrameExpose(
  struct gui_Widget *w
)
{
  if (!w) return -1;
  
  #ifdef _DEBUG
    if (DEBUG==TRUE) printf(":form_FrameExpose\n");
  #endif
  
  gui_WidgetExpose(
    w,
    w->attr.x,
    w->attr.y,
    w->attr.w,
    w->attr.h
  );
  
  return 0;
}
