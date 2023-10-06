/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _MIKA_CONFIG_H_
#define _MIKA_CONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define _MIKA_SIGNAGE_APP
#define _MIKA_CONFIG_SERVER "127.0.0.1"

int mikaConfigGet( unsigned char *activity );

#endif
