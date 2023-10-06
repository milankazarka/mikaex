#include "mikaGlWindow.h"
#include "mikaGlConfig.h"
#include "mikaGlImage.h"

struct lowWindow *lowWindowInit( int w, int h )
{
  #ifdef _GLDEBUG
    printf("lowWindowInit\n");
  #endif
  struct lowWindow *window = NULL;
  window = (struct lowWindow*)malloc(sizeof(struct lowWindow));
  
  XInitThreads();
  window->dpy = XOpenDisplay(NULL);
  window->screen = DefaultScreen(window->dpy);
  Window root;
  unsigned long valuemask;
  XWindowAttributes attr1;
  XSetWindowAttributes attr;
  attr.event_mask = ExposureMask | ButtonPressMask | ButtonReleaseMask |
      ButtonMotionMask | EnterWindowMask | LeaveWindowMask;
  root = RootWindow(window->dpy,window->screen);
  XGetWindowAttributes(window->dpy,root,&attr1);
  valuemask = CWEventMask;
  
  #ifdef _OVERRIDE_REDIRECT
    valuemask += CWOverrideRedirect;
    attr.override_redirect = True;
  #endif
  
  window->id = XCreateWindow(
      window->dpy,
      root,
      0,0,
      w, h,
      1, DefaultDepth(window->dpy,window->screen),
      CopyFromParent,
      DefaultVisual(window->dpy,window->screen),
      valuemask, &attr
    );
    
  XSetWindowBackground(window->dpy,window->id,
      BlackPixel(window->dpy,window->screen));
  XSelectInput(window->dpy,window->id,
      ExposureMask|ButtonPressMask|ButtonReleaseMask|ButtonMotionMask);
  XMapWindow(window->dpy,window->id);
  
  return window;
}

mikaGlWindow::mikaGlWindow( int w, int h )
{
  #ifdef _GLDEBUG
    printf("mikaGlWindow::mikaGlWindow\n");
  #endif
  mw = w;
  mh = h;
  surface = new mikaGlSurface(mw,mh);
  window = lowWindowInit(mw,mh);
  gl = new mikaGl((void*)surface->pixels,mw,mh);
  delegate = NULL;
  #ifdef _GLTEST
    gl->setColor(67,67,67,190);
    gl->drawRoundRect(0,0,100,100,2);
    gl->drawRoundRect(80,80,180,180,2);
    gl->drawText(100,100,(unsigned char*)"hello world");
    mikaGlImage *testImage = new mikaGlImage((unsigned char*)"image.jpg");
    gl->drawImage(0,0,testImage);
  #endif
}

int mikaGlWindow::eventExpose( )
{
  static XImage *image = NULL;
  static cairo_t 		*cairo = NULL;
  static cairo_surface_t 	*csurface = NULL;
  static cairo_surface_t	*source = NULL;
  
  #ifdef _GLDEBUG
    printf("mikaGlWindow::eventExpose\n");
  #endif
  
  XGrabServer(window->dpy);
  csurface = cairo_xlib_surface_create(
                              window->dpy,
                              window->id,
                              DefaultVisual(window->dpy,0),
                              mw,
                              mh
                              );
  
  source = cairo_image_surface_create_for_data(
                              surface->pixels,
                              CAIRO_FORMAT_ARGB32,
                              mw,
                              mh,
                              4*mw
      );
  
  cairo = cairo_create(csurface);
  cairo_set_source_surface(cairo, source, 0, 0);
  cairo_paint(cairo);
  
  cairo_destroy(cairo);
  cairo_surface_destroy(csurface);
  cairo_surface_destroy(source);
  XUngrabServer(window->dpy);
  XFlush(window->dpy);
  
  XSync(window->dpy,0);
  
  return 0;
}

int mikaGlWindow::eventButtonDown( int x, int y, int id )
{
  #ifdef _GLDEBUG
    printf("mikaGlWindow::eventButtonDown\n");
  #endif
  struct eventButton ev;
  if (delegate)
  {
    if (delegate->onButtonDown)
    {
      ev.x = x;
      ev.y = y;
      ev.id = id;
      delegate->onButtonDown((void*)this,&ev);
    }
  }  
  return 0;
}

int mikaGlWindow::eventButtonRelease( int x, int y, int id )
{
  #ifdef _GLDEBUG
    printf("mikaGlWindow::eventButtonRelease\n");
  #endif
  struct eventButton ev;
  if (delegate)
  {
    if (delegate->onButtonRelease)
    {
      ev.x = x;
      ev.y = y;
      ev.id = id;
      delegate->onButtonRelease((void*)this,&ev);
    }
  }
  return 0;
}

int mikaGlWindow::eventLoopThreaded( )
{
  XEvent ev;
  while(1)
  {
    XNextEvent(window->dpy,&ev);
  
    switch(ev.type)
    {
      case ButtonPress:
        #ifdef _GLDEBUG
          printf("mikaGlWindow::eventLoopThreaded ButtonPress\n");
        #endif
        eventButtonDown(ev.xbutton.x,ev.xbutton.y,0);
        break;
      case ButtonRelease:
        #ifdef _GLDEBUG
          printf("mikaGlWindow::eventLoopThreaded ButtonRelease\n");
        #endif
        eventButtonRelease(ev.xbutton.x,ev.xbutton.y,0);
        break;
      case Expose:
        #ifdef _GLDEBUG
          printf("mikaGlWindow::eventLoopThreaded Expose\n");
        #endif
        eventExpose();
        break;
      default:
        break;
    }
  }
  return 0;
}

void *eventLoopThread(void*data)
{
  if (!data)
  {
    #ifdef _GLDEBUG
      printf("eventLoopThread error, invalid arguments\n");
    #endif
    return NULL;
  }
  #ifdef _GLDEBUG
    printf("eventLoopThread\n");
  #endif
  mikaGlWindow *glw = (mikaGlWindow*)data;
  glw->eventLoopThreaded();
  return NULL;
}

int mikaGlWindow::eventLoop( )
{
  pthread_t t;
  pthread_create(&t,NULL,eventLoopThread,this);
  return 0;
}

int mikaGlWindow::wait( )
{
  while(1)
  {
    sleep(1);
  }
  return 0;
}

mikaGlWindow::~mikaGlWindow( )
{
}

int mikaGlWindow::triggerExpose( )
{
  #ifdef _DEBUG
    printf("mikaGlWindow::triggerExpose\n");
  #endif
  
  return 0;
}
