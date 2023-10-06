
/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _WM_H_
#define _WM_H_

#include "system.h"
#include "gui.h"
#include "xml.h"
#include "mikaSkin.h"

#define TWT_IMAGE		0
#define TWT_BUTTON		1
#define TWT_MENU		2
#define TWT_SCROLL		3
#define TWT_SLIDEBAR		31		// extended slidebar
#define TWT_PANEL		4		// panel, best controling other parts of gui
#define TWT_CANVAS		5		// general visualisation widget
#define TWT_LBENTRY		6		// listbox item
#define TWT_TEXT		7		// multiline text widget
#define TWT_EDITABLE		8		// text input field
#define TWT_KEYBOARD		9		// simple keyboard
#define TWT_LISTBOX		10		// listbox
#define TWT_LISTBOXITEM		11		// listbox item

#define TR_POS_TOP		0
#define TR_POS_BOTTOM		1
#define TR_POS_LEFT		2
#define TR_POS_RIGHT		3

#define TR_DEFA_BG		"#dfdfdf"	// default bg
#define TR_DEFA_WTBG		"#ffffff"	// default widget bg
#define TR_DEFA_BTBG		"#cccccc"	// default button 
#define TR_DEFA_NAMELEN		256		// default name length

#define TR_ACTION_NONE		0	
#define TR_ACTION_BUTTON	1		// button press (simple)
#define TR_ACTION_BUTTONDOWN	2
#define TR_ACTION_BUTTONUP	3

#define TR_GOID_ACTION		1		// custom goid of action data

#ifdef __cplusplus
  extern "C" {
#endif

typedef struct _tr_goid_action
{
  int		id;	
  char		*descr;
} _tr_goid_action;

static struct _tr_goid_action _tr_goid_action_list[1] = {
  {TR_GOID_ACTION, (char*)"TR_GOID_ACTION"}
};

enum {
  TR_ACTION_TYPE_NONE,
  TR_ACTION_TYPE_SCENE_SWITCH,
  TR_ACTION_TYPE_SYSTEM,
  TR_ACTION_TYPE_FUNCTION
};

typedef struct tr_ActionSceneSwitch
{
  char			*scene; 	// parent name
  void			*pscene; 	// ptr to scene (tr_WindowEx)
} tr_ActionSceneSwitch;

typedef struct tr_Action
{
  int			type; 		// TR_ACTION_TYPE_NONE
  void			*sub;
  void			*(*call)(void*data);
} tr_Action;

typedef struct tr_Attr
{
  int			x,
                        y,
                        w,
                        h;
  BOOL			bvisible;
  BOOL			btop;
} tr_Attr;

struct tr_Attr tr_GetAttr( int w, int h, int x, int y );

/**
 *  needs gui_ImageFreeContent to free pixel buffer
 */
typedef struct tr_WidgetImage
{
  gui_Image		sub;
} tr_WidgetImage;

typedef struct tr_WidgetButton
{
  char			text[TR_DEFA_NAMELEN];
} tr_WidgetLabel;

typedef struct tr_WidgetText { } tr_WidgetText;
typedef struct tr_WidgetListboxItem { } tr_WidgetListboxItem;
typedef struct tr_WidgetEditable { } tr_WidgetEditable;
typedef struct tr_WidgetMenu { } tr_WidgetMenu;
typedef struct tr_WidgetPanel { } tr_WidgetPanel;
typedef struct tr_WidgetScroll { } tr_WidgetScroll;
typedef struct tr_WidgetSlidebar { } tr_WidgetSlidebar;
typedef struct tr_WidgetCanvas { } tr_WidgetCanvas;
typedef struct tr_WidgetKeyboard { } tr_WidgetKeyboard;
typedef struct tr_Listbox { } tr_Listbox;

/**
 *  when making widgets, we use sub, for other objects we
 *  use wdata
 */
typedef struct tr_Widget
{
  struct gui_Widget	*sub;		 // sub-layer object
  struct tr_Attr	*attr;
  int			type;
  void			*(*onclick)(void*data); // simple
  void			*(*onbuttondown)(void*data);
  void			*(*onbuttonup)(void*data);
  void			*wdata;
} tr_Widget;

typedef struct tr_Object // todo - generalize other objects
{
  struct tr_Widget	widget;
} tr_Object;

typedef struct tr_Window
{
  struct gui_Widget	*sub;
  struct gui_Scene	*scene;	 // sub-layer object
  struct tr_Attr	*attr;
  struct tr_Window	*parent;
  struct trop_List	*widgets;
} tr_Window;

typedef struct tr_WindowEx // extended more complete window abstraction
{
  char			cfgfile[TR_DEFA_NAMELEN];
  struct tr_Window	*window;
  struct tr_Object	*canvas;
} tr_WindowEx;

typedef struct tr_Layout
{
  struct tr_WindowEx	*root;
  struct tr_WindowEx	*start;
  struct trop_List	*losalst;
} tr_Layout;

extern struct trop_List		*lstWindows;

int _set_gui_object_attr( struct gui_Widget *w, struct tr_Attr *attr );

#include "wmdefines.h"

/**
 *  xmlsetup
 */
int _gui_setup_object_get_position( struct xml_Object *oo, int *x, int *y, int *w, int *h );

#ifdef __cplusplus
  }
#endif

#endif
