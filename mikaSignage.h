/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _MIKA_SIGNAGE_H_
#define _MIKA_SIGNAGE_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include "mikaParser.h"
#include "config.h"
#include "os.h"

struct FtpFile 
{
  const char *filename;
  FILE *stream;
};

typedef struct mikaSignageContent
{
  unsigned char id[1024];
  unsigned char filename[1024];
  struct mikaSignageContent *next;
} mikaSignageContent;

typedef struct mikaSignageConfiguration
{
  struct mikaSignageContent *content;
} mikaSignageConfiguration;

typedef struct mikaSignageWindowAttr
{
  struct osWindowAttr osAttr;
} mikaSignageWindowAttr;

typedef struct mikaSignageWindow
{
  struct mikaSignageWindowAttr attr;
  struct osWindow *window;
} mikaSignageWindow;

typedef struct mikaSignageMutex
{
  pthread_mutex_t mutex;
} mikaSignageMutex;

typedef struct mikaSignageSem
{
  int id;
} mikaSignageSem;

typedef struct mikaSignage
{
  CURL *curl;
  CURLcode res;  
  unsigned char server[256];
  unsigned char directory[256];
  struct FtpFile ftpFile;
  struct mikaSignageConfiguration *configuration;
} mikaSignage;

struct mikaSignage *mikaSignageInit( );
int mikaSignageRelease( struct mikaSignage *myMikaSignage );
int mikaSignageSetServer( struct mikaSignage *myMikaSignage, unsigned char *server );
int seaqieSignageSetDirectory( struct mikaSignage *myMikaSignage, unsigned char *directory );
int mikaSignageDownload( struct mikaSignage *myMikaSignage, unsigned char *filename );
int mikaSignageConfigurationGet( struct mikaSignage *myMikaSignage );
struct mikaSignageConfiguration *mikaSignageConfigurationInit( );
int mikaSignageConfigurationParse( struct mikaSignage *mikaSignage );
int mikaSignageIterate( struct mikaSignage *mySignage );

struct mikaSignageContent *mikaSignageContentInit( );

struct mikaSignageWindow *mikaSignageWindowInit( struct mikaSignageWindowAttr *attr );
int mikaSignageWindowEventLoop( struct mikaSignageWindow *myWindow );
int mikaSignageWindowHide( struct mikaSignageWindow *myWindow );
int mikaSignageWindowShow( struct mikaSignageWindow *myWindow );

struct mikaSignageMutex *mikaSignageMutexInit( );
int mikaSignageMutexLock( struct mikaSignageMutex *myMikaSignageMutex );
int mikaSignageMutexUnlock( struct mikaSignageMutex *myMikaSignageMutex );

struct mikaSignageSem *mikaSignageSemInit( );
int mikaSignageSemWait( struct mikaSignageSem *myMikaSem );
int mikaSignageSemPost( struct mikaSignageSem *myMikaSem );

extern mikaSignage *signage;
extern struct mikaSignageMutex *signageMutex;
extern struct mikaSignageSem *signageSem;

#endif
