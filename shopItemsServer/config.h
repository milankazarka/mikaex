/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#define _CF_PLATFORM_UNIX
#define _CF_GUI_XLIB

#define USECAIRO
#define USETCL0

#ifdef USECAIRO
  #define _CAIRO_DEVICE_NO
#endif

#define USERSS_NO
#define _RSSTYPE_MRSS

#define		TRUE		1
#define		FALSE		0
typedef unsigned short int	BOOL;

#ifndef _DEBUG
  #define _DEBUG
  #define _POLL_DEBUG
#endif
#define DEBUG	TRUE

#define _STRESS_NO

#define _USE_EAPI_NO
#define _IDLE_THREAD_CALL_NO
#define _POLL_ACTIVITY_DIRECTORY_NO
#define _EVENT_TIMER_NO
#define _XFLUSH
#define _WIDGET_EXPOSE
#define _XTHREADS
#define _USE_SKIN_NO	// use auto-configuration of UI objects
#define _BORDER 15
#define _GUI_CLEAR_ON_EXPOSE
#define _SYNC_NO

#endif
