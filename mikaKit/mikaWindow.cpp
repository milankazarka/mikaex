/** Copyright Mika Limited
 *
 */
#include "mikaWindow.h"

#include "lowCommon.h"
#include "lowObjects.h"

mikaWindow::mikaWindow( struct mikaFrame *frame )
{
  #ifdef _DEBUG
    printf("mikaWindow::mikaWindow frame(%dx%d+%d+%d)\n",
      frame->w,frame->h,frame->x,frame->y);
  #endif
  myFrame = *frame;
  lowWindow = malloc(sizeof(struct mikaWindowLow));
  struct mikaWindowLow *dWindowLow = (struct mikaWindowLow*)lowWindow;
  dWindowLow->attr.x = myFrame.x;
  dWindowLow->attr.y = myFrame.y;
  dWindowLow->attr.w = myFrame.w;
  dWindowLow->attr.h = myFrame.h;
  /** initialize the Root Window of the application
   */
  dWindowLow->window = tr_WindowInit(NULL,NULL,&dWindowLow->attr);
}

mikaWindow::~mikaWindow( )
{
}

int mikaWindow::addScene( dhandle scene )
{
  if (!scene)
    return 1;
  mikaScene *dScene = (mikaScene*)scene;
  return 0;
}

int mikaWindow::eventLoop( )
{
  #ifdef _DEBUG
    printf("mikaWindow::eventLoop\n");
  #endif
  struct mikaWindowLow *dWindowLow = (struct mikaWindowLow*)lowWindow;
  tr_WindowQueue(dWindowLow->window);
  return 0;
}
