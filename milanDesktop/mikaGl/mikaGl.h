#ifndef _MIKA_GL_H_
#define _MIKA_GL_H_

#include "agg.h"
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include "mikaGlImage.h"

class mikaGl {
  public:
    struct gui_Gl *gl;    
    struct gui_GlColor *glColor;
    struct gui_GlColor *glFontColor;
    int fontSize;
    cairo_t *cairo;
    cairo_surface_t *image;
    unsigned char *pixels;
    int mw, mh;
  
    mikaGl( void *surface, int w, int h );
    ~mikaGl( );
    
    int setColor( unsigned char r, unsigned char g, unsigned char b, unsigned char a );
    int drawRoundRect( int x1, int y1, int x2, int y2, int round );
    int drawText( int x1, int y1, unsigned char *text );
    int drawImage( int x1, int y1, mikaGlImage *glImage );
};

#endif
