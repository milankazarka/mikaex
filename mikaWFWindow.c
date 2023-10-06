extern "C" {
  #include "interface.h"
  #include "common.h"
  #include "config.h"
  #include "os.h"
  #include "system.h"
  #include "systemip.h"
  #include "xml.h"
  #include "wm.h"
  #include "dom.h"
  #include "activityObject.h"
  #include "rootControllerSync.h"
  #include "basic.h"
}
#include "mikaWFWindow.h"

mikaWFWindow::mikaWFWindow( mikaWFWindow *_parent, struct mikaFrame *_frame )
{
  frame = *_frame;
  parent = _parent;
}

mikaWFWindow::~mikaWFWindow( )
{
}

void mikaWFWindow::moveToTop( )
{
}
