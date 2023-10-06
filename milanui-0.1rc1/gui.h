/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _GUI_C_
#define _GUI_C_

#include "system.h"
#include "agg.h"
#include "xml.h"
#include "csv.h"
#include "guiBounds.h"

#ifdef __cplusplus
    extern "C" {
#endif

#define MSG_WBADLEVEL	-2	        // initializing widget not according to hierarchy

#define DEFA_FTSIZE	18	        // default font size
#define DEFA_FT		"fonts/Vera.ttf" 	// default font
#define DEFA_FTBG	"30/30/30"		// default bg font color
#define DEFA_FTFG	"255/255/255"		// default ft font color
#define DEFA_CSBG	"#dfdfdf"		// default canvas color
#define DEFA_LBEBG	"#ffffff"		// default listbox entry color
#define DEFA_CSMAR	5			// default canvas margin
#define DEFA_CSMARRL	90			// default canvas margin right,left
#define DEFA_CSMARTB	5			// default canvas margin top,bottom
#define DEFA_TXTR	2			// text border round
#define DEFA_TXTCACHE	256			// text cache
#define DEFA_ROUND	2			// default round edge value
#define C_TRANS_VAL	0xff00cc		// transparent color, ignored when drawing
#define C_TRANS_VALR	0x00			// transparency
#define C_TRANS_VALG	-2			// gui transparency - underlying colors shaded
#define _C_R		1			// red position
#define _C_G		2			// green position
#define _C_B		3			// blue position
#define C_R		_C_R
#define C_G		_C_G
#define C_B		_C_B

enum {
    GO_STATIC,			// static gui object
    GO_DYNAMIC			// managed dynamic gui object
};
  
#define GOID_IMAGE	1	// widget objects
#define GOID_TEXT	2
#define GOID_TEXTSIM	200	// simple text
#define GOID_TEXTSIMB	201	// simple text with better possibilities
#define GOID_TEXTSIMC	202	// simple text with better geometry handling
#define GOID_TEXTSIMD 203	// for multiline text widgets
#define GOID_TROP	3
#define GOID_COLOR	4
#define GOID_WIDGET	5	// child widget
#define GOID_SCENE	6
#define GOID_BGALPHA	7	// background alpha 'possibly use instead of color'
#define GCFG_ROUND	1000	// roundness
#define GCFG_GRADIENT	2000	// gradient
#define GCFG_STR	3000	// string as custom data in gui object
#define GOID_MSG	8	// message associated with a widget
#define GOID_CUSTOM	9	// custom object 'for example speciffic event data'
#define GOID_POLY     100       // polygon gui object

typedef struct _goid_id_descr
{
    int		id;		// main id
    int		myid;		// for implementation speciffic GOID's
    char	*descr;
} _goid_id_descr;
  
static struct _goid_id_descr _goid_id_descr_pool[15] = {
    { GOID_IMAGE, 	0,	(char*)"goid_image" },
    { GOID_TEXT, 	0,	(char*)"goid_text" },
    { GOID_TEXTSIM, 	0,	(char*)"goid_textsim" },
    { GOID_TEXTSIMB, 	0,	(char*)"goid_textsimb" },
    { GOID_TEXTSIMC,	0,	(char*)"goid_textsimc" },
    { GOID_TEXTSIMD,	0,	(char*)"goid_textsimd" },
    { GOID_TROP,	0,	(char*)"goid_trop" },
    { GOID_COLOR,	0,	(char*)"goid_color" },
    { GOID_WIDGET,	0,	(char*)"goid_widget" },
    { GOID_SCENE,	0,	(char*)"goid_scene" },
    { GOID_BGALPHA,	0,	(char*)"goid_bgalpha" },
    { GCFG_ROUND,	0,	(char*)"gcfg_round" },
    { GCFG_GRADIENT,	0,	(char*)"gcfg_gradient" },
    { GOID_MSG,		0,	(char*)"goid_msg" },
    { GOID_CUSTOM,	0,	(char*)"goid_custom" }
};
  
//int _goid_id_descr_add_id( int myid, char *descr ); 	// return goid
int _goid_id_descr_at_myid( int myid ); 		// return goid of myid (-1 on false)

#define GREL_EXPOSE	0	// relay events
#define GREL_BDOWN	1
#define GREL_BREL	2

#define FORM_NONE	0	// forms logic of widgets
#define FORM_SIMPLE	99	// simple form with general data and cfg
  #define FORM_MENU	1
  #define FORM_MOVABLE	2
  #define FORM_BUTTON	3
  #define FORM_SLIDEBAR	4
  #define FORM_OVERLAY	5
  #define FORM_TITLEBAR	6
  #define FORM_PROGRESS	7
  #define FORM_CANVAS	8
  #define FORM_LBENTRY	9	// listbox entry 'canvas can be used as a listbox'
  #define FORM_KEYBOARD 10	// simple keyboard widget
  #define FORM_TEXT	11	// multiline text
  #define FORM_EDITABLE	12	// text entry
  #define FORM_PANEL	13	// static panel to which controls can be applied
  #define FORM_DRAWABLE	14	// special content widget (drawn by vector API's)
  #define FORM_FRAME	15	// frame can hold a surface

  #define FEDITABLE_LEN	256
  #define GUIMSGBIT_LEN 256	// max length of a bit of an xml message

  #define DUP		0
  #define DDOWN		1

  enum {
    ROT_NONE,
    ROT_LEFT,
    ROT_RIGHT,
    ROT_DOWN
  };

  #define PLACE_NONE	-1
  #define PLACE_UP	0
  #define PLACE_DOWN	1
  #define PLACE_LEFT	2
  #define PLACE_RIGHT	3
  #define PLACE_CENTER	4
  #define PLACE_FILL	5

  #define IMG_XPM	0
  #define IMG_JPG	1

  #define IID_UP	"sbar_up"	// deprecated, remove
  #define IID_DOWN	"sbar_down"

  #define FIMG_AUP	"arrow_up.xpm"
  #define FIMG_ADOWN	"arrow_down.xpm"

  #define REC_ENABLE	0

enum {
    _COLOR_MODE_RGBA8888
};

typedef struct gui_SVec 	// speed vector
{
    short	l;		// length
    long	t;		// time
} gui_SVec;

typedef struct gui_Position
{
    int		x;
    int		y;
    int		w;
    int		h;
} gui_Position;

typedef struct gui_Grid
{
    int		x;
    int		y;
    int		w;
    int		h;
    int		step;
    int		move;
    int		begin;
    void	*child;
} gui_Grid;

typedef struct gui_Motion
{
    time_t	t;
    int		x;
    int		y;
} gui_Motion;

/**
 *  levels are the most basic widget hierarchy
 *  used in canvas
 */
#define GUI_LEVELS	10
#define GUI_LEVEL_MIN	0
#define GUI_LEVEL_MAX	9

typedef struct gui_WidgetAttributes
{
    int			state;	// id parsed between handling of widget
    BOOL		bvisible;
    BOOL		bvirtual;
    int			w;
    int			h;
    int			x;
    int			y;
    int			vw;	// virtual ( for ex. 'scroll region' )
    int			vh;
    int			vx;
    int			vy;
    int			level;
    int			ftsize;	// font size
    struct gui_Motion 	motion; // last motion event
    int			form;
    void		*(*onclick)(void*data); // deprecated
    void		*(*onbuttondown)(void*data);
    void		*(*onbuttonup)(void*data);
    void                *(*onclickEx)( struct gui_Event *e ); // widget id in event
    void		*(*onover)(void*data);
    void		*(*onmotion)(void*data);
    void		*(*onexpose)(void*data);
    void                *(*onexposeEx)( void*obj, int x, int y, int w, int h );
    struct trop_Action	*aonclick; // actions are client/server orientated events
    int			hierarchy;
    char		name[DEFA_TXTCACHE]; // widget id
} gui_WidgetAttributes;

typedef struct gui_object
{
    int			id;
    unsigned int	value;	// possible value, otherwise data ptr
    void		*data;
    struct gui_object	*next;
} gui_object;

/**
 *  released through gui_ImageRelease
 */
typedef struct gui_Image
{
    char		name[64];
    char		filename[128];
    unsigned char       *data;
    int			x;	// x location in holder/parent
    int			y; 
    int			w;	// side
    int			h;
    int			round;	// round edges
    int			type;
} gui_Image;

/**
 *  released in gui_WidgetReleaseEx
 */
typedef struct gui_ColorLow	// color handling without system
{				// dependant GC routines
    unsigned long	value;
    unsigned char	r;
    unsigned char	g;
    unsigned char	b;
    unsigned char	a;
} gui_ColorLow;

/**
 *  released in gui_WidgetReleaseEx
 */  
typedef struct gui_GC
{
    gui_ColorLow	*colorl;
} gui_GC;

typedef struct gui_Surface
{
    char		*data;
    struct gui_Image	*image;
} gui_Surface;

typedef struct gui_Pixel
{
    struct gui_Color	*color;
    struct gui_Pixel	*next;
} gui_Pixel;
  
typedef struct gui_PixBuf	// pixel buffer, framebuffer abstraction 01
{
    int			w, h;
    struct gui_Pixel	*pixel;
} gui_PixBuf;

/**
 *  mika: standard inter-widget communication: 
 */
typedef struct gui_Calls
{
    void		*(*my)(void*data);
} gui_Calls;

typedef struct gui_Font
{
    struct gui_GC	gc;
} gui_Font;

typedef struct gui_Widget
{
    struct gui_Calls	            calls;
    long                            id;
    struct gui_Surface	            *_surface;
    struct gui_WidgetAttributes     attr;
    struct trop_Atom	            *queue;
    struct gui_Widget	            *parent;
    struct gui_Widget	            *next;
    struct gui_object	            *obj;
    struct trop_List	            *relay; // type (trop_Relay) relay some messages to other objects
    struct gui_GlFont               *glfont;
    void		            *form;
    int			            otype; // GO_DYNAMIC, GO_STATIC
    struct trop_DataModel	    *dataModel;
} gui_Widget;

/**
 *
 *  current geometry bounds in which drawing occurs
 *
 */
typedef struct gui_Bounds
{
  int			x;
  int			y;
  int			width;
  int			height;
} gui_Bounds;

  #define CAN_VRMOVE	7	// number of saved canvas movement vectors
  
  enum {
    CAN_NONE,
    CAN_OBJ,			// standard object
    // virtual is used when we only have a predefined number of
    // canvas objects of which we only change content as we slide
    CAN_VIRTUAL,                // virtual object being a holder of a canvas managed gui object
    CAN_BR			// canvas newline
  };
  
  typedef struct _can_obj
  {
    struct gui_Position	pos;
    BOOL		bstatic;	// static, non movable object
    unsigned short int	type;
    struct gui_Widget	*widget;
    struct _can_obj	*next;
  } _can_obj;
  
  enum {
    CAN_MOV_SLIDE,
    CAN_MOV_SCROLL,
    CAN_MOV_NONE
  };
  
  typedef struct _can_content
  {
    int			fillx;
    int			filly;
    int			id; // id of the content, possibly in a content pool
    int			bottomy;
    int			movey;
    int			w;
    int			h;
    int			vrmove[CAN_VRMOVE]; // last movement vectors
    int			svrmove[CAN_VRMOVE]; // last speed vectors
    struct _can_obj	*can_pool;
  } _can_content;
  
  typedef struct form_Canvas
  {
    int			fillx;
    int			filly;
    int			margin_l;
    int			margin_r;
    int			margin_top;
    int			margin_bot;
    int			bottomy;
    int			movey;
    int			movetype;	// slide, scroll, none
    struct gui_Widget	*wup;
    struct gui_Widget	*wdown;
    int			w;
    int			h;
    int			vrmove[CAN_VRMOVE]; // last movement vectors
    struct gui_SVec	svrmove[CAN_VRMOVE]; // last speed vectors
    struct _can_obj	*can_pool;
    // mika - todo, make methods handling _can_content
    struct _can_content	*content; // separating the data with the data holder
    struct gui_Surface	*surface; // possible surface
  } form_Canvas;
  
  struct gui_Scene
  {
    int 		id;
    struct trop_List	*list;
  } gui_Scene;
  
  struct gui_SceneEx
  {
    int			id;		// internaly id's should be used
    char		name[128];	// configured based on scene name
    struct trop_List	*list;
  } gui_SceneEx;
  
  struct gui_Window
  {
    struct gui_Scene	*scene;
    struct gui_SceneEx	*sceneex;
    char		name[DEFA_TXTCACHE];
    struct gui_Widget	*widget;
  } gui_Window;
  
  struct _gui_layout_link
  {
    char		from[64];
    char		to[64];
  } _gui_layout_link;
  
  struct _gui_layout_widget
  {
    char		name[64];
  } _gui_layout_widget;
  
  struct gui_Layout			// inner representation of gui layout (gui scripting)
  {
    char		name[64];
    struct gui_Layout	*parent;
    struct trop_List	*links;
    struct trop_List	*widgets;
  } gui_Layout;
  
  struct gui_Run
  {
  //  todo
  } gui_Run;
  
  /*
    saving a structure describing how to reconstruct an event
    when doing a relay to a different system, or when recording 
  */
  enum {
    REC_EXP_NONE,
    REC_EXP_RECT,
    REC_EXP_TXT,
    REC_EXP_IMG
  };
  
  struct _rec_expose_rect
  {
    int				x;
    int				y;
    int				w;
    int				h;
    int				round;
    unsigned long		pixel;
  } _rec_expose_rect;
  
  struct _rec_expose_txt
  {
    int				x;
    int				y;
    int				w;
    int				h;
    char			str[256];
  } _rec_expose_txt;
  
  struct _rec_expose_bit
  {
    int				type;
    void			*sub;
    struct _rec_expose_bit	*next;
  } _rec_expose_bit;
  
  struct rec_Expose
  {
    struct _rec_expose_bit	*bits;
  } rec_Expose;
  
  #define GUI_STR_LEN	32
  typedef struct gui_Str
  {
    char		data[GUI_STR_LEN];
  } gui_Str;
  
  /**
   *  text objects freed through gui_WidgetReleaseEx
   */
  typedef struct gui_Text
  {
    struct gui_Font	*font;
    char		*str;
    int			len;
    struct gui_Position	pos;
  } gui_Text;
  
  typedef struct gui_TextSim
  {
    char		*str;
    int			placement;
    struct gui_Position	pos;
  } gui_TextSim;
  
  typedef struct _gui_cache_text
  {
    int			len;
    char		*str;
  } _gui_cache_text;
  
  typedef struct _gui_cache
  {
    gui_Surface		*surface;
    void		*sub;
  } _gui_cache;
  
  enum {
    GMSG_NONE,
    GMSG_NETWORK,		// msg send to remote listener
    GMSG_LOCAL
  };
  
  typedef struct gui_MsgNetwork
  {
    struct trop_Conn	*conn;
  } gui_MsgNetwork;
  
  typedef struct gui_Msg	// messaging on actions
  {
    int			type;
    void		*descr;	// msg descriptor
    char		*str;
  } gui_Msg;
  
  typedef struct gui_Relay
  {
    struct gui_Widget	*wi;	// we can relay an event to a widget
    struct gui_Msg	*msg;	// or/and relay an event through a msg
  } gui_Relay;

  /**
   *  deprecated, should not be used
   */
  typedef struct gui_Draw
  {
    int			x;
    int			y;
    int			w;
    int			h;
    int			round;
    int			gradient;
    struct gui_GC	*gc;
    struct gui_Surface	*surface;
  } gui_Draw;
  
  enum {
    GEVENT_EXPOSE,
    GEVENT_BPRESS,
    GEVENT_BRELEASE,
    GEVENT_MOTION,
    GEVENT_KEY,
    GEVENT_NONE
  };
  
  typedef struct gui_Event
  {
    struct gui_Widget	*widget;
    void		*sub;	// sub binding
    int			type;
    int 		x;
    int			y;
    int			w;
    int			h;
    char		key;	// on ASCII keyboard we define this directly
  } gui_Event;
  
  typedef struct _drawable_object
  {
  } _drawable_object;
  
  typedef struct form_Drawable
  {
    struct trop_List	*lst_objs;
  } form_Drawable;
  
  /**
   *  a frame has it's own surface, which it manipulates
   *
   */
  typedef struct form_Frame
  {
    struct gui_Surface	*surface;
  } form_Frame;
  
  typedef struct form_Simple
  {
    char		name[256];
    char		long_desc[32768];
    char		short_desc[32768];
    struct gui_Image	*pimage;
    struct gui_Image	image;
  } form_Simple;
  
  /**
   *  deprecated, only canvas should be used
   */
  typedef struct form_Menu
  {
    int			xfill;
    int			yfill;
    int			ymove;
    int			xmove;
    int			grid_ew;
    int			grid_eh;
    struct trop_List	*grid;
  } form_Menu;
  
  typedef struct form_Slidebar
  {
    struct gui_Widget	*relay;
  } form_Slidebar;

  typedef struct form_Overlay
  {
  } form_Overlay;
  
  typedef struct form_Progress
  {
    int 		high;	// 0-high
    int			value;
  } form_Progress;

  /**
   *  released by form_LBEntryRelease
   *  image not released with form_ListboxEntry
   */
  typedef struct form_ListboxEntry
  {
    char		*str_big;
    char		*str_small;
    struct gui_Image	*image;
  } form_ListboxEntry;
  
  typedef struct form_Text
  {
    int			margin;
    struct trop_List	*texts;
  } form_Text;
  
  typedef struct form_Editable
  {
    int			vlen;
    char		value[FEDITABLE_LEN];
  } form_Editable;

extern struct gui_Widget *wCurrent;

/**
 *  adding sub-objects to the Widget object:
 */
int _goid_id_descr_add_id( int myid, char *descr );
int _goid_id_descr_by_myid( int myid );
int gui_WidgetAddCustomGobj( struct gui_Widget *w, int goid, void *obj, int value );
void *gui_WidgetGetObjAtGoid( struct gui_Widget *w, int goid );

unsigned long _hex2long( unsigned char *str );
int _hex2unsignedChar( unsigned char *atr, unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a );
unsigned char *_long2hex( unsigned long value, unsigned char *hex );
int _hex_move_up( unsigned char *hex );
int _hex_move_up_ex( unsigned char *hex );
int _hex_move_down( unsigned char *hex );
int _hex_move_down_at( unsigned char *hex, int at );

int gui_Flush( );
 
int __x_flush( );
int __x_draw_pixel( struct gui_Widget *main, struct gui_GC *gc, int x, int y );

void _transform_coord( int *x, int *y );
int gui_GeomGetFReeSpace( int w, int h, int *x, int *y, int *x2, int *y2 );

int gui_SetCursor( struct gui_Widget *w, char *name );

int gui_RGB( int which );
int gui_RED( );
int gui_GREEN( );
int gui_BLUE( );
int gui_ParseColorValue( char *value, int *r, int *g, int *b );
int gui_DrawRectangle( struct gui_GC *gc, int x, int y, int w, int h );
int gui_DrawRectangleExA( struct gui_Draw *draw );
int gui_DrawRoundRectangle( struct gui_GC *gc, int x, int y, int w, int h, int val );
int gui_DrawRoundRectangleSurface( struct gui_GC *gc, struct gui_Surface *surface, int x, int y, int w, int h, int val );
int gui_DrawRoundRectangleGradient( struct gui_GC *gc, int x, int y, int w, int h, int val, int grad );
int gui_DrawRoundRectangleSurfaceGradient( struct gui_GC *gc, struct gui_Surface *surface, int x, int y, int w, int h, int val, int grad );
int gui_DrawText( char *str, int x, int y, int w, int h );
int gui_DrawTextEx( char *str, int x, int y, int w, int h, int round );
int gui_DrawTextCLX( struct gui_TextSim *ts, int x, int y, int w, int h, int round );
int gui_SurfaceMoveAlpha( int x, int y, int w, int h, int value );
int gui_DrawImageSurface( struct gui_Image *image, struct gui_Surface *surface, int x, int y, int w, int h );
int gui_DrawImageEx( struct gui_Image *image, int x1, int y1, int x2, int y2, int w, int h );
int gui_DrawImageRound( struct gui_Image *image, int x1, int y1, int x2, int y2, int w, int h, int val );

int _gui_widget_set_goid( struct gui_Widget *w, int type, void *value, int nvalue );
struct _gui_cache *_gui_cache_text_init( struct _gui_cache *cache, struct gui_Surface *surface_from, int x, int y, int w, int h, char *str );
struct _gui_cache *_gui_cache_init( struct _gui_cache *cache, struct gui_Surface *surface_from, int x, int y, int w, int h );
int _gui_cache_text_add( int x, int y, int w, int h, char *str );
  
struct gui_Draw *gui_DrawInit( struct gui_Draw *draw );
  
int gui_Init( );
int gui_InitSkin( struct xml_Inst *xml );

struct gui_Relay *gui_WidgetNextRelay( struct gui_Widget *wi );

void *_test_jpeg_handling( char *filename );
struct gui_Image *_gui_jpeg_to_image( struct gui_Image *i );
struct gui_Image *gui_ImagePick( char *name ); // pick image from list, by name
int gui_ImageRegister( char *filename, char *name, int x, int y );
int gui_ImageRegisterSimple( char *filename, char *name );
int gui_ImageMoveAlpha( struct gui_Image *image, int at, int value );
int gui_ImageSetRound( struct gui_Image *image, int round );
struct gui_Image *gui_ImagePosCenter( struct gui_Image *image, struct gui_Widget *wi ); // center (x/y) to widget

int gui_TextGetSize( char *str, int *strw, int *strh );

struct gui_Surface *gui_SurfaceInit( struct gui_Widget *main, struct gui_Surface *surface, int w, int h );
int gui_SurfaceRotate( struct gui_Surface *surface, int angle );

int form_AddButton( struct gui_Widget *w, struct gui_Widget *b );

int gui_TriggerExpose( struct gui_Widget *main );

void *_gui_WidgetQueue( void *data );
BOOL gui_WidgetIsViewable( struct gui_Widget *w );
struct gui_Widget *gui_GetMain( );
struct gui_Widget *gui_WidgetAt( int x, int y );
int gui_WidgetMap( struct gui_Widget *w );
//int gui_WidgetToTop( struct gui_Widget *w );
int gui_WidgetUnTop( struct gui_Widget *w );
int gui_WidgetRemoveAllObj( struct gui_Widget *w );
struct gui_Widget *gui_NewWidget( );
int gui_WidgetInitAttr( struct gui_Widget *w );
//int gui_WidgetRelease( struct gui_Widget *w );
int gui_WidgetInit( struct gui_Widget *w, struct gui_Widget *parent );
int gui_WidgetSetDynamic( struct gui_Widget *w );
int gui_WidgetSetStatic( struct gui_Widget *w );
int gui_WidgetSetName( struct gui_Widget *w, char *name );
char *gui_WidgetGetName( struct gui_Widget *w );
int gui_WidgetSetAction( struct gui_Widget *w, struct trop_Action *action );
inline void gui_EventInit( struct gui_Event *ev );
#ifdef USETCL
    void tkEventProc( ClientData clientData, XEvent *eventPtr );
#else
    int gui_WidgetQueue( struct gui_Widget *w );
#endif
int gui_WidgetPushEvent( void *event );
int gui_WidgetDoEvent( struct gui_Widget *wi, struct gui_Event *ev );
int gui_WidgetExpose( struct gui_Widget *wi, int x, int y, int w, int h );
int gui_WidgetButtonPress( struct gui_Event *event );
int _gui_widget_add_gobj( struct gui_Widget *w, void *gobj, unsigned int value, int id );
int gui_WidgetAddWidget( struct gui_Widget *p, struct gui_Widget *w ); // add a sub widget to parent
int gui_WidgetAddRelay( struct gui_Widget *w, struct trop_Relay *r );
int gui_WidgetAddRelayMsg( struct gui_Widget *wi, struct gui_Msg *msg );
int gui_WidgetAddRelayEx( struct gui_Widget *wi, struct gui_Widget *rwi ); // guirelay.c
int gui_WidgetDoRelaysStatic( struct gui_Widget *wi, struct gui_Event *ev );
int gui_WidgetRemoveAllRelayAtID( struct gui_Widget *w, int id );

struct gui_GC *gui_InitGCEx( struct gui_GC *gc, char *hex );
int gui_CreateGC( struct gui_GC *gc );
int gui_CreateImage( struct gui_Widget *main, struct gui_Image *i );
int gui_ImageFreeContent( struct gui_Image *i ); // only free the pixel buffer
int gui_ImageFree( struct gui_Image *i ); // free the object with the pixel buffer

int gui_WidgetReleaseEx( struct gui_Widget *w );
int gui_WidgetAddCustomValue( struct gui_Widget *w, int value ); // GOID_MISC for custom handling
int gui_WidgetAddCustomPtr( struct gui_Widget *w, void *ptr );
int gui_WidgetAddImage( struct gui_Widget *w, struct gui_Image *i );
int gui_WidgetAddStr( struct gui_Widget *w, char *str ); // GCFG_STR
int gui_WidgetAddText( struct gui_Widget *w, struct gui_Text *text );
int gui_WidgetAddTextSimple( struct gui_Widget *w, char *str );
int gui_WidgetAddTextSimpleB( struct gui_Widget *w, char *str, int placement );
int gui_WidgetAddTextSimpleC( struct gui_Widget *w, char *str, int placement );
int gui_WidgetAddTextSimpleD( struct gui_Widget *w, struct gui_TextSim *ts, 
char *str, int placement );
int gui_WidgetSetColor( struct gui_Widget *w, char *name );
int gui_WidgetSetRound( struct gui_Widget *w, int round );
BOOL gui_WidgetIsRound( struct gui_Widget *w );
int gui_WidgetSetGradient( struct gui_Widget *w, int grad );
int gui_WidgetSetBgAlpha( struct gui_Widget *w, int value );
int gui_WidgetAddGlPoly( struct gui_Widget *w, struct gui_GlPoly *poly );
int gui_ImagePosition( struct gui_Image *i, int x, int y );
int gui_WidgetPaintRegion( struct gui_Widget *main, int x, int y, int x2, int y2 );
void _gui_WidgetResetMotion( struct gui_Widget *w );
int gui_WidgetSetPosition( struct gui_Widget *widget, int x, int y, int w, int h );
int gui_WidgetSetPositionRelative( struct gui_Widget *widget, struct gui_Widget *parent, double dx, double dy, double dw, double dh );
int gui_WidgetGetLevel( struct gui_Widget *widget );
int gui_WidgetSetLevel( struct gui_Widget *widget );

/**
 *  DataModel handling
 */
int gui_WidgetInitDataModelCSV( struct gui_Widget *widget, unsigned char *filename );

int form_SlidebarInit( struct gui_Widget *main, struct gui_Widget *w );
int form_SlidebarSetGeometry( struct gui_Widget *main, struct gui_Widget *w );
int form_SlidebarButtonPress( struct gui_Event *event );
int form_SlidebarExpose( struct gui_Widget *main, struct gui_Widget *w );
int form_SlidebarSetRelay( struct gui_Widget *w, struct gui_Widget *relay );

int form_MenuInit( struct gui_Widget *w );
int form_MenuSetGrid( struct gui_Widget *w, int ew, int eh );
int form_MenuAddButton( struct gui_Widget *w, struct gui_Widget *b ); // override with form_AddButton
void *form_MenuExposeCall( void *obj, int x, int y, int w, int h );
int form_MenuExpose( struct gui_Widget *main, struct gui_Widget *w );
int form_MenuButtonPress( struct gui_Event *event );
int form_MenuMoveElements( struct gui_Widget *w, int x, int y );
int form_MenuMoveElementsExY( struct gui_Widget *w, int direction );
int form_MenuResetElementsPosition( struct gui_Widget *w );
int _form_MenuMoveUp( struct gui_Widget *w );
int _form_MenuMoveDown( struct gui_Widget *w );
void *form_MenuMoveUp( void *data );
void *form_MenuMoveDown( void *data );

int form_OverlayInit( struct gui_Widget *w );
int form_OverlayExpose( struct gui_Widget *w );
int form_OverlayButtonPress( struct gui_Event *event );

int form_TitlebarInit( struct gui_Widget *w );
int form_TitlebarExpose( struct gui_Widget *w );
int form_TitlebarButtonPress( struct gui_Event *event );
  
int form_ButtonSetText( struct gui_Widget *w, char *str );

int form_ProgressInit( struct gui_Widget *w );
int form_ProgressSetValue( struct gui_Widget *w, int high, int value );
int form_ProgressExpose( struct gui_Widget *w );

int form_LBEntryInit( struct gui_Widget *w );
int form_LBEntryConfigure( struct gui_Widget *w, char *strbig, char *strsmall );
int form_LBEntryButtonPress( struct gui_Event *event );
int form_LBEntryRelease( struct gui_Widget *w );

struct form_Frame *form_FrameInit( struct form_Frame *frame, int x, int y, int w, int h );

int form_CanvasRelease( struct gui_Event *event );
int form_CanvasMotion( struct gui_Event *event );
int form_CanvasButtonPress( struct gui_Event *event );
int form_CanvasMove( struct gui_Widget *, int val );
void *form_CanvasExposeCall( void *obj, int x, int y, int w, int h ); // refd to obj
int form_CanvasExpose( struct gui_Widget *w );
int form_CanvasAddNewline( struct gui_Widget *can );
int form_CanvasRemoveAllObjRefType( struct gui_Widget *wi, int form_type );
int form_CanvasRemoveAllObjRef( struct gui_Widget *wi ); // remove references
int form_CanvasRemoveAllObj( struct gui_Widget *wi ); // remove objects + references
int form_CanvasAddObject( struct gui_Widget *can, struct gui_Widget *child );
int form_CanvasAddObjectStatic( struct gui_Widget *can, struct gui_Widget *child );
int form_CanvasInit( struct gui_Widget *w );
int form_CanvasSetMov( struct gui_Widget *w, int movetype );
struct _can_content *form_CanvasGetContentEx( struct gui_Widget *wi ); // content handling
int form_CanvasSetContentEx( struct gui_Widget *wi, struct _can_content *content );
int form_CanvasAddObjByRss( struct gui_Widget *w, struct trop_Rss *rss );

/**
 * Canvas Data Model
 */
int form_CanvasDataModelInitFromCSV( struct gui_Widget *w, unsigned char *filename );

int form_TextInit( struct gui_Widget *w );
int form_TextAddText( struct gui_Widget *w, char *str );
int form_TextAddHtmlText( struct gui_Widget *w, char *str );
  
int form_EditableInit( struct gui_Widget *w );
int form_EditableExpose( struct gui_Widget *wi, int w, int h, int x, int y );
int form_EditableAppendChar( struct gui_Widget *w, char c );
int form_EditableRemoveChar( struct gui_Widget *w );
  
int form_KeyboardInit( struct gui_Widget *w );
int form_KeyboardExpose( struct gui_Widget *wi, int w, int h, int x, int y );
int form_KeyboardButtonPress( struct gui_Event *ev );
int form_KeyboardButtonPressEx( struct gui_Event *ev );
  
int form_SimpleInit( struct gui_Widget *w );
int form_SimpleRelease( struct gui_Widget *w );
int form_SimpleConfigure( struct gui_Widget *w, char *image_filename, char *name, char *long_desc, char *short_desc );
  
int __read_file_content( FILE *fd, char *line, char *content );
int _gui_setup_get_object_position( struct xml_Object *oo, int *x, int *y, int *w, int *h );
struct gui_Layout *gui_LayoutInit( struct gui_Layout *_layout, char *name );
  
struct gui_Scene *gui_NewScene( );
int gui_SceneNone( );
int gui_SceneSet( struct gui_Scene *_scene );
int gui_SceneAddMember( struct gui_Scene *_scene, struct gui_Widget *member );
BOOL gui_SceneIsWidget( struct gui_Scene *_scene, struct gui_Widget *w );
int gui_SceneBack( );
int gui_SceneSave( ); // save current scene to history 'use after initialisation'
struct gui_Scene *gui_SceneCurrent( );

struct gui_SceneEx *gui_SceneExInit( struct gui_SceneEx *s, char *name );

struct gui_Color *gui_NewColor( char *name );
struct gui_PixBuf *gui_NewPixBuf( int w, int h );

int gui_WidgetToTop( struct gui_Widget *w );

struct rec_Expose *rec_ExposeInit( struct rec_Expose *rec );
int rec_ExposePrint( struct rec_Expose *rec );
int rec_ExposeAddRect( struct rec_Expose *rec, int x, int y, int w, int h, unsigned long pixel );
int rec_ExposeAddTxt( struct rec_Expose *rec, int x, int y, int w, int h, char *str );
int rec_ExposeToXML( struct rec_Expose *rec, char *str );
int rec_ExposeLogXML( char *str, char *filename );
int rec_ExposePlayXML( char *str );
int rec_ExposePlayXMLFile( char *filename );

struct gui_Msg *gui_MsgInit( struct gui_Msg *msg, int type );
int gui_MsgDoMsg( struct gui_Msg *msg );
struct gui_Msg *gui_MsgNetworkInit( struct gui_Msg *msg );
int gui_MsgNetworkDoMsg( struct gui_Msg *msg );
int gui_MsgNetworkSetConnection( struct gui_Msg *msg, struct trop_Conn *conn );
int gui_MsgSetStringSimple( struct gui_Msg *msg, char *str );
int gui_MsgSetXString( struct gui_Msg *msg, char *name, char *from, char *body );
struct trop_List *gui_MsgBitsAddBit( struct trop_List *bits, char *name, char *value );

int gui_BoundsSet( struct gui_Bounds *bounds, int x, int y, int width, int height );
int gui_BoundsSetEx( struct gui_Bounds *bounds, struct gui_WidgetAttributes *attr );

#ifdef USETCL
  int _tcl_init( );
#endif

#ifdef __cplusplus
    }
#endif

#endif
