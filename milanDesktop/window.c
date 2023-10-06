#include "window.h"
#include "config.h"
#include "canvas.h"

milanWindow *windows = NULL;

milanWindow::milanWindow( int w, int h )
{
  mw = w;
  mh = h;
  glWindow = new mikaGlWindow(mw,mh);
  glWindowDelegate.onButtonDown = onButtonDownShared;
  glWindowDelegate.onButtonRelease = onButtonUpShared;
  glWindow->delegate = &glWindowDelegate;
  next = windows;
  windows = this;
  currentCanvas = NULL;
}

milanWindow::~milanWindow( )
{
}

int milanWindow::eventLoop( )
{
  glWindow->eventLoop();
  glWindow->wait();
  return 0;
}

int milanWindow::triggerExpose( )
{
  #ifdef _DEBUG
    printf("milanWindow::triggerExpose\n");
  #endif
  milanCanvas *canvas = (milanCanvas*)currentCanvas;
  canvas->triggerExpose();
  glWindow->triggerExpose();
  return 0;
}

void *onButtonDownShared( void *windowData, struct eventButton *event )
{
  mikaGlWindow *glWindow = (mikaGlWindow*)windowData;
  milanWindow *current = windows;
  while(current)
  {
    if (current->glWindow==glWindow)
    {
      current->onButtonDown(event);
      break;
    }
    current = current->next;
  }
  return NULL;
}

void *onButtonUpShared( void *windowData, struct eventButton *event )
{
  mikaGlWindow *glWindow = (mikaGlWindow*)windowData;
  milanWindow *current = windows;
  while(current)
  {
    if (current->glWindow==glWindow)
    {
      current->onButtonUp(event);
      break;
    }
    current = current->next;
  }
  return NULL;
}

int milanWindow::onButtonDown( struct eventButton *event )
{
  if (!event)
  {
    #ifdef _DEBUG
      printf("milanWindow::onButtonDown error, invalid argument\n");
    #endif
    return 1;
  }
  #ifdef _DEBUG
    printf("milanWindow::onButtonDown\n");
  #endif
  if (currentCanvas)
  {
    milanCanvas *c = (milanCanvas*)currentCanvas;
      c->onButtonDown(event);
  }
  return 0;
}

int milanWindow::onButtonUp( struct eventButton *event )
{
  if (!event)
  {
    #ifdef _DEBUG
      printf("milanWindow::onButtonUp error, invalid argument\n");
    #endif
    return 1;
  }
  #ifdef _DEBUG
    printf("milanWindow::onButtonUp\n");
  #endif
  if (currentCanvas)
  {
    milanCanvas *c = (milanCanvas*)currentCanvas;
      c->onButtonUp(event);
  }
  return 0;
}

int milanWindow::addCanvas( object *nc )
{
  if (!nc)
  {
    return 1;
  }
  milanCanvas *c = (milanCanvas*)nc;
  c->next = (milanCanvas*)canvasses;
  canvasses = nc;
  return 0;
}

int milanWindow::pushCanvas( object *c )
{
  if (!c)
  {
    #ifdef _DEBUG
      printf("milanWindow::pushCanvas error, invalid argument\n");
    #endif
    return 1;
  }
  #ifdef _DEBUG
    printf("milanWindow::pushCanvas\n");
  #endif
  currentCanvas = c;
  milanCanvas *canvas = (milanCanvas*)c;
  
  return 0;
}
