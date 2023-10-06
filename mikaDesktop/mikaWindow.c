#include "mikaWindow.h"

mikaWindow::mikaWindow( struct mikaFrame *frame, mikaWindow *parent )
{
  next = NULL;
  myFrame = *frame;
  myParent = parent;
  
  sysWindow *sysParent = NULL;
  if (parent)
    sysParent = parent->sys;
    
  sys = new sysWindow(frame,sysParent);
}

mikaWindow::~mikaWindow( )
{
}

void mikaWindow::show( )
{
  sys->show();
}

void mikaWindow::eventLoop( )
{
  sys->eventLoop();
}
