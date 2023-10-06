#include "rootWmInterface.h"
#include "rootWm.h"
#include "rootFrame.h"
#include "config.h"

extern "C" int rootWmInterfaceInit( )
{
  wmInstance = new rootWm( );
  return 0;
}

extern "C" int rootWmGlobalInit( )
{
  return rootWmInterfaceInit();
}

extern "C" int rootWmInterfaceSetFrame( struct rootFrame *frame )
{
  if (!wmInstance)
  {
    #ifdef _DEBUG
      printf("rootWmInterfaceSetFrame error WM not initialized\n");
    #endif
    return 1;
  }
  
  if (!frame)
  {
    #ifdef _DEBUG
      printf("rootWmInterfaceSetFrame error invalid arguments\n");
    #endif
    return 1;
  }
  return 0;
}

/** add a new window to the WM holding a low level object
 */
extern "C" int rootWmInterfaceAddWindow( void *lowObject )
{
  if (!lowObject)
  {
    return 1;
  }
  rootWmWindow *window = new rootWmWindow();
  window->lowObject = lowObject;
  wmInstance->addWindow(window);
  return 0;
}

extern "C" int rootWmInterfaceMvWindow( void *lowObject, struct rootFrame *frame )
{
  if (!lowObject)
  {
    return 1;
  }
  rootWmWindowHolder *holder = wmInstance->windowHolderByLowObject(lowObject);
  holder->window->setFrame(frame);
  return 0;
}
