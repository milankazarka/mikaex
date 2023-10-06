#include "sysDesktop.h"
#include "config.h"

sysDesktop::sysDesktop( )
{
  dpy = XOpenDisplay(NULL);
  screen = DefaultScreen(dpy);
  root = RootWindow(dpy,screen);
  XGetWindowAttributes(dpy,root,&attr);
  
  int event_base;
  int error_base;
  if ( XCompositeQueryExtension(dpy, &event_base, &error_base ) )
  {
    #ifdef _DEBUG
      printf("sysDesktop::sysDesktop compositing possible\n");
    #endif
  }
  else
  {
    #ifdef _DEBUG
      printf("sysDesktop::sysDesktop compositing not possible\n");
    #endif
  }
}

sysDesktop::~sysDesktop( )
{
}
