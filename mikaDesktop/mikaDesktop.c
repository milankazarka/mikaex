#include "mikaDesktop.h"

mikaDesktop *desktop = NULL;

mikaDesktop::mikaDesktop( struct mikaFrame *frame )
{
  if (desktop)
  {
    #ifdef _DEBUG
      printf("mikaDesktop::mikaDesktop error desktop already initialized\n");
    #endif
  }
  desktop = this;
  windows = NULL;
  
  sys = new sysDesktop();
}

mikaDesktop::~mikaDesktop( )
{
}

int mikaDesktop::addWindow( mikaWindow *window )
{
  if (!window)
  {
    #ifdef _DEBUG
      printf("mikaDesktop::addWindow error argument(s)\n");
    #endif
    return 1;
  }
  window->next = windows;
  windows = window;
  return 0;
}
