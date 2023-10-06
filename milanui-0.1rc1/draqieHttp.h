/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _DRAQIE_HTTP_H_
#define _DRAQIE_HTTP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

typedef struct draqieHttp
{
  CURL *curl;
  CURLcode res;
} draqieHttp;

struct draqieHttp *draqieHttpInit( struct draqieHttp *myHttp );
int draqieHttpGet( struct draqieHttp *myHttp, unsigned char *address );
unsigned char *draqieHttpData( struct draqieHttp *myHttp, unsigned char *data );

#endif
