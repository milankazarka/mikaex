#include "mikaGl.h"
#include "mikaGlConfig.h"

mikaGl::mikaGl( void *surface, int w, int h )
{
  #ifdef _GLDEBUG
    printf("mikaGl::mikaGl (%dx%d)\n",w,h);
  #endif
  gl = gui_GlInit(NULL,surface,w,h);
  if (!gl)
  {
    #ifdef _GLDEBUG
      printf("mikaGl::mikaGl error agg gl couldn't be initialized\n");
    #endif
  }
  glColor = (struct gui_GlColor*)malloc(sizeof(struct gui_GlColor));
  glFontColor = (struct gui_GlColor*)malloc(sizeof(struct gui_GlColor));
  glFontColor->rgba[0] = 225;
  glFontColor->rgba[1] = 225;
  glFontColor->rgba[2] = 225;
  glFontColor->rgba[3] = 255;
  fontSize = 24;
  
  mw = w;
  mh = h;
  pixels = (unsigned char*)surface;
  image = cairo_image_surface_create_for_data(
    pixels,
    CAIRO_FORMAT_ARGB32,
    mw, mh,
    4*mw
  );
  cairo = cairo_create(image);
  if (!cairo)
  {
    #ifdef _GLDEBUG
      printf("mikaGl::mikaGl error, couldn't create cairo\n");
    #endif
  }
  cairo_select_font_face(cairo, "Arial",
    CAIRO_FONT_SLANT_NORMAL,
    CAIRO_FONT_WEIGHT_NORMAL);
}

mikaGl::~mikaGl( )
{
}

int mikaGl::setColor( unsigned char r, unsigned char g, unsigned char b, unsigned char a )
{
  #ifdef _DEBUG
    printf("mikaGl::setColor\n");
  #endif
  gui_GlColorSet(gl,glColor,r,g,b,a);
  gui_GlSetColor(gl,glColor);
  return 0;
}

int mikaGl::drawRoundRect( int x1, int y1, int x2, int y2, int round )
{
  #ifdef _GLDEBUG
    printf("mikaGl::drawRoundRect (%dx%d+%d+%d)\n",
      x2-x1,y2-y1,x1,y1);
  #endif
  gui_GlDrawRoundRect( gl,x1,y1,x2,y2,round );
  return 0;
}

int mikaGl::drawText( int x1, int y1, unsigned char *text )
{
  if (!text)
  {
    #ifdef _GLDEBUG
      printf("mikaGl::drawText error, invalid arguments\n");
    #endif
    return 1;
  }
  
  cairo_set_source_rgb(cairo,
    (double)((double)glFontColor->rgba[0]/(double)0xff),
    (double)((double)glFontColor->rgba[1]/(double)0xff),
    (double)((double)glFontColor->rgba[2]/(double)0xff)
  );
  cairo_set_font_size(cairo,fontSize);
  cairo_move_to(cairo,x1,y1);
  cairo_show_text(cairo,(char*)text);
  
  return 0;
}

int mikaGl::drawImage( int x1, int y1, mikaGlImage *glImage )
{
  if (!glImage)
  {
    #ifdef _GLDEBUG
      printf("mikaGl::drawImage error, invalid arguments\n");
    #endif
    return 1;
  }
  
  #ifdef _GLDEBUG
    printf("mikaGl::drawImage (%dx%d+%d+%d)\n",
      glImage->mw,glImage->mh,x1,y1);
  #endif
  
  cairo_surface_t *tmp = cairo_image_surface_create_for_data(
    glImage->pixels,
    CAIRO_FORMAT_ARGB32,
    glImage->mw, glImage->mh,
    4*glImage->mw
  );
  
  cairo_save(cairo);
  cairo_translate(cairo,(double)x1,(double)y1);
  cairo_set_source_surface(cairo,tmp,0,0);
  cairo_paint(cairo);
  cairo_surface_destroy(tmp);
  cairo_surface_flush(tmp);
  cairo_surface_finish(tmp);
  cairo_restore(cairo);
  
  return 0;
}
