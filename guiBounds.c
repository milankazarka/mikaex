/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "guiBounds.h"

/**
 *
 *    pass widget to draw and get drawing bounds
 *    returns TRUE if the widget gets drawn at all
 *
 */
BOOL gui_BoundsWidgetDraw( struct gui_Widget *widget, struct _gui_bound *_bound )
{
    struct gui_Widget *parent = widget->parent;
    
    if (
        widget->attr.x >= parent->attr.w ||
        widget->attr.y >= parent->attr.h ||
        widget->attr.x+widget->attr.w <= 0 ||
        widget->attr.y+widget->attr.h <= 0
    )
    {
        return FALSE;
    }
    return TRUE;
}
