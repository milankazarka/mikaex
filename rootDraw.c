/**
 * rootDraw is used to draw some primitives on the cairo surface
 *
 */
#include "rootController.h"
#include "rootDraw.h"
#include "config.h"
#include <math.h>

extern "C" int rootDrawDrawControlCircle( cairo_t *cairo, struct rootFrame *frame )
{
  if (!cairo || !frame)
  {
    return 1;
  }
  
  cairo_set_source_rgba(cairo,0.9,0.9,0.9,0.85);
  
  cairo_arc(
    cairo, 
    frame->x,frame->y,
    frame->w,0.2,2*M_PI
    );
  cairo_stroke_preserve(cairo);
  cairo_fill(cairo);
  
  return 0;
}

extern "C" int rootDrawDrawWindowBorder( cairo_t *cairo, struct rootFrame *frame )
{
  if (!cairo || !frame)
  {
    return 1;
  }
  
  cairo_new_path (cairo);
  cairo_set_source_rgb (cairo, 0, 1, 0);
  cairo_move_to (cairo, 0, 0);
  cairo_line_to (cairo, 256, 256);
  cairo_move_to (cairo, 256, 0);
  cairo_line_to (cairo, 0, 256);
  cairo_set_line_width (cairo, 10.0);
  cairo_stroke (cairo);
  cairo_fill(cairo);
  
  return 0;
}

extern "C" int rootDrawDrawControlCircleGC( cairo_t *cairo, struct rootGc *gc, struct rootFrame *frame )
{
  if (!cairo || !gc || !frame)
  {
    return 1;
  }
  return 0;
}

/** draw round rectangle
 */
extern "C" int rootDrawDrawRoundRect( cairo_t *cairo, struct rootFrame *frame, int round )
{
  if (!cairo || !frame)
  {
    #ifdef _DEBUG
      printf("rootDrawDrawRoundRect error invalid arguments\n");
    #endif
    return 1;
  }
  
  cairo_new_path (cairo);
  
  cairo_move_to (cairo, frame->x+round, frame->y);
  cairo_line_to (cairo, frame->x+frame->w-round, frame->y);
  cairo_curve_to(cairo, 
              frame->x+frame->w,frame->y,frame->x+frame->w,
              frame->y,frame->x+frame->w,frame->y+round);
  cairo_line_to(cairo,frame->x+frame->w,frame->y+frame->h-round);
  cairo_curve_to(cairo,
              frame->x+frame->w,frame->y+frame->h,frame->x+frame->w,
              frame->y+frame->h,frame->x+frame->w-round,frame->y+frame->h);
  cairo_line_to(cairo,frame->x+round,frame->y+frame->h);
  cairo_curve_to(cairo,
              frame->x,frame->y+frame->h,frame->x,
              frame->y+frame->h,frame->x,frame->y+frame->h-round);
  cairo_line_to(cairo,frame->x,frame->y+round);
  cairo_curve_to(cairo,
              frame->x,frame->y,frame->x,
              frame->y,frame->x+round,frame->y
              );
  
  cairo_stroke (cairo);
  
  return 0;
}

/** draw a glow round rectangle effect
 */
extern "C" int rootDrawDrawRoundRectGlow( cairo_t *cairo, struct rootFrame *frame, int round )
{
  struct rootFrame subFrame;
  static int n;
  for(n = 0; n < _BORDER; n++)
  {
    cairo_set_source_rgba(cairo,1.0,1.0,1.0,0.0+(0.002*n));
    subFrame.x = ((frame->x-_ROOT_OBJECT_FRAME)-_BORDER)+n;
    subFrame.y = ((frame->y-_ROOT_OBJECT_FRAME)-_BORDER)+n;
    subFrame.w = (frame->w+(_ROOT_OBJECT_FRAME*2))-(n*2)+(_BORDER*2);
    subFrame.h = (frame->h+(_ROOT_OBJECT_FRAME*2))-(n*2)+(_BORDER*2);
    rootDrawDrawRoundRect(cairo,&subFrame,round);
  }
  
  return 0;
}
