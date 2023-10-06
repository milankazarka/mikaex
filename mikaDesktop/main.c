#include "mikaDesktop.h"
#include "mikaWindow.h"
#include "mikaFrame.h"

int main( )
{
  struct mikaFrame frame;
  frame.x = 0;
  frame.y = 0;
  frame.w = 640;
  frame.h = 480;
  mikaDesktop *dDesktop = new mikaDesktop(&frame);
  if (dDesktop)
  {
    mikaWindow *dWindow = new mikaWindow(&frame,NULL);
    dWindow->show();
    dDesktop->addWindow(dWindow);
    dWindow->eventLoop();
  }
}
