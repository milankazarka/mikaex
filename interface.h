/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "config.h"
#include "os.h"
#include "activity.h"
#include "mikaSerial.h"

/** defines if we should use a backplane in our applications
 *  which can be accessed by external applications
 */
#define _INTERFACE_BACKPLANE
#define _INTERFACE_BACKPLANE_BLANK_BG_NO 	// only copy the backplane to pixels on
                                                // the interface surface if they're blank

enum {
  INTERFACE_EVENT_NONE,
  INTERFACE_EVENT_EXPOSE, // can come only from the application
  INTERFACE_EVENT_BDOWN,
  INTERFACE_EVENT_BUP,
  INTERFACE_EVENT_MOTION,
  INTERFACE_EVENT_LEAVE
};

typedef struct interface_event_t
{
  int            type;
  int            x;
  int            y;
  int            width;
  int            height;
} interface_event_t;

#ifdef _CF_GUI_XLIB

  #include <X11/Xlib.h>
  #include <X11/X.h>
  #include <X11/Xutil.h>
  #include <X11/extensions/shape.h>
  #include <X11/xpm.h>

  typedef struct platform_low_t
  {
    Display		            *dpy;
    int			            screen;
    Window		            id;
    
    GC		                    *gc;
    XGCValues		            *gcv;

    Pixmap		            p;
    Pixmap		            m;
    XImage		            *surface;
    XImage			    *_surface;
  } platform_low_t;
#endif

typedef struct platform_t
{
  void                          *interface;
  struct platform_low_t         low;
  void		                *(*refresh)(struct platform_t *platform);
} platform;

#define _SHM_ID_FILENAME "logShmRootController"
typedef struct controller_t
{
  int				SHMID;
  int				SEMID;
} controller;

// accessible backplane of the interface
typedef struct interface_backplane_t
{
  // custom size - don't use the interface size
  int			w;
  int			h;
  int			shmid;
  void			*pixels; // shm data
} interface_backplane_t;

typedef struct interface_delegate
{
  void			*(*interfaceOnEvent)(void*event);
} interface_delegate;

typedef struct interface_t
{
  struct interface_delegate 
                        *delegate;
  
  struct platform_t     platform;
  struct mikaSerial	*serial;
  
  struct controller_t	controller;
  struct activity	activity;
  
  struct interface_backplane_t
                        *backplane;
  struct gui_Surface	*surface;
  
  unsigned char		*surfaceData;
  unsigned char         *_surfaceData; // when rotation, this is a middle step
  unsigned short int    x;
  unsigned short int    y;
  unsigned short int	width;
  unsigned short int	height;
  unsigned short int	bpp;            // bits per pixel
  unsigned short int	scanline;
  struct interface_event_t
                        newEvent;
  void			*(*createSurface)(struct interface_t *interface );
  void                  *(*mapMainWindow)(struct interface_t *interface);
  void                  *(*getHWWindowId)(struct interface_t *interface, long *id);
  void			*(*guiFlush)(struct interface_t *interface);
  void                  *(*nextGuiEvent)(struct interface_t *interface, struct interface_event_t *event);
  void                  *(*addGuiEvent)(struct interface_t *interface, struct interface_event_t *event);
  void			*(*triggerExpose)(struct interface_t *interface);
  /** custom interface wide event handling
   */
  void			*(*onButtonDown)(int x, int y);
  void			*(*onButtonUp)(int x, int y);
} interface_t;

extern struct interface_t *bigInterface;
extern unsigned char *interfaceName;

struct interface_t *interfaceInit( struct interface_t *interface );
int interfaceStart( struct interface_t *interface );
/** set the name of this app
 */
int interfaceSetName( unsigned char *name );
int interfaceRecordBackplaneAccess( unsigned char *name );
int interfaceSetBackplaneSize( int w, int h );
void interfaceDeinit( );

#endif
