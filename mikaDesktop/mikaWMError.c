#include "mikaWMError.h"
#include "config.h"

#include <X11/Xproto.h>

int mikaErrorHandler( Display *dpy, XErrorEvent *event )
{
  #ifdef _DEBUG
    printf("mikaWM mikaErrorHandler\n");
  #endif
  
  if (
    event->request_code == X_ChangeWindowAttributes &&
    event->error_code == BadAccess )
  {
    #ifdef _DEBUG
      printf("mikaWM another window manager is already running\n");
    #endif
    exit(2);
  }
  
  return 0;
}
