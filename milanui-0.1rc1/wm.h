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

#define TWT_IMAGE		0
#define TWT_BUTTON		1
#define TWT_MENU		2
#define TWT_SCROLL		3
#define TWT_PANEL		4		// panel, best controling other parts of gui
#define TWT_CANVAS		5		// general visualisation widget
#define TWT_LBENTRY		6		// listbox item
#define TWT_TEXT		7		// multiline text widget
#define TWT_EDITABLE		8		// text input field
#define TWT_KEYBOARD		9		// simple keyboard

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
typedef struct tr_WidgetCanvas { } tr_WidgetCanvas;
typedef struct tr_WidgetKeyboard { } tr_WidgetKeyboard;

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

struct trop_List		*lstWindows;

int _set_gui_object_attr( struct gui_Widget *w, struct tr_Attr *attr );

int tr_WmInit( );
int tr_WmListWindows( );
struct tr_Window *tr_WindowMain( );
struct tr_Window *tr_WindowInit( struct tr_Window *w, struct tr_Window *parent,
  struct tr_Attr *attr );
int tr_WindowQueue( struct tr_Window *w );
int tr_WindowExpose( struct tr_Window *w );
struct tr_WindowEx *tr_WindowExInit( struct tr_WindowEx *w, struct tr_WindowEx *parent, struct tr_Attr *attr );
int tr_WindowExSetup( struct tr_WindowEx *w, char *filename );
int tr_WindowAddWidget( struct tr_Window *win, struct tr_Widget *wid );
int tr_WindowExAddObj( struct tr_WindowEx *w, struct tr_Object *o );
int tr_WindowExAddObjStatic( struct tr_WindowEx *w, struct tr_Object *o );
int tr_WindowMove( struct tr_Window *w, int x, int y );
int tr_WindowMoveBy( struct tr_Window *w, int x, int y );
int tr_WindowMap( struct tr_Window *w );
int tr_WindowUnmap( struct tr_Window *w );
int tr_WindowResize( struct tr_Window *win, int w, int h );
int tr_WindowToTop( struct tr_Window *w );
int tr_WindowExToTop( struct tr_WindowEx *w );
struct tr_Window *tr_WindowNamedGet( char *name );
struct tr_Window *tr_WindowExposable( );
int tr_WindowAddButton( struct tr_Window *w, struct tr_Object *button );

// layout
struct tr_Layout *tr_LayoutInit( struct tr_Layout *l, char *name );

// general object handling
int tr_ObjectRelease( struct tr_Object *o );
int tr_ObjectReleaseSub( struct tr_Object *o );

// general
int tr_SetCustomValue( struct tr_Object *parent, int value );
int tr_SetCustomPtr( struct tr_Object *parent, void *ptr );
int tr_SetRound( struct tr_Object *parent, int round );
BOOL tr_IsRound( struct tr_Object *parent );
int tr_SetColor( struct tr_Object *parent, char *color );
int tr_SetBgAlpha( struct tr_Object *parent, int value );
int tr_SetParent( struct tr_Object *o, struct tr_Object *parent );
int tr_SetName( struct tr_Object *o, char *name );
char *tr_GetName( struct tr_Object *o );
int tr_SetAction( struct tr_Object *parent, void *(*action)(void*data), int type );
int tr_SetActionEx( struct tr_Object *parent, void *(*action)(void*data), void *actiondata, int type );
int tr_SetLevel( struct tr_Object *o, int level );
int tr_SetPositionRelative( struct tr_Object *o, struct tr_Object *parent, double dx, double dy, double dw, double dh );
int tr_PutObject( struct tr_Object *parent, struct tr_Object *o ); // put a gui_Widget inside another gui_Widget
int tr_AddImage( struct tr_Object *parent, struct tr_Object *image );
int tr_SetTextEx( struct tr_Object *parent, char *str, int placement );
int tr_SetFontColor( struct tr_Object *o, unsigned char r, unsigned char g, unsigned char b, unsigned char a );

// button
struct tr_Object *tr_ButtonInitEx2( struct tr_Object *button, struct tr_Attr *attr, struct tr_Object *parent );
int tr_ButtonRelease( struct tr_Object *button );

// canvas
int tr_CanvasAddObj( struct tr_Object *can, struct tr_Object *obj );
int tr_CanvasAddObjStatic( struct tr_Object *can, struct tr_Object *obj );
int tr_CanvasRemoveAllObj( struct tr_Object *can );
int tr_CanvasRemoveAllObjRefType( struct tr_Object *can, int type );
struct tr_Object *tr_CanvasInit( struct tr_Object *can, struct tr_Attr *attr, struct tr_Window *parent );
int tr_CanvasSetMov( struct tr_Object *can, int movtype );

struct tr_Object *tr_LBItemInit( struct tr_Object *lbi, struct tr_Attr *attr, struct tr_Window *parent );

struct tr_Object *tr_TextInit( struct tr_Object *text, struct tr_Attr *attr,
  struct tr_Object *parent );

// menu
int tr_MenuSetGrid( struct tr_Object *menu, int gx, int gy );

// panel
struct tr_Object *tr_PanelInit( struct tr_Object *panel, struct tr_Attr *attr, struct tr_Window *parent );
struct tr_Object *tr_PanelInitEx( struct tr_Object *panel, struct tr_Attr *attr, struct tr_Window *parent, int pos );

// image
int tr_ImageChangeAlpha( struct tr_Object *image, int value );
struct tr_Object *tr_ImageInitEx( struct tr_Object *image, struct tr_Attr *attr, char *filename );
int tr_ImageRelease( struct tr_Object *image );
int tr_ImageSetRound( struct tr_Object *image, int value );
int tr_ImagePosCenter( struct tr_Object *image, struct tr_Object *parent );

// listbox entry
struct tr_Object *tr_LBEntryInit( struct tr_Object *lbi, struct tr_Attr *attr, struct tr_Window *parent );
int tr_LBEntrySetImage( struct tr_Object *lbi, struct tr_Object *image );
int tr_LBEntryConfigure( struct tr_Object *lbi, char *str_big, char *str_small );

// relay
int tr_RelayAdd( struct tr_Object *o, struct tr_Object *r );

// action handling
void *tr_ActionDo( void *data );

/**
 *  xmlsetup
 */
int _gui_setup_object_get_position( struct xml_Object *oo, int *x, int *y, int *w, int *h );

#ifdef __cplusplus
  }
#endif

#endif
