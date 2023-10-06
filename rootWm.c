#include "rootWm.h"
#include "config.h"

rootWm *wmInstance = NULL;

rootWmWindow::rootWmWindow( )
{
}

rootWmWindow::~rootWmWindow( )
{
}

int rootWmWindow::setFrame( struct rootFrame *_frame )
{
  if (!_frame)
  {
    #ifdef _DEBUG
      printf("rootWm::setFrame error\n");
    #endif
    return 1;
  }
  return 0;
}

rootWm::rootWm( )
{
  wmInstance = this;
  windows = NULL;
}

rootWm::~rootWm( )
{
}

int rootWm::setFrame( struct rootFrame *_frame )
{
  if (!_frame)
  {
    #ifdef _DEBUG
      printf("rootWn::setFrame error invalid arguments\n");
    #endif
    return 1;
  }
  frame = *_frame;
  return 0;
}

int rootWm::addWindow( rootWmWindow *window )
{
  if (window)
  {
    #ifdef _DEBUG
      printf("rootWm::addWindow error invalid arguments\n");
    #endif
    return 1;
  }
  
  struct rootWmWindowHolder *holder = (struct rootWmWindowHolder*)malloc(sizeof(struct rootWmWindowHolder));
  holder->window = window;
  holder->next = windows;
  windows = holder;
  
  return 0;
}

/** retrieve a rootWmWindowHolder taking care of a low level object 'lowObject'
 */
struct rootWmWindowHolder *rootWm::windowHolderByLowObject( void *lowObject )
{
  if (!lowObject)
  {
    #ifdef _DEBUG
      printf("rootWm::windowHolderByLowObject error invalid arguments\n");
    #endif
    return NULL;
  }

  struct rootWmWindowHolder *holder = NULL;
  struct rootWmWindowHolder *current = windows;
  rootWmWindow *window = NULL;
  while(current)
  {
    window = current->window;
    if (window->lowObject==lowObject)
    {
      holder = current;
      break;
    }
    current = current->next;
  }
  
  return holder;
}

int rootWm::update( )
{
  
  return 0;
}
