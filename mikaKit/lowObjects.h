#ifndef _LOW_OBJECTS_H_
#define _LOW_OBJECTS_H_

extern "C" {
  #include "interface.h"
  #include "common.h"
  #include "os.h"
  #include "system.h"
  #include "systemip.h"
  #include "global.h"
  #include "xml.h"
  #include "wm.h"
  #include "dom.h"
  #include "activityObject.h"
  #include "rootControllerSync.h"
  #include "basic.h"
}

typedef struct mikaSceneLow {
  struct tr_Window	*window;
  struct tr_Attr	attr;
} mikaSceneLow;

typedef struct mikaWindowLow {
  struct tr_Window	*window;
  struct tr_Attr	attr;
} mikaWindowLow;

typedef struct mikaCanvasLow {
  struct tr_Object	*canvas;
  struct tr_Window	*parentWindow;
  struct tr_Attr	attr;
} mikaCanvasLow;

typedef struct mikaWidgetLow {
  struct tr_Object	*widget;
  /** information incorporated in top level object, remove
   */
  struct tr_Object	*parentCanvas;
  struct tr_Attr	attr;
} mikaWidgetLow;

typedef struct mikaImageLow {
  struct tr_Object	*image;
  struct tr_Attr	attr;
} mikaImageLow;

#endif
