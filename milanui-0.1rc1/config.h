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

#define USERSS_NO
#define _RSSTYPE_MRSS_NO

#define		TRUE		1
#define		FALSE		0
typedef unsigned short int	BOOL;

#ifndef _DEBUG
  #define _DEBUG_NO
#endif
#define DEBUG	FALSE

#endif
