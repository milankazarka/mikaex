/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "system.h"
#include "gui.h"
#include "xml.h"

  int gui_WindowConfigure(
    struct gui_Window *window,
    char *name
  )
  {
    if (!window) return -1;
    strcpy((char*)window->name,name);
    window->scene = gui_NewScene();
    window->widget = gui_NewWidget();
    return 0;
  }

  struct gui_Window *gui_WindowNew(
    struct gui_Window *window
  )
  {
    if (window==NULL)
    {
      window = (struct gui_Window*)malloc(sizeof(struct gui_Window));
    }
    window->scene = NULL;
    window->sceneex = NULL;
    window->name[0] = 0x00;
    window->widget = NULL;
    return window;
  }
