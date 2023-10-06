#include "mikaPBO.h"
#include "config.h"
#define _PBO_CLEAR_VALUE 0xff
#define _PBO_CLEAR_ENABLE

void mikaPBO::clear( )
{
  #ifndef _PBO_CLEAR_ENABLE
    return;
  #endif
  
  #ifdef _DEBUG
    printf("mikaPBO::clear clearChar(%2X)\n",clearChar);
  #endif
  int nx;
  int ny;
  memset((void*)pixels,clearChar,frame.w*frame.h*4);
  for(ny = 0; ny < frame.h; ny++)
  {
    for(nx = 0; nx < frame.w; nx++)
    {
      memset((unsigned char*)pixels+(ny*(frame.w*4))+(nx*4)+3,clearChar,1);
    }
  }
}

mikaPBO::mikaPBO( struct mikaFrame *_frame, void *_pixels )
{
}

mikaPBO::mikaPBO( struct mikaFrame *_frame )
{
  event.timestamp = 0;
  event.type = _MIKA_PBO_EVENT_NONE;
  frame = *_frame;
  clearChar = _PBO_CLEAR_VALUE;
  
  delegate.onUpdate = NULL;
  
  pixels = malloc(frame.w*frame.h*4);
  clear();
  cairo_surface = cairo_image_surface_create_for_data(
    (unsigned char*)pixels, CAIRO_FORMAT_ARGB32,
    frame.w, frame.h,
    4*frame.w
    );
}

mikaPBO::mikaPBO( )
{
}

mikaPBO::~mikaPBO( )
{
}

void mikaPBO::update( void *_pixels )
{
  memcpy(pixels,_pixels,frame.w*frame.h*4);
  if (delegate.onUpdate)
    delegate.onUpdate((void*)this);
}
