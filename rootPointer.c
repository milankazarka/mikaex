#include "rootPointer.h"
#include "config.h"
#include <math.h>

static int active = 0;

int rootPointerDrawESYNC( struct rootDesktop *rd, cairo_t *cairo, int x, int y )
{
  #ifdef _DEBUG
    printf("rootPointerDraw\n");
  #endif
  
  if (!rd || !cairo)
  {
    #ifdef _DEBUG
      printf("rootPointerDraw ERROR invalid arguments\n");
    #endif
    return 1;
  }
  
  if (active==0)
    cairo_set_source_rgba(cairo, 0.69, 0.19, 0, 0.5);
  else
    cairo_set_source_rgba(cairo, 145.0/255.0, 243.0/255.0, 97.0/255.0, 0.5);
    
  cairo_arc(cairo, x, y, 
      20, 0, 2 * M_PI);
  cairo_stroke_preserve(cairo);       
  cairo_fill(cairo);
  
  return 0;
}

int rootPointerSetActive( struct rootDesktop *rd )
{
  #ifdef _DEBUG
    printf("rootPointerSetActive\n");
  #endif
  
  if (!rd)
  {
    #ifdef _DEBUG
      printf("rootPointerSetActive ERROR invalid arguments\n");
    #endif
    return 1;
  }
  
  active = 1;
  
  return 0;
}

int rootPointerSetInactive( struct rootDesktop *rd )
{
  #ifdef _DEBUG
    printf("rootPointerSetInactive\n");
  #endif
  
  if (!rd)
  {
    #ifdef _DEBUG
      printf("rootPointerSetInactive ERROR invalid arguments\n");
    #endif
    return 1;
  }
  
  active = 0;
  
  return 0;
}
