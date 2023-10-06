#include "sysWindow.h"
#include "mikaDesktop.h"
#include "config.h"

sysWindow::sysWindow( struct mikaFrame *frame, sysWindow *parent )
{
  #ifdef _DEBUG
    printf("sysWindow::sysWindow\n");
  #endif
  myFrame = *frame;
  
  attr.event_mask = ExposureMask | ButtonPressMask | ButtonReleaseMask |
    ButtonMotionMask | EnterWindowMask | LeaveWindowMask;
  attr.backing_store = Always;
  attr.override_redirect = True;
  unsigned long pixel = 0;
  attr.border_pixel = pixel;
  valuemask = CWEventMask | CWOverrideRedirect |
    CWBorderPixel | CWWinGravity;
  
  id = XCreateWindow(
    desktop->sys->dpy,
    desktop->sys->root,
    myFrame.x, myFrame.y,
    myFrame.w, myFrame.h,
    0, DefaultDepth(desktop->sys->dpy,desktop->sys->screen),
    CopyFromParent,
    DefaultVisual(desktop->sys->dpy,desktop->sys->screen),
    valuemask,&attr
    );  
  
  XCompositeRedirectWindow(desktop->sys->dpy, 
    id, True);
  
  XSelectInput(
    desktop->sys->dpy,id,
    ExposureMask|ButtonPressMask|ButtonReleaseMask|ButtonMotionMask
    );
}

sysWindow::~sysWindow( )
{
}

/** display window on screen
 *
 *  Creating a cairo surface out of a pixmap:
 *  cairo_surface_t *   cairo_xlib_surface_create_for_bitmap
      (Display *dpy,
      Pixmap bitmap,
      Screen *screen,
      int width,
      int height);
 */
void sysWindow::show( )
{
  #ifdef _DEBUG
    printf("sysWindow::show\n");
  #endif
  XMapWindow(desktop->sys->dpy,id);
  XFlush(desktop->sys->dpy);
  /** a new Pixmap is allocated each time the Window gets
   *  mapped or resized
   */
  pixmap = XCompositeNameWindowPixmap(desktop->sys->dpy,id);
  Screen *screen = DefaultScreenOfDisplay(desktop->sys->dpy);
  surface = cairo_xlib_surface_create_for_bitmap(
    desktop->sys->dpy,
    pixmap,screen,
    myFrame.w,myFrame.h
    );
  if (!surface)
  {
    #ifdef _DEBUG
      printf("sysWindow::show error couldn't create slave surface\n");
    #endif
  }
  
  ximageData = (void*)malloc(myFrame.w*myFrame.h*4);
  int count = 0;
  XPixmapFormatValues *formats = NULL;
  XPixmapFormatValues *format = NULL;
  formats = XListPixmapFormats(desktop->sys->dpy,&count);
  for(format = formats; count > 0; count--, format++)
  {
    if (format->depth == DefaultDepth(desktop->sys->dpy,desktop->sys->screen))
    {
      // create the XImage that we will be drawing to the X Window
      // and which pixels will be used as the canvas of the master
      // surface
      ximage = XCreateImage(
        desktop->sys->dpy,
        DefaultVisual(desktop->sys->dpy,desktop->sys->screen),
        DefaultDepth(desktop->sys->dpy,desktop->sys->screen),
        ZPixmap,
        0,
        (char*)ximageData,
        myFrame.w,myFrame.h,
        format->scanline_pad,
        0
        );
    }
  }
  
  masterSurface = cairo_image_surface_create_for_data(
    (unsigned char*)ximageData,
    CAIRO_FORMAT_ARGB32,
    myFrame.w,myFrame.h,
    myFrame.w*4
    );
  if (!masterSurface)
  {
    #ifdef _DEBUG
      printf("sysWindow::show couldn't create master surface\n");
    #endif
  }
}

void sysWindow::resize( struct mikaFrame *frame )
{
  #ifdef _DEBUG
    printf("sysWindow::resize\n");
  #endif
}

void sysWindow::onExpose( )
{
  #ifdef _DEBUG
    printf("sysWindow::onExpose\n");
  #endif
}

void sysWindow::onButtonDown( struct mikaPoint *point )
{
  if (!point)
  {
    #ifdef _DEBUG
      printf("sysWindow::onButtonDown error invalid argument\n");
    #endif
    return;
  }
  #ifdef _DEBUG
    printf("sysWindow::onButtonDown\n");
  #endif
}

void sysWindow::eventLoop( )
{
  XEvent event;
  while(1)
  {
    XNextEvent(desktop->sys->dpy,&event);
    struct mikaPoint point;
    switch(event.type)
    {
      case ButtonPress:
        point.x = event.xbutton.x;
        point.y = event.xbutton.y;
        onButtonDown(&point);
        break;
      case LeaveNotify:
        break;
      case ButtonRelease:
        break;
      case MotionNotify:
        break;
      case Expose:
        onExpose();
        break;
    }
  }
}
