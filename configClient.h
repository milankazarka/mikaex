/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _CONFIG_CLIENT_H_
#define _CONFIG_CLIENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "libTinyXml/tinyxml.h"
#include "libTinyXml/tinystr.h"

typedef struct configClient
{
} configClient;

struct configClient *configClientNew( );

#endif
