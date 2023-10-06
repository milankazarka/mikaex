/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _DRAQIE_CONFIG_H_
#define _DRAQIE_CONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define _DRAQIE_SIGNAGE_APP
#define _DRAQIE_CONFIG_SERVER "127.0.0.1"

int draqieConfigGet( unsigned char *activity );

#endif
