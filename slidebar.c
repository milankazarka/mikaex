#include "slidebar.h"
#include "system.h"
#include "gui.h"
#include "xml.h"
#include "global.h"
#include "config.h"

void *form_SlidebarExExpose( void *obj, int x, int y, int w, int h )
{
  if (!obj)
  {
    #ifdef _DEBUG
      printf("form_SlidebarExExpose error\n");
    #endif
    return NULL;
  }
  
  #ifdef _DEBUG
    printf("form_SlidebarExExpose\n");
  #endif
  struct gui_Widget *wi = (struct gui_Widget*)obj;
  struct form_SlidebarEx *slidebar = (struct form_SlidebarEx*)wi->form;
  
  int total = wi->attr.h;
  float q = ((float)slidebar->size/(float)slidebar->total);
  int size = (int)((float)total*q);
  
  int position = (int)((float)slidebar->position*q);
  
  #ifdef _DEBUG
    printf("form_SlidebarExExpose position(%d) size(%d)\n",
      position,size);
  #endif
  
  gui_DrawRoundRectangle(
    NULL,
    wi->attr.x,wi->attr.y+position,
    wi->attr.w,size,
    1
  );
  
  return NULL;
}

int form_SlidebarExInit( struct gui_Widget *w )
{
  if (!w)
  {
    #ifdef _DEBUG
      printf("form_SlidebarInit error gui_Widget nil\n");
    #endif
    return 1;
  }
  
  #ifdef _DEBUG
    printf("form_SlidebarInit\n");
  #endif
  
  w->attr.onexposeEx = form_SlidebarExExpose;
  struct form_SlidebarEx *slidebarEx = (struct form_SlidebarEx*)malloc(sizeof(struct form_SlidebarEx));
  if (slidebarEx)
  {
    slidebarEx->total 		= 10;
    slidebarEx->position 	= 0;
    slidebarEx->size 		= 10;
    w->form		 	= slidebarEx;
  }
  else
  {
    #ifdef _DEBUG
      printf("form_SlidebarInit error couldn't create form_SlidebarEx\n");
    #endif
    return 1;
  }
  
  return 0;
}

/** attach the slidebar to a canvas
 */
int form_SlidebarExAttachCanvas( struct gui_Widget *slidebar, struct gui_Widget *canvas )
{
  if (!slidebar || !canvas)
  {
    #ifdef _DEBUG
      printf("form_SlidebarAattachCanvas error invalid argument\n");
    #endif
    return 1;
  }
  return 0;
}
