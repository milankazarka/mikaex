#ifndef _WMDEFINES_H_
#define _WMDEFINES_H_

#include "config.h"

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
int tr_ToggleVisibility( struct tr_Object *o, BOOL visibility );
int tr_SetCustomValue( struct tr_Object *parent, int value );
int tr_SetCustomPtr( struct tr_Object *parent, void *ptr );
int tr_SetRound( struct tr_Object *parent, int round );
BOOL tr_IsRound( struct tr_Object *parent );
int tr_SetColor( struct tr_Object *parent, char *color );
int tr_SetColor2( struct tr_Object *parent, char *color );
int tr_SetColorEx( struct tr_Object *parent, char *color );
int tr_ResetColor( struct tr_Object *parent );
int tr_SetBgAlpha( struct tr_Object *parent, int value );
int tr_SetParent( struct tr_Object *o, struct tr_Object *parent );
int tr_SetName( struct tr_Object *o, char *name );
char *tr_GetName( struct tr_Object *o );
int tr_SetAction( struct tr_Object *parent, void *(*action)(void*data), int type );
int tr_SetActionEx( struct tr_Object *parent, void *(*action)(void*data), void *actiondata, int type );
int tr_SetLevel( struct tr_Object *o, int level );
int tr_SetOverlay( struct tr_Object *o, int level );
int tr_SetPositionRelative( struct tr_Object *o, struct tr_Object *parent, double dx, double dy, double dw, double dh );
int tr_PutObject( struct tr_Object *parent, struct tr_Object *o ); // put a gui_Widget inside another gui_Widget
int tr_AddImage( struct tr_Object *parent, struct tr_Object *image );
int tr_SetTextEx( struct tr_Object *parent, char *str, int placement );
int tr_ChangeTextEx( struct tr_Object *parent, char *str, int placement );
int tr_ChangeTextExPos( struct tr_Object *parent, char *str, int x, int y );
int tr_SetFontColor( struct tr_Object *o, unsigned char r, unsigned char g, unsigned char b, unsigned char a );
int tr_SetFontColorEx( struct tr_Object *o, char *color );
int tr_SetFontSize( struct tr_Object *o, int size );

// button
struct tr_Object *tr_ButtonInitEx2( struct tr_Object *button, struct tr_Attr *attr, struct tr_Object *parent );
int tr_ButtonRelease( struct tr_Object *button );

struct tr_Object *tr_SlidebarInit( struct tr_Object *slidebar, struct tr_Attr *attr, struct tr_Object *parent );

// canvas
int tr_CanvasAddObj( struct tr_Object *can, struct tr_Object *obj );
int tr_CanvasAddObjStatic( struct tr_Object *can, struct tr_Object *obj );
/** exchange an object in canvas
 */
int tr_CanvasExchangeObjStatic( struct tr_Object *can, struct tr_Object *original, struct tr_Object *obj );
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
int tr_ImagePosLeft( struct tr_Object *image, struct tr_Object *parent );

// listbox entry
struct tr_Object *tr_LBEntryInit( struct tr_Object *lbi, struct tr_Attr *attr, struct tr_Window *parent );
int tr_LBEntrySetImage( struct tr_Object *lbi, struct tr_Object *image );
int tr_LBEntryConfigure( struct tr_Object *lbi, char *str_big, char *str_small );

struct tr_Object *tr_ListboxInit( struct tr_Object *lb, struct tr_Attr *attr, struct tr_Window *parent );

// relay
int tr_RelayAdd( struct tr_Object *o, struct tr_Object *r );

// action handling
void *tr_ActionDo( void *data );

#endif
