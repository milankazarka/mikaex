#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mikaGlWindow.h"
#include "mikaGlConfig.h"

void *onButtonDown( void *windowData, struct eventButton *event )
{
  #ifdef _GLDEBUG
    printf("onButtonDown\n");
  #endif
  mikaGlWindow *window = (mikaGlWindow*)windowData;
  return NULL;
}

void *onButtonUp( void *windowData, struct eventButton *event )
{
  #ifdef _GLDEBUG
    printf("onButtonUp\n");
  #endif
  mikaGlWindow *window = (mikaGlWindow*)windowData;
  return NULL;
}

int main( )
{
  struct mikaGlWindowDelegate delegate;
  mikaGlWindow *window = new mikaGlWindow(640,480);
  delegate.onButtonDown = onButtonDown;
  delegate.onButtonRelease = onButtonUp;
  window->delegate = &delegate;
  window->eventLoop();
  window->wait();

  return 0;
}
