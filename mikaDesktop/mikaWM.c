#include "mikaWM.h"
#include "mikaWMError.h"
#include "config.h"
#define _STATIC

#define ChildMask 	(SubstructureRedirectMask|SubstructureNotifyMask)
#define ButtonMask 	(ButtonPressMask|ButtonReleaseMask)
#define MouseMask 	(ButtonMask|PointerMotionMask)

mikaWMClient::mikaWMClient( Window window )
{
  myWindow = window;
}

mikaWMClient::~mikaWMClient( )
{
}

mikaWMRootWindow::mikaWMRootWindow( )
{
  
}

mikaWMRootWindow::~mikaWMRootWindow( )
{
}

mikaWM::mikaWM( )
{
  dpy = XOpenDisplay(NULL);
  if (dpy)
  {
    screen = DefaultScreen(dpy);
    root = RootWindow(dpy,screen);
  }
  else
  {
    #ifdef _DEBUG
      printf("mikaWM::mikaWM error, couldn't open display\n");
    #endif
  }
  
  clients = NULL;
  
  XSetErrorHandler(mikaErrorHandler);
  initEnvironment();
  eventLoop();
}

mikaWM::~mikaWM( )
{
}

int mikaWM::initEnvironment( )
{
  #ifdef _DEBUG
    printf("mikaWM::initEnvironment\n");
  #endif
  XSetWindowAttributes attr;
  XGCValues gv;
  attr.event_mask = ChildMask | ButtonMask | PropertyChangeMask;
  XChangeWindowAttributes(dpy,root,CWEventMask,&attr);
  return 0;
}

void mikaWM::eventLoop( )
{
  XEvent ev;
  for(;;)
  {
    XNextEvent(dpy,&ev);
    switch(ev.type)
    {
      case Expose:
        eventExpose(&ev);
        break;
      case CreateNotify:
        eventCreateNotify(&ev);
        break;
      case ReparentNotify:
        break;
      case ConfigureRequest:
        eventConfigureRequest(&ev);
        break;
      case ConfigureNotify:
        break;
      case MapRequest:
        break;
      case UnmapNotify:
        break;
    }
  }
}

int mikaWM::eventExpose( XEvent *ev )
{
  #ifdef _DEBUG
    printf("mikaWM::eventExpose\n");
  #endif
  if (!ev)
    return 1;
  return 0;
}

int mikaWM::eventCreateNotify( XEvent *ev )
{
  #ifdef _DEBUG
    printf("mikaWM::eventCreateNotify\n");
  #endif
  if (!ev)
    return 1;
  return 0;
}

int mikaWM::eventConfigureRequest( XEvent *ev )
{
  #ifdef _DEBUG
    printf("mikaWM::eventConfigureRequest\n");
  #endif
  if (!ev)
    return 1;
  return 0;
}

int mikaWM::addClient( Window window )
{
  #ifdef _DEBUG
    printf("mikaWM::addClient\n");
  #endif
  
  mikaWMClient *client = new mikaWMClient(window);
  client->next = clients;
  clients = client;
  
  return 0;
}

/** retrieve the mikaWMClient object holding a Window
 */
mikaWMClient *mikaWM::clientOfWindow( Window window )
{
  mikaWMClient *current = clients;
  while(current)
  {
    if (current->myWindow==window)
      return current;
    current = current->next;
  }
  return NULL;
}

/** retrieve the mikaWMClient object by the parent Window
 */
mikaWMClient *mikaWM::clientOfWindowByParent( Window parent )
{
  mikaWMClient *current = clients;
  while(current)
  {
    if (current->parent==parent)
      return current;
    current = current->next;
  }
  return NULL;
}

#ifdef _STATIC
  int main( )
  {
    mikaWM *wm = new mikaWM();
    return 0;
  }
#endif
