/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _MIKA_HTTP_H_
#define _MIKA_HTTP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

typedef struct mikaHttp
{
  CURL *curl;
  CURLcode res;
} mikaHttp;

struct mikaHttp *mikaHttpInit( struct mikaHttp *myHttp );
int mikaHttpGet( struct mikaHttp *myHttp, unsigned char *address );
unsigned char *mikaHttpData( struct mikaHttp *myHttp, unsigned char *data );

#endif
