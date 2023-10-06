/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _GUI_BOUNDS_H_
#define _GUI_BOUNDS_H_

#include "gui.h"

typedef struct _gui_bound
{
    int     x;
    int     y;
    int     w;
    int     h;
} _gui_bound;

BOOL gui_BoundsWidgetDraw( struct gui_Widget *widget, struct _gui_bound *_bound );

#endif
