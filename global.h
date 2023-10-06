/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _BASE_H_
#define _BASE_H_

#ifdef __cplusplus
  extern "C" {
#endif

#include "config.h"

extern struct gui_Widget	*expose;
extern struct gui_Widget	*lst_widgets;
extern struct gui_Scene	        *scene;
extern struct trop_List	        *layout;
extern struct trop_List	        *lst_images;
extern struct trop_List	        *lst_txtcache;			// text cache
extern BOOL			bredrawbg;
extern struct gui_object	*gobj_scene_history;
extern struct trop_List	        *lst_can_content;		// canvas content pool
extern struct xml_Inst	        skin;			        // general skin
extern struct gui_Surface	*txt_surface;
extern struct gui_Surface	*surface;			// direct surface to blit - if
                                                                //	there is no rotation, than
                                                                //	we point it to the transformation
                                                                //	surface
extern struct gui_Bounds	gui_bounds;			// current bounds used by drawing
extern struct gui_Gl		*gl;
extern struct gui_GlPoly	*glpoly;
extern struct rec_Expose	*rec_expose;			// record 'exposure'
extern char			str_rec_expose[1024*64]; 	// xml string describing rec_expose
extern short int		rotation;

extern struct trop_List         *_lst_goid_custom;

extern unsigned char		glob_data[TROP_WINDATA];

#ifdef USECAIRO
    extern struct gui_Surface	*cairo_gui_surface;
    extern cairo_surface_t	*cairo_surface;
    extern cairo_t		*cairo;
#endif

#ifdef USETCL
    extern Tcl_Interp		*interp;
    extern Tk_Window		tkwin;
#endif

extern FILE 			*_stdout;
extern struct trop_Mutex	_m_lock;
#ifdef USETCL
  extern struct trop_Mutex	tcl_lock;
#endif

#ifdef __cplusplus
  }
#endif

#endif
