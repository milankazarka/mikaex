#include "system.h"
#include "gui.h"
#include "global.h"
#include "config.h"

void *form_CustomExpose( void *obj, int x, int y, int w, int h )
{
  if (!obj)
  {
    #ifdef _DEBUG
      printf("form_CustomExpose error invalid arguments\n");
    #endif
    return NULL;
  }
  
  struct gui_Widget *wi = (struct gui_Widget*)obj;
  
  
  return NULL;
}

int form_CustomInit( struct gui_Widget *w )
{
  if (!w)
  {
    #ifdef _DEBUG
      printf("form_CustomInit error invalid arguments\n");
    #endif
    return 1;
  }
 
  w->attr.onexposeEx = form_CustomExpose;
  struct form_Custom *fcustom = (struct form_Custom*)malloc(sizeof(struct form_Custom));
  w->form = fcustom;
  
  return 0;
}
