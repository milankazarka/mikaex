/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "config.h"
#include "system.h"
#include "gui.h"
#include "xml.h"
#include "interface.h"
#include "global.h"
#include <cairo/cairo.h>

#ifdef JPEG
  #include "jpeglib.h"
  #include "jpegint.h"
  #include "jerror.h"
  #include "jconfig.h"
#endif

#define _USE_GL_TEXT
#define _USE_GL_RENDERER
#define _PIXEL_SECURITY

struct gui_Widget *wCurrent = NULL;

  /**
   *  get a pixel on a speciffic position
  */
  inline int __get_pixel(
                         void *buffer,
                         int bw, int bh,
                         int x, int y,
                         int bpp,
                         char *storage )
  {
    if (!buffer||!storage) return -1;
    memcpy(storage,buffer+((bw*bpp*y)+(x*bpp)),bpp);
    return 0;
  }

  /**
   *  put a pixel from pstorage into a certain possition
   *  in buffer
   */
  inline int __put_pixel(
                         void *buffer,
                         int bw, int bh,
                         int x, int y,
                         int bpp,
                         char *pstorage )
  {
  
    #ifdef _PIXEL_SECURITY
      if (
        x > (gui_bounds.x+gui_bounds.width)-1 ||
        y > (gui_bounds.y+gui_bounds.height)-1 ||
        x < gui_bounds.x ||
        y < gui_bounds.y 
      )
      {
        return 0;
      }
    #endif
  
    if (!buffer||!pstorage) return -1;
    memcpy(buffer+((bw*bpp*y)+(x*bpp)),pstorage,bpp);
    return 0;
  }

  inline int __x_draw_pixel( struct gui_Widget *main, struct gui_GC *gc, int x, int y )
  {
    #ifdef _USE_GUI_XLIB_INTERNALS
      XDrawPoint(main->dpy,main->id,gc->gc,x,y);
    #endif
    return 0;
  }

  inline int __x_flush( )
  {
    #ifdef _USE_GUI_XLIB_INTERNALS
      XFlush(gui_GetMain()->dpy);
    #else
      bigInterface->guiFlush(bigInterface);
    #endif
    return 0;
  }

  inline int gui_Flush( )
  {
    return __x_flush();
  }

  /*
    get free space in window - usable if we already have side widgets (titlebar, slidebar) 
  */
  int gui_GeomGetFreeSpace( int w, int h, int *x, int *y, int *x2, int *y2 ) // todo
  {
    int 
      fx = 0, 
      fy = 0, 
      fyh = gui_GetMain()->attr.h,
      fxh = gui_GetMain()->attr.w;
    struct gui_Widget *current = gui_GetMain();
    while(current!=NULL)
    {
      if (current!=gui_GetMain() && current->parent == gui_GetMain())
      {
        if (current->attr.x+current->attr.w <= gui_GetMain()->attr.w/2
          && fx < current->attr.x+current->attr.w)
          fx = current->attr.x+current->attr.w;
        else if (current->attr.x > gui_GetMain()->attr.w/2
          && fxh > current->attr.x)
          fxh = current->attr.x;
        if (current->attr.y+current->attr.h <= gui_GetMain()->attr.h/2
          && fy < current->attr.y+current->attr.h)
          fy = current->attr.y+current->attr.h;
        else if (current->attr.y > gui_GetMain()->attr.h/2
          && fyh > current->attr.y)
          fyh = current->attr.y;
      }
      current = current->next;
    }
    *x = fx;
    *y = fy;
    *x2 = fxh;
    *y2 = fyh;
    return 0;
  }

  inline int gui_DrawPixel( struct gui_Widget *main, struct gui_GC *gc, int x, int y )
  {
    #ifdef _USE_GUI_XLIB_INTERNALS
      return __x_draw_pixel( main, gc, x, y );
    #endif
    return 0;
  }

  int _gui_draw_rectangle( struct gui_Surface *surface, struct gui_GC *gc, int x, int y, int w, int h )
  {
    #ifdef _USE_GUI_XLIB_INTERNALS
      static XImage *xsurface = NULL;
      static int xn, yn;
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("_gui_draw_rectangle\n");
      #endif
      
      if (surface==NULL)
        xsurface = gui_GetMain()->surface;
      else
        xsurface = surface->image->ximage;
      
      if (x<0||y<0||w<1||h<1) return -1;
      if (gui_GetMain()->attr.w<=x||gui_GetMain()->attr.h<=y) return -1;
      if (gui_GetMain()->attr.w<=x+w) w = gui_GetMain()->attr.w-x;
      if (gui_GetMain()->attr.h<=y+h) h = gui_GetMain()->attr.h-y;
      
      for (yn = y; yn < y+h; yn++)
      {
        for (xn = x; xn < x+w; xn++)
        {
          if (gc->colorl != NULL)
            XPutPixel(xsurface,xn,yn,gc->colorl->value);
        }
      }
      
      if (gc->colorl != NULL)
        rec_ExposeAddRect(rec_expose,x,y,w,h,gc->colorl->value);
      
    #else
      gui_DrawRoundRectangle(gc,x,y,w,h,0);
    #endif
      
    return 0;
  }

  int gui_DrawRectangle( struct gui_GC *gc, int x, int y, int w, int h )
  {
    return _gui_draw_rectangle(NULL,gc,x,y,w,h);
  }

  int gui_DrawRectangleEx( struct gui_Surface *surface, struct gui_GC *gc, int x, int y, int w, int h )
  {
    if (!surface) return -1;
    static struct gui_GC *neutral = NULL;
    if (neutral==NULL && gc==NULL)
    {
      neutral = (struct gui_GC*)malloc(sizeof(struct gui_GC));
      if ( gui_InitGCEx((struct gui_GC*)neutral,(char*)"#ffffff") != 0 ) return -1;
    }
    if (gc==NULL&&neutral!=NULL) gc=neutral;
    return _gui_draw_rectangle(surface,gc,x,y,w,h);
  }

  int _CGC_move;  
  inline void _color_get_components( 
    long color, 
    unsigned char *r, 
    unsigned char *g, 
    unsigned char *b 
    )
  {
    ldiv_t d;
    
    d = ldiv(color,256);	color = d.quot; *r = d.rem;
    d = ldiv(color,256);	color = d.quot; *g = d.rem;
    d = ldiv(color,256);	color = d.quot; *b = d.rem;
  }

  inline long _color_move_components(
    unsigned char r,
    unsigned char g,
    unsigned char b,
    short move
  )
  {
    static long c;
    
    if (move<0)
    {
      if (r+move<0)	move = -r;
      if (g+move<0)	move = -g;
      if (b+move<0)	move = -b;
    }
    else if (move>0)
    {
      if (r+move>255)	move = 255-r;
      if (g+move>255)	move = 255-g;
      if (b+move>255)	move = 255-b;
    }
    
    r+=move;
    g+=move;
    b+=move;
    
    c = r+(g*0xff)+(b*0xff*0xff);
    
    return c;
  }

  /*
    todo - combine a lower and upper color 
  */
  inline long _color_combine( 
    long pixel00,
    long pixel01
  )
  {
    static long l;
    static char	r00, g00, b00;
    static char	r01, g01, b01;
    static char seed;
    _color_get_components(pixel00,&r00,&g00,&b00);
    _color_get_components(pixel01,&r01,&g01,&b01);
    
    l = r00+(g00*0xff)+(b00*0xff*0xff);
    return l;
  }

  struct gui_Draw *gui_DrawInit( struct gui_Draw *draw )
  {
    if (draw==NULL)
    {
      draw = (struct gui_Draw*)malloc(sizeof(struct gui_Draw));
    }
    draw->x 		= 0;
    draw->y 		= 0;
    draw->w 		= 0;
    draw->h 		= 0;
    draw->round		= 0;
    draw->gradient	= 0;
    draw->gc 		= NULL;
    draw->surface 	= NULL;
    return draw;
  }

  #define _RECTANGLE_BORDERNO

  /*
    if we pass a gc=NULL, than we are drawing a transparent rectangle 
    grad = 0, no gradient
  */
  int _gui_draw_round_rectangle( 
    struct gui_GC *gc, 
    struct gui_Surface *surface, 
    int x, int y, 
    int w, int h, 
    int val, int grad 
  )
  {
    static int 			xn, yn;
    static long 		color;
    static long 		border;
    static long			orig;
    static long			pixel;
    static long 		lhelper;
    static long			last;
    static long			last_moved;
    
    static BOOL 		btop;
    static BOOL 		bbottom;
    static BOOL 		bleft;
    static BOOL 		bright;
    static BOOL			btrans;
    
    static struct gui_GC 	*neutral = NULL;
    
    static int			mainw = -1;
    static int			mainh = -1;
    
    static unsigned char	ns;
    static unsigned char 	r, g, b;
    ldiv_t 			d;
    
    if ( w<1||h<1 ) return -1;
    #ifdef _DEBUG
      if ( DEBUG==TRUE) printf("gui_DrawRoundRectangle\n");
    #endif
    if ( gui_GetMain()->attr.w <= x || gui_GetMain()->attr.h<=y ) return -1;
    if ( gui_GetMain()->attr.w <= x+w ) w = gui_GetMain()->attr.w-x;
    if ( gui_GetMain()->attr.h <= y+h ) h = gui_GetMain()->attr.h-y;
    
    if (mainw == -1 || mainh == -1)
    {
      mainw = gui_GetMain()->attr.w;
      mainh = gui_GetMain()->attr.h;
    }
    
    orig		= -1;
    last 		= -1;
    last_moved 		= -1;
    
    r = g = b		= 0;
    
    if (gc==NULL)
    {
      if (neutral==NULL)
      {
        neutral = (struct gui_GC*)malloc(sizeof(struct gui_GC));
        gui_InitGCEx((struct gui_GC*)neutral,"trans");
      }
      gc = neutral;
    }
    
    btrans = FALSE;
    if (gc->colorl!=NULL)
    {
      if (gc->colorl->value==C_TRANS_VALG)
        btrans = TRUE;
    }
    
    btop 	= TRUE;
    bbottom 	= FALSE;
    
    if (btrans==FALSE)
    {
      if (gc->colorl != NULL)	color = gc->colorl->value;
      else
      {
        #ifdef _USE_GUI_XLIB_INTERNALS			
          color = gc->color->pixel;
        #else
          #ifdef _DEBUG
            if (DEBUG==TRUE) printf("color format unsupported\n");
          #endif
          return -1;
        #endif
      }
    
      #ifdef _RECTANGLE_BORDER
        d = ldiv(color,256);	lhelper = d.quot; r = d.rem;
        d = ldiv(lhelper,256);	lhelper = d.quot; g = d.rem;
        d = ldiv(lhelper,256);	lhelper = d.quot; b = d.rem;
    
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("colors c(%d) rgb(%d,%d,%d,%d) 01\n",color,r,g,b,lhelper);
        #endif
        if (r-80 > -1) r-=80; else r = 0;
        if (g-80 > -1) g-=80; else g = 0;
        if (b-80 > -1) b-=80; else b = 0;
      
        border = r+(g*256)+(b*256*256);
      #endif
    }
    
    orig 	= color;
    ns 		= 0;
    
    for (yn = y; yn < y+h; yn++)
    {
      if (yn!=y) 	btop = FALSE;
      if (yn==y+h-1) 	bbottom = TRUE;
      bright = FALSE;
      
      if (
        btrans==FALSE && 
        grad!=0 && 
        color != orig+grad
      )
      {
        if (ns==0)
        {  
          
          if (r==0&&g==0&&b==0)
          {
            r = gc->colorl->r;
            g = gc->colorl->g;
            b = gc->colorl->b;
          }
    
          #ifdef _DEBUG
            if (DEBUG==TRUE) printf("colors c(%d) rgb(%d,%d,%d,%d)\n",color,r,g,b,lhelper);
          #endif
          if (g+1 > -1) r-=1; else r = 0;
          if (g+1 > -1) g-=1; else g = 0;
          if (b+1 > -1) b-=1; else b = 0;
      
          color = r+(g*(0xff+1))+(b*(0xff+1)*(0xff+1));
        
        }
        
        if (ns!=1)
          ns++;
        else
          ns = 0;
      }
      
      for (xn = x; xn < x+w; xn++)
      { 
        if (
          xn>=mainw &&
          yn>=mainh &&
          xn < 0 && yn < 0
        )
        {
          continue;
        }
         
        bleft = FALSE;
        if ( val > 0 &&
          (
          ((yn-y+1)*(xn-x+1)<val) || 			// top left
          (((y+h)-yn)*((x+w)-xn)<val) ||		// bottom right
          (((y+h)-yn)*(xn-x+1)<val) ||			// bottom left
          ((yn-y+1)*((x+w)-xn)<val)			// top right 
          ))
        {
          continue;
        }
        
        if (btrans==FALSE) // moving color of border pixels "seriosly eats cpu time"
        {
          if ( val > 0 )
          {
            if ( xn+1 != x+h &&
                  ((((y+h)-yn)*((x+w)-(xn+1))<val) || 	// bottom right
                  (((y+h)-(yn+1))*((x+w)-(xn+1))<val))
              )
              bright = TRUE;
            else if ( xn+1 != x+h &&
                  (((yn-y+1)*((x+w)-(xn+1))<val) ||	// top right
                  (((yn-1)-y+1)*((x+w)-(xn+1))<val))
              )
              bright = TRUE;
            else if (
                  ((((y+h)-yn)*((xn-1)-x+1)<val) ||	// bottom left
                  (((y+h)-(yn+1))*((xn-1)-x+1)<val))
              )
              bleft = TRUE;
            else if (
                  (((yn-y+1)*((xn-1)-x+1)<val) ||		// top left
                  (((yn-1)-y+1)*((xn-1)-x+1)<val))
              )
              bleft = TRUE;
            else if ( xn == x )				// left
              bleft = TRUE;
          }
          else if ( val == 0 )
          {
            if (xn==x+w-1) 	bright = TRUE;
            if (xn==x) 		bleft = TRUE;
          }
        }
        
        if (btrans==TRUE)
        {
          #ifdef _USE_GUI_XLIB_INTERNALS
            pixel = XGetPixel(gui_GetMain()->surface,xn,yn);
          #else
            __get_pixel(
              bigInterface->surfaceData,
              bigInterface->width,
              bigInterface->height,
              xn,yn,
              bigInterface->bpp/8,
              (char*)&pixel
            );
          #endif
        
          if (pixel==last)
          {
            color = last_moved;
          }
          else
          {
            _color_get_components(pixel,&r,&g,&b);
            color = _color_move_components(r,g,b,-64); //r+(g*256)+(b*256*256);
            last 		= pixel;
            last_moved 	= color;
          }
        }
        
        //color = _color_combine(pixel,color);
        
        //if (btrans==FALSE&&(btop==TRUE||bbottom==TRUE||bright==TRUE||bleft==TRUE))
        //  XPutPixel(gui_GetMain()->surface,xn,yn,border);
        //else
        
        #ifdef _USE_GUI_XLIB_INTERNALS
          XPutPixel(gui_GetMain()->surface,xn,yn,color);
        #else
          __put_pixel(
            bigInterface->surfaceData,
            bigInterface->width,
            bigInterface->height,
            xn,yn,
            bigInterface->bpp/8,(char*)&color
          );
        #endif
      }
    }
    rec_ExposeAddRect(rec_expose,x,y,w,h,color);
    return 0;
  }

  int gui_DrawRectangleExA(
    struct gui_Draw *draw
  )
  {
    if (!draw) return -1;
    _gui_draw_round_rectangle( 
      draw->gc, draw->surface, 
      draw->x, draw->y,
      draw->w, draw->h,
      draw->round, draw->gradient 
    );
    return 0;
  }

  /**
   * mika - preffered method
   *
   */
  int gui_DrawRoundRectangle( 
    struct gui_GC *gc, 
    int x, int y, 
    int w, int h, 
    int val 
  )
  {
    #ifdef _USE_GL_RENDERER
      static BOOL btrans;
      static gui_GlColor glc;
      static unsigned char r;
      static unsigned char g;
      static unsigned char b;
      static unsigned char a;
      
      if (gc)
      {
        btrans = FALSE;
        a = 0xff;
        
        if (!gc->colorl)
        {
          #ifdef _DEBUG
            if (DEBUG==TRUE) printf("warning: gui_DrawRoundRectangle colorl not set (deprecated color use)\n");
          #endif
          return -1;
        }
        
        if (gc->colorl->value==C_TRANS_VALG)
        {
          btrans = TRUE;
          r = 0xa0;
          g = 0xa0;
          b = 0xa0;
          a = 0xa0;
        }
        else
        {
          if (gc->colorl)
          {
            r = gc->colorl->r;
            g = gc->colorl->g;
            b = gc->colorl->b;
          }
        }
        
        glc.rgba[gl->r_pos] = r;
        glc.rgba[gl->g_pos] = g;
        glc.rgba[gl->b_pos] = b;
        glc.rgba[gl->a_pos] = a;
        gui_GlSetColor(gl,&glc);
      }
      else // gl not set, use white
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("gui_DrawRoundRectangle gl not set\n");
        #endif
        glc.rgba[gl->r_pos] = 0xff;
        glc.rgba[gl->g_pos] = 0xff;
        glc.rgba[gl->b_pos] = 0xff;
        glc.rgba[gl->a_pos] = 0xc0;
        gui_GlSetColor(gl,&glc);
      }
      
      if (h/2<=val) val = h/2;
      if (w/2<=val) val = w/2;
      return gui_GlDrawRoundRect( gl, x, y, x+w, y+h, val );
    #else
      return _gui_draw_round_rectangle( gc, NULL, x, y, w, h, val, 0 );
    #endif
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("gui_DrawRoundRectangle end\n");
    #endif
  }
  
  int gui_DrawRoundRectangleSurface( 
    struct gui_GC *gc, 
    struct gui_Surface *surface, 
    int x, int y, 
    int w, int h, 
    int val 
  )
  {
    return _gui_draw_round_rectangle( gc, surface, x, y, w, h, val, 0 );
  }
  
  int gui_DrawRoundRectangleGradient( 
    struct gui_GC *gc, 
    int x, int y, 
    int w, int h, 
    int val, int grad 
  )
  {
    return _gui_draw_round_rectangle( gc, NULL, x, y, w, h, val, grad );
  }
  
  int gui_DrawRoundRectangleGradientSurface( 
    struct gui_GC *gc, 
    struct gui_Surface *surface, 
    int x, int y, 
    int w, int h, 
    int val, int grad 
  )
  {
    return _gui_draw_round_rectangle( gc, surface, x, y, w, h, val, grad );
  }

  int _gui_draw_image_surface( 
    gui_Image *image, 
    struct gui_Surface *surface, 
    int x, int y, 
    int w, int h 
  )
  {
    static struct gui_Widget	*main;
    static unsigned long 	pixel;
    static short int 		round;
    static int			mainw;
    static int			mainh;
    static int			xn, yn;
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("gui_DrawImage __bounds (%dx%d)+%d+%d in (%dx%d)\n",w,h,x,y,
                              gui_bounds.width,gui_bounds.height,
                              gui_bounds.x,gui_bounds.y);
    #endif
    
    if (x>2048||y>2048||x<-2048||y<-2048) // sanity check
    {
      printf(":warning gui_DrawImage sanity check bypass by image draw\n");
      return -1;
    }
    
    if (
      w<=1 ||
      h<=1
      ) return -1;
    
    main = gui_GetMain();
    
    mainw = main->attr.w;
    mainh = main->attr.h;
    
    round = image->round;
    #ifdef _DEBUG
      if (DEBUG==TRUE && round>0) printf("round_image(%d) main(%dx%d)\n",round,mainw,mainh);
    #endif
    for (yn = y; yn < y + h; yn++)
    {
      for (xn = x; xn < x + w; xn++)
      {
        if (
          xn < 0 ||
          yn < 0 ||
          xn+1 > mainw ||
          yn+1 > mainh
        )
        {
          continue;
        }
        
        if (
          xn-x > image->w-1 || 
          yn-y > image->h-1
        )
        {
          continue;
        }
        
        #ifdef _USE_GUI_XLIB_INTERNALS
          pixel = XGetPixel(image->ximage,xn-x,yn-y);
        #else
          __get_pixel(
                      image->data,
                      image->w,image->h,
                      xn-x,yn-y,
                      4,(char*)&pixel
                      );
        #endif
        if (pixel == 0x00 && image->type==IMG_XPM)
          continue;
        
        if ( round > 0 &&
          (
          ((yn-y+1)*(xn-x+1)<round) || 		// top left
          (((y+h)-yn)*((x+w)-xn)<round) ||	// bottom right
          (((y+h)-yn)*(xn-x+1)<round) ||	// bottom left
          ((yn-y+1)*((x+w)-xn)<round)		// top right 
          ))
        {
          continue;
        }
        if (!surface) // draw to main surface
        {
          #ifdef _USE_GUI_XLIB_INTERNALS
            XPutPixel(main->surface,xn,yn,pixel);
          #else
            if (
              xn < gui_bounds.x+gui_bounds.width ||
              yn < gui_bounds.y+gui_bounds.height 
            ) // move before processing
            {
              __put_pixel(
                bigInterface->surfaceData,
                bigInterface->width,
                bigInterface->height,
                xn,yn,
                bigInterface->bpp/8,(char*)&pixel
              );
            }
          #endif
        }
        else // draw to a defined surface
        {
          #ifdef _USE_GUI_XLIB_INTERNALS
            XPutPixel(surface->image->ximage,xn,yn,pixel);
          #else
            if (
              xn < gui_bounds.x+gui_bounds.width ||
              yn < gui_bounds.y+gui_bounds.height 
            ) // move before processing
            {
              __put_pixel(
                surface->data,
                bigInterface->width,
                bigInterface->height,
                xn,yn,
                bigInterface->bpp/8,(char*)&pixel
              );
            }
          #endif
        }
      }
    }
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("gui_DrawImage_end main (%dx%d)\n",mainw,mainh);
    #endif
    
    return 0;
  }

  /*
    blit part of image to a position on main surface '1'=from '2'=to 'w,h'=size
  */
  int _gui_draw_image( gui_Image *image, int x1, int y1, int x2, int y2, int w, int h, int round )
  {
    if (w<1||h<1) return -1;
    static int xn, yn, xn2, yn2;
    static unsigned long pixel;
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("_gui_draw_image x1,y1(%d,%d) x2,y2(%d,%d) w,h(%d,%d)\n",
        x1,y1,x2,y2,w,h);
    #endif
    
    xn2 = 0;
    yn2 = 0;
    
    for (yn = y1; yn < y1 + h; yn++)
    {
      for (xn = x1; xn < x1 + w; xn++)
      {
        if (
          gui_GetMain()->attr.w<x2+xn2+1 ||
          gui_GetMain()->attr.h<y2+yn2+1 ||
          x2+xn2 < 0 ||
          yn+yn2 < 0
          )
        {
          xn2++;
          continue;
        }
        
        if ( round > 0 &&
          (
          ((yn-y1+1)*(xn-x1+1)<round) || 	// top left
          (((y1+h)-yn)*((x1+w)-xn)<round) ||	// bottom right
          (((y1+h)-yn)*(xn-x1+1)<round) ||	// bottom left
          ((yn-y1+1)*((x1+w)-xn)<round)		// top right 
          ))
        {
          xn2++;
          continue;
        }
        #ifdef _USE_GUI_XLIB_INTERNALS
          pixel = XGetPixel(image->ximage,xn,yn);
        #else
          __get_pixel(
            image->data,
            image->w,image->h,
            xn,yn,4,
            (char*)&pixel
          );
        #endif
        if (pixel == C_TRANS_VAL)
        {
          xn2++;
          continue;
        }
        
        #ifdef _USE_GUI_XLIB_INTERNALS
          XPutPixel(gui_GetMain()->surface,x2+xn2,y2+yn2,pixel); // yn2 + y2
        #else
        
          if (
            x2+xn2 < gui_bounds.x+gui_bounds.width ||
            y2+yn2 < gui_bounds.y+gui_bounds.height 
          ) // move before processing
          {
        
            __put_pixel(
              bigInterface->surfaceData,
              bigInterface->width,
              bigInterface->height,
              x2+xn2,y2+yn2,
              bigInterface->bpp/8,
              (char*)&pixel
            );
          }
        #endif
        
        xn2++;
      }
      xn2 = 0;
      yn2++;
    }
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("_gui_draw_image end\n"); 
    #endif
    return 0;
  }

  int gui_DrawImageEx( gui_Image *image, int x1, int y1, int x2, int y2, int w, int h )
  {
    return _gui_draw_image(image,x1,y1,x2,y2,w,h,0);
  }

  int gui_DrawImageRound( gui_Image *image, int x1, int y1, int x2, int y2, int w, int h, int val )
  {
    return _gui_draw_image(image,x1,y1,x2,y2,w,h,val);
  }

  /*
    draw an image to a surface 
  */
  int gui_DrawImageSurface( 
    gui_Image *image, 
    struct gui_Surface *surface, 
    int x, int y, 
    int w, int h )
  {
    return _gui_draw_image_surface(image,surface,x,y,w,h);
  }

  int gui_DrawImage( 
    gui_Image *image, 
    int x, int y, 
    int w, int h )
  {
    return _gui_draw_image_surface(image,NULL,x,y,w,h);
  }

  /*
    move colors of image at value for ex. (1=r 2=g 3=b)
    at = 0, then move all
    at values: gui_RED( ), gui_GREEN( ), gui_BLUE( )
  */
  int gui_ImageMoveAlpha( gui_Image *image, int at, int value ) // needs sync
  {
    static char hex[7];
    static int n, xn, yn;
    static unsigned long pixel, pixel_mem;
    
    if (!image || value==0) return -1;
    
    pixel = pixel_mem = -1;
    for (yn = 0; yn < image->h; yn++)
    {
      for (xn = 0; xn < image->w; xn++)
      {
        #ifdef _USE_GUI_XLIB_INTERNALS
          pixel = XGetPixel(image->ximage,xn,yn);
        #else
          __get_pixel(
            image->data,
            image->w,image->h,
            xn,yn,4,
            (char*)&pixel
          );
        #endif
        if (pixel_mem==-1 || pixel!=pixel_mem )
        {
          if (pixel == C_TRANS_VAL)
            continue;
          _long2hex(pixel,(char*)hex);
          for (n = 0; n < abs(value); n++)
          {
            if (value<0)
            {
              if (at==0)
                _hex_move_down((char*)hex);
              else
                _hex_move_down_at((char*)hex,at);
            }
            else
            {
              if (at==0)
                _hex_move_up_ex((char*)hex);
              else
                _hex_move_up_at((char*)hex,at);
            }
          }
          pixel = _hex2long((char*)hex);
          pixel_mem = pixel;
        }
        #ifdef _USE_GUI_XLIB_INTERNALS
          XPutPixel(image->ximage,xn,yn,pixel);
        #else
          __put_pixel(
            image->data,
            image->w,image->h,
            xn,yn,4,
            (char*)&pixel
          );
        #endif
      }
    }
    return 0;
  }

  /*
    move the alpha at a speciffic area on the main surface 
  */
  int gui_SurfaceMoveAlpha( int x, int y, int w, int h, int value ) // needs sync
  {
    static int xn, yn, n;
    static char hex[7];
    static unsigned long pixel;
    static unsigned long pixel_mem;
    
    if ( w<1 || h<1 ) return -1;
    
    pixel = pixel_mem = -1;
    for (yn = y; yn < y+h; yn++)
    {
      for (xn = x; xn < x+w; xn++)
      {
        #ifdef _USE_GUI_XLIB_INTERNALS
          pixel = XGetPixel(gui_GetMain()->surface,xn,yn);
        #else
          __get_pixel(
            bigInterface->surfaceData,
            bigInterface->width,
            bigInterface->height,
            xn,yn,
            bigInterface->bpp/8,
            (char*)&pixel
          );
        #endif
        if (pixel_mem==-1 || pixel_mem!=pixel)
        {
          _long2hex(pixel,(char*)hex);
          for (n = 0; n < abs(value); n++)
          {
            if (value<0)
            {
              _hex_move_down((char*)hex);
            }
            else
            {
              _hex_move_up_ex((char*)hex);
            }
          }
          pixel = _hex2long((char*)hex);
          pixel_mem = pixel;
        }
        
        #ifdef _USE_GUI_XLIB_INTERNALS
          XPutPixel(gui_GetMain()->surface,xn,yn,pixel);
        #else
          __put_pixel(
            bigInterface->surfaceData,
            bigInterface->width,
            bigInterface->height,
            xn,yn,
            bigInterface->bpp/8,
            (char*)&pixel
          );
        #endif
      }
    }
    return 0;
  }

  #ifdef _USE_GUI_XLIB_INTERNALS
    inline int __gui_surface_to_window( XImage *xi, int w, int h )
    {
      XPutImage(
        gui_GetMain()->dpy,
        gui_GetMain()->id,
        DefaultGC(gui_GetMain()->dpy,gui_GetMain()->screen),
        xi,
        0,0,0,0,
        w,
        h
        );
      return 0;
    }
  #else
    /**
     *  else handled by in bigInterface->platform
    */
  #endif

  int _gui_surface_to_window( )
  {
    #ifdef _USE_GUI_XLIB_INTERNALS
      static unsigned int	xn;
      static unsigned int yn;
      static unsigned long pixel;
      static struct gui_GC *neutral = NULL;
      
      if (neutral==NULL)
      {
        neutral = (struct gui_GC*)malloc(sizeof(struct gui_GC));
        if ( gui_InitGCEx((struct gui_GC*)neutral,(char*)"#ffffff") != 0 ) return -1;
      }
      
      if (rotation == ROT_LEFT || rotation == ROT_RIGHT || rotation == ROT_DOWN)
      {
        for (yn = 0; yn < gui_GetMain()->attr.h; yn++)
        {
          for (xn = 0; xn < gui_GetMain()->attr.w; xn++)
          {
            #ifdef _USE_GUI_XLIB_INTERNALS
              pixel = XGetPixel(
                gui_GetMain()->surface,
                xn,yn);
            #else
              
            #endif
            
            if (rotation == ROT_LEFT)
            {
              #ifdef _USE_GUI_XLIB_INTERNALS
                XPutPixel(			// todo, make own function
                  surface->image->ximage,
                  gui_GetMain()->attr.h-yn,
                  xn,
                pixel);
              #endif
            }
            else if (rotation == ROT_RIGHT)
            {
              #ifdef _USE_GUI_XLIB_INTERNALS
                XPutPixel(
                  surface->image->ximage,
                  yn,
                  gui_GetMain()->attr.w-xn,
                  pixel);
              #else
              
              #endif
            }
            else if (rotation == ROT_DOWN)
            {
              #ifdef _USE_GUI_XLIB_INTERNALS
                XPutPixel(
                  surface->image->ximage,
                  gui_GetMain()->attr.w-xn,
                  gui_GetMain()->attr.h-yn,
                  pixel);
              #endif
            }
          }
        }
        if (rotation != ROT_DOWN)
          __gui_surface_to_window(
            surface->image->ximage,
            gui_GetMain()->attr.h,gui_GetMain()->attr.w);
        else
          __gui_surface_to_window(
            surface->image->ximage,
            gui_GetMain()->attr.w,gui_GetMain()->attr.h);
      }
      else if (rotation == ROT_NONE)
        __gui_surface_to_window(
            gui_GetMain()->surface,
            gui_GetMain()->attr.w,gui_GetMain()->attr.h);
      
      return 0;
    #else
      //TODO ROTATE PIXELS
      bigInterface->platform.refresh(&bigInterface->platform);
    #endif
  }

  /*
    refresh a rectangle on surface 
  */
  int gui_SurfaceRefresh( struct gui_Surface *surface, int x, int y, int w, int h )
  {
    
    return 0;
  }

  #ifdef _USE_GUI_XLIB_INTERNALS
    XImage *_gui_surface_init_ex( XImage *xi, void *data, int w, int h )
    {
      static int 				count = 0;
      static XPixmapFormatValues		*formats = NULL;
      static XPixmapFormatValues		*format = NULL;
      
      formats = XListPixmapFormats(gui_GetMain()->dpy,&count);
      
      for(format = formats; count > 0; count--, format++)
      {
        if (format->depth == DefaultDepth(gui_GetMain()->dpy,gui_GetMain()->screen))
        {
          xi = XCreateImage(
                                  gui_GetMain()->dpy,
                                  DefaultVisual(gui_GetMain()->dpy,gui_GetMain()->screen),
                                  DefaultDepth(gui_GetMain()->dpy,gui_GetMain()->screen),
                                  ZPixmap,
                                  0,
                                  data,
                                  w, h,
                                  format->scanline_pad,
                                  0
                                );
        }
      }
      return xi;
    }
  #endif

  /*
    create ximage, not fetch from existing window
  */
  struct gui_Surface *gui_SurfaceInitEx( struct gui_Widget *main, struct gui_Surface *surface, 
    int w, int h )
  {
    #ifdef _USE_GUI_XLIB_INTERNALS
      static int 				count = 0;
      static XPixmapFormatValues		*formats = NULL;
      static XPixmapFormatValues		*format = NULL;
    #endif
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":gui_SurfaceInitEx (%d %d)\n",w,h);
    #endif
    
    if (w==0||h==0) 
    {
      printf("bad geometry, surface will not be generated\n");
      return NULL;
    }
    if (surface==NULL) surface = (struct gui_Surface*)malloc(sizeof(struct gui_Surface));
    
    /**
     *  surface->image holds the surface data in image->data
    */
    surface->image = (struct gui_Image*)malloc(sizeof(struct gui_Image));
    if (bigInterface)
      surface->data = malloc(w*h*(bigInterface->bpp/8));
    else
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf(":gui_SurfaceIitEx without bigInterface\n");
      #endif
    }
    
    #ifdef _USE_GUI_XLIB_INTERNALS
      formats = XListPixmapFormats(main->dpy,&count);
      surface->data = NULL;
    
      for(format = formats; count > 0; count--, format++)
      {
        if (format->depth == DefaultDepth(main->dpy,main->screen))
        {
          surface->data = malloc(w*h*4); // todo
          surface->image->ximage = XCreateImage(
                                  main->dpy,
                                  DefaultVisual(main->dpy,main->screen),
                                  DefaultDepth(main->dpy,main->screen),
                                  ZPixmap,
                                  0,
                                  surface->data,
                                  w, h,
                                  format->scanline_pad,
                                  0
                                );
        }
      }
    #endif
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":gui_SurfaceInitEx end\n");
    #endif
    
    return surface;
  }
  
  struct gui_Surface *gui_SurfaceInit( struct gui_Widget *main, struct gui_Surface *surface, int w, int h )
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":gui_SurfaceInit (%d %d)\n",w,h);
    #endif
    if (w==0||h==0) return NULL;
    if (surface==NULL) surface = (struct gui_Surface*)malloc(sizeof(struct gui_Surface));
    surface->image = (struct gui_Image*)malloc(sizeof(struct gui_Image));
    #ifdef _USE_GUI_XLIB_INTERNALS
      surface->image->ximage = XGetImage(main->dpy,main->id,0,0,w,h,AllPlanes,ZPixmap);
    #endif
    return surface;
  }

  int gui_RGB( int which ) { return which; }
  int gui_RED( ) { return C_R; }
  int gui_GREEN( ) { return C_G; }
  int gui_BLUE( ) { return C_B; }

  /*
    parse a string of red/green/blue (000-255/000-255/000-255)
  */
  int gui_ParseColorValue( char *_value, int *r, int *g, int *b )
  {
    sscanf(_value,"%03d/%03d/%03d",r,g,b);
    return 0;
  }

  inline struct _gui_cache *_gui_cache_text_init(
    struct _gui_cache *cache,  
    struct gui_Surface *surface_from,
    int x, int y,
    int w, int h,
    char *str
    )
  {
    static struct _gui_cache_text *cache_text = NULL;
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":_gui_cache_text_init\n");
    #endif
    cache = _gui_cache_init(cache,surface_from,x,y,w,h);
    if (cache->sub==NULL)
    {
      cache->sub = (struct _gui_cache_text*)malloc(sizeof(struct _gui_cache_text));
      cache_text = cache->sub;
      cache_text->str = NULL;
    }
    else
      cache_text = cache->sub;
    if (cache_text->str!=NULL) free(cache_text->str);
    cache_text->str = (char*)malloc(sizeof(char)*(strlen(str)+1));
    strcpy(cache_text->str,str);
    cache_text->len = strlen(str);
    return cache;
  }

  struct _gui_cache *_gui_cache_init( 
    struct _gui_cache *cache,  
    struct gui_Surface *surface_from,
    int x, int y,
    int w, int h 
    )
  {
    static int xn = 0;
    static int yn = 0;
    static long pixel;
    if (w<1||h<1||x<0||y<0) return NULL;
    if ( cache == NULL )
    {
      cache = (struct _gui_cache*)malloc(sizeof(struct _gui_cache));
      cache->surface = gui_SurfaceInit(gui_GetMain(),NULL,w,h);
      cache->surface->image->w = w;
      cache->surface->image->h = h;
      cache->sub = NULL;
    }
    for (yn = 0; yn < h; yn++)
    {
      for (xn = 0; xn < w; xn++)
      {
        #ifdef _USE_GUI_XLIB_INTERNALS
          pixel = XGetPixel(surface_from->image->ximage,xn+x,yn+y);
          XPutPixel(cache->surface->image->ximage,xn,yn,pixel);
        #else
          __get_pixel(
            surface_from->image->data,
            surface_from->image->w,
            surface_from->image->h,
            xn+x,yn+y,4,
            (char*)&pixel
          );
          __put_pixel(
            surface_from->image->data,
            surface_from->image->w,
            surface_from->image->h,
            xn,yn,4,
            (char*)&pixel
          );
        #endif
      }
    }
    return cache;
  }

  /** mika - can eat CPU time like wild 
   */
  inline BOOL _gui_cache_text_iscache( char *str, int w, int h )
  {
    static struct _gui_cache *cache = NULL;
    static struct _gui_cache_text *cache_text = NULL;
    static struct trop_List *current = NULL;
    static int len = 0;
    
    len = strlen(str);
    current = lst_txtcache; 
    
    while(current)
    {
      cache = current->ptr;
      cache_text = cache->sub;
      if (
        ( cache->surface->image->w == w &&
        cache->surface->image->h == h ) ||
        (w == -1 && h == -1)
        )
      {
        if (len == strlen(cache_text->str))
        {
          if (memcmp(str,cache_text->str,len)==0)
          {
            return TRUE;
          }
        }
      }
      current = current->next;
    }
    return FALSE;
  }

  inline struct _gui_cache *_gui_cache_text_getat( char *str )
  {
    static struct _gui_cache *cache = NULL;
    static struct _gui_cache_text *cache_text = NULL;
    static struct trop_List *current = NULL;
    static int len = 0;
    
    len = strlen(str);
    current = lst_txtcache;
    
    while(current)
    {
      cache = current->ptr;
      cache_text = cache->sub;
      if (len == cache_text->len)
      {
        if (memcmp(str,cache_text->str,len)==0)
        {
          return cache;
        }
      }
      current = current->next;
    }
    return NULL;
  }

  int _gui_cache_text_list( )
  {
    static struct _gui_cache *cache = NULL;
    static struct _gui_cache_text *cache_text = NULL;
    static struct trop_List *current = NULL;
    current = lst_txtcache;
    while(current)
    {
      cache = current->ptr;
      cache_text = cache->sub;
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf(":cache_text (%s)\n",cache_text->str);
      #endif
      current = current->next;
    }
    return 0;
  }

  inline int _gui_cache_text_add(
    int x, int y,
    int w, int h,
    char *str  
  )
  {
    struct _gui_cache *cache = NULL;
    struct _gui_cache_text *cache_text = NULL;
    if (!str||w<1||h<1) return -1;
    
    cache = _gui_cache_text_init(NULL,txt_surface,x,y,w,h,str);
    lst_txtcache = trop_ListAdd(lst_txtcache,cache);
    
    return 0;
  }

  int gui_DrawText( char *str, int x, int y, int w, int h )
  {
    #ifdef USEFT
      #ifdef _FT_TYPE_CUSTOM
        if (ftb==FALSE) 
        {
          _ft_init();
          grSetPixbuf( ftdisplay->surface, txt_surface->image->ximage );
          ftb = TRUE;
        }
        FTDemo_Display_Clear(ftdisplay);
      #endif
    #endif
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":gui_DrawText\n");
    #endif
    if (str==NULL||x<0||y<0||w<=0||h<=0) return -1;
    #ifdef USEFT
      #ifdef _FT_TYPE_CUSTOM
        FTDemo_String_Set(fthandle,str);
        FTDemo_String_Draw(fthandle,ftdisplay,&ftstatus.sc,x,y+gui_GetMain()->attr.ftsize);
        grRefreshRectangle(ftdisplay->surface,x,y,w,h); // todo - innefective
      #endif
    #endif
    return 0;
  }

  #define _GUI_TEXT_MARGIN 2

  int gui_TextGetSize( char *str, int *strw, int *strh )
  {
    static struct _gui_cache *cache = NULL;
    
    #ifdef _DEBUG
      #ifdef _FT_TYPE_CUSTOM
        if (DEBUG==TRUE) printf("gui_TextGetSize _ft ftb(%d)\n",ftb);
      #endif
    #endif
    if (!str)
    {
      *strw = 0;
      *strh = 0; 
      return -1;
    }
    if (strlen(str)==0)
    {
      *strw = 0;
      *strh = 0;
      return -1;
    }
    
    #ifdef _USE_GL_TEXT
      return gui_GlFontGetSize( gl, gui_GetMain()->glfont, str, strw, strh );
    #endif
    
    #ifdef USEFT
      #ifdef _FT_TYPE_CUSTOM
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("gui_TextGetSize() _ft_init run\n");
        #endif
        if (ftb==FALSE) 
        {
          _ft_init();
          if (!txt_surface||!ftdisplay)
          {
            printf("txt_surface or ftdisplay error\n");
            exit(2);
          }
          grSetPixbuf( ftdisplay->surface, txt_surface->image->ximage );
          ftb = TRUE;
        }
        FTDemo_Display_Clear(ftdisplay);
      
        if (DEBUG==TRUE) printf("a000\n");
        grColor gcolor;
        memset(&gcolor,0xff,sizeof(grColor));
      
        grFillRect(
          &ftdisplay->surface->bitmap,
          0,0,
          gui_GetMain()->attr.w,
          gui_GetMain()->attr.ftsize,
          gcolor
          );
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("a00 lowDraw\n");
        #endif
        FTDemo_String_Set(fthandle,str);
        FTDemo_String_Draw(
          fthandle,
          ftdisplay,
          &ftstatus.sc,
          0,0+gui_GetMain()->attr.ftsize);
        grRefreshRectangle(
          ftdisplay->surface,
          0,0,
          gui_GetMain()->attr.w,
          gui_GetMain()->attr.ftsize); // todo - innefective
        #ifdef _DEBUG
          if (DEBUG==TRUE) 
            printf("__textcache txt(%d/%d)\n",fthandle->width,fthandle->height);
        #endif
        
        if (fthandle->width > gui_GetMain()->attr.w) 
          fthandle->width = gui_GetMain()->attr.w;
      
        *strw = fthandle->width;
        *strh = fthandle->height;
      #endif
    #endif
    
    return 0;
  }

  #define _DRAW_TEXT_USECACHE // remove 'NO' to enable

  int _gui_draw_text_ex(
    struct gui_TextSim 	*ts,
    char		*str,
    int			x,
    int			y,
    int			w,
    int			h,
    int			round
  )
  {
    static int		placement;
    #ifdef _FT_TYPE_CUSTOM
      static grColor	gcolor;
    #endif
    
    placement 	= -1;
    if 	(str==NULL && ts!=NULL)
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("__text ts\n");
      #endif
      str 	= ts->str;
      placement	= ts->placement;
    }
    else if (str==NULL && ts==NULL)	return -1;
    
    #ifdef USEFT
      #ifdef _FT_TYPE_CUSTOM
        struct _gui_cache *cache = NULL;
        #ifdef _DRAW_TEXT_USECACHE
          if (_gui_cache_text_iscache(str,w,h)==FALSE)
        #endif
        {
          FTDemo_String_Set(fthandle,str);
          FTDemo_String_Draw(
            fthandle,
            ftdisplay,
            &ftstatus.sc,
            0,0+h-(h/4));
          grRefreshRectangle(
            ftdisplay->surface,
            0,0,
            w,h); // todo - innefective
          #ifdef _DEBUG
            if (DEBUG==TRUE) 
              printf("__textcache txt(%d/%d)\n",fthandle->width,fthandle->height);
          #endif
          
          if (fthandle->width>w) fthandle->width = w;
          if (fthandle->height>h) fthandle->height = h;
          
          #ifdef _DRAW_TEXT_USECACHE
            _gui_cache_text_add(
              0,0,
              fthandle->width,h,
              str);
          #endif
        }
        
        #ifdef _DRAW_TEXT_USECACHE
          cache = _gui_cache_text_getat(str);
          #ifdef _DEBUG
            if (DEBUG==TRUE) 
              printf(":copy_text_surface (%d %d) (%d %d)\n",x,y,w,h);
          #endif
        
          if (placement==PLACE_CENTER) x = x+((w-cache->surface->image->w)/2);
        
          gui_DrawImageRound(
            cache->surface->image,
            0,0,
            x,y,
            cache->surface->image->w,h,
            round);
            
          if (ts!=NULL)
          {
            if (ts->pos.w==0) ts->pos.w = cache->surface->image->w; 
            if (ts->pos.h==0) ts->pos.h = h;
          }
        #else
          if (placement==PLACE_CENTER) x = x+((w-fthandle->width)/2);
          if (placement==PLACE_FILL) fthandle->width = h; // todo
        
          gui_DrawImageRound(
            txt_surface->image,
            0,0,
            x,y,
            fthandle->width,h,
            round);
          
          if (ts!=NULL)
          {
            if (ts->pos.w==0) ts->pos.w = fthandle->width; 
            if (ts->pos.h==0) ts->pos.h = h;
          }
        #endif
        
        if (REC_ENABLE == 1)
        {
          rec_ExposeAddTxt(rec_expose,x,y,fthandle->width,h,str);
        }
      #endif
    #endif
    
    return 0;
  }

  /*
    draw text first to separate layer, then move to main one 
  */
  int gui_DrawTextEx( char *str, int x, int y, int w, int h, int round )
  {
    #ifdef USEFT
      #ifdef _FT_TYPE_CUSTOM
        if (ftb==FALSE) 
        {
          _ft_init();
          grSetPixbuf( ftdisplay->surface, txt_surface->image->ximage );
          ftb = TRUE;
        }
        FTDemo_Display_Clear(ftdisplay);
      #endif
    #endif
    if (str==NULL||w<1||h<1) return -1;
    #ifndef _USE_GL_TEXT
      return _gui_draw_text_ex(NULL,str,x,y,w,h,round);
    #else
      if (wCurrent)
      {
        cairo_set_source_rgb(cairo,
          (double)(wCurrent->glfont->color.rgba[0]/0xff),
          (double)(wCurrent->glfont->color.rgba[1]/0xff),
          (double)(wCurrent->glfont->color.rgba[2]/0xff)
          );
        cairo_set_font_size(cairo, wCurrent->glfont->height);
        cairo_move_to(cairo, x, y);
        cairo_show_text(cairo, str);
        /**
        if (wCurrent->glfont)
          return gui_GlFontDraw(gl,wCurrent->glfont,str,x,y,x+w,y+h);
        else
          return gui_GlFontDraw(gl,gui_GetMain()->glfont,str,x,y,x+w,y+h);
        */
        return 0;
      }
      else
        return gui_GlFontDraw(gl,gui_GetMain()->glfont,str,x,y,x+w,y+h);
    #endif
  }

  int gui_DrawTextExA( struct gui_TextSim *ts, int x, int y, int w, int h, int round )
  {
    #ifdef _USE_GL_TEXT
      static int placement;
      static int _x, _y, _w, _h;
    #endif
    #ifdef USEFT
      #ifdef _FT_TYPE_CUSTOM
        if (ftb==FALSE) 
        {
          _ft_init();
          grSetPixbuf( ftdisplay->surface, txt_surface->image->ximage );
          ftb = TRUE;
        }
        FTDemo_Display_Clear(ftdisplay);
      #endif
    #endif
    if (ts==NULL||w<1||h<1) return -1;
    #ifndef _USE_GL_TEXT
      return _gui_draw_text_ex(ts,NULL,x,y,w,h,round);
    #else
      //gui_GlFontGetSize(gl,gui_GetMain()->glfont,ts->str,&_w,&_h);
      //switch(ts->placement)
      {
        //case PLACE_CENTER:
        //  x += (w-_w)/2;
        //  y -= (h-_h)/2;
        x += (w-w)/2;
        x+=10;
        y-=8;
        //break;
      }
      if (wCurrent)
      {
        cairo_set_source_rgb(cairo,
          (double)((double)wCurrent->glfont->color.rgba[0]/(double)0xff),
          (double)((double)wCurrent->glfont->color.rgba[1]/(double)0xff),
          (double)((double)wCurrent->glfont->color.rgba[2]/(double)0xff)
          );
        cairo_set_font_size(cairo, wCurrent->glfont->height);
        cairo_move_to(cairo, x, y);
        cairo_show_text(cairo, ts->str);
        //if (wCurrent->glfont)
        //  return gui_GlFontDraw(gl,wCurrent->glfont,ts->str,x,y,x+w,y+h);
        //else
        //  return gui_GlFontDraw(gl,gui_GetMain()->glfont,ts->str,x,y,x+w,y+h);
      }
      else
        return gui_GlFontDraw(gl,gui_GetMain()->glfont,ts->str,x,y,x+w,y+h);
    #endif
  }

  /**
   * draw text complex
   */
  int gui_DrawTextCLX( struct gui_TextSim *ts, int x, int y, int w, int h, int round )
  {
    return 0;
  }
  