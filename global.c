/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "system.h"
#include "gui.h"
#include "global.h"

struct gui_Widget	*expose;
struct gui_Widget	*lst_widgets;
struct gui_Scene	*scene;
struct trop_List	*layout;
struct trop_List	*lst_images;
struct trop_List	*lst_txtcache;			// text cache
BOOL		        bredrawbg;
struct gui_object	*gobj_scene_history;
struct trop_List	*lst_can_content;		// canvas content pool
struct xml_Inst	        skin;			        // general skin
struct gui_Surface	*txt_surface;
struct gui_Surface	*surface;			// direct surface to blit - if
                                                        // there is no rotation, than
                                                        // we point it to the transformation
                                                        // surface
struct gui_Bounds	gui_bounds;
struct gui_Gl		*gl;
struct gui_GlPoly	*glpoly;
struct rec_Expose	*rec_expose;			// record 'exposure'
char			str_rec_expose[1024*64]; 	// xml string describing rec_expose
short int		rotation;

struct trop_List        *_lst_goid_custom;

unsigned char		glob_data[TROP_WINDATA];

#ifdef USECAIRO
    struct gui_Surface	*cairo_gui_surface;
    cairo_surface_t	*cairo_surface;
    cairo_t		*cairo;
#endif

#ifdef USETCL
    Tcl_Interp		*interp;
    Tk_Window		tkwin;
#endif

FILE 			*_stdout;
struct trop_Mutex	_m_lock;
#ifdef USETCL
  struct trop_Mutex	tcl_lock;
#endif
