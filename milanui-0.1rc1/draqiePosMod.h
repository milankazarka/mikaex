/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _DRAQIE_POS_MOD_H_
#define _DRAQIE_POS_MOD_H_

#include <stdio.h>

typedef struct draqiePosRequest
{
  unsigned char line[4096];
  unsigned char order[4096];
  int deviceid;
  int state;
  int requestid;
  struct draqiePosRequest *next;
} draqiePosRequest;

typedef struct draqiePosData
{
  struct draqiePosRequest *requests;
} draqiePosData;

void *_process_data_draqie_pos( unsigned char *in, unsigned char *out );
struct draqiePosData *draqiePosDataInit( );
int draqiePosDataRelease( struct draqiePosData *data );
int draqiePosDataRead( struct draqiePosData *data, unsigned char *filename );
int draqiePosDataWrite( struct draqiePosData *data, unsigned char *filename );
int draqiePosDataAdd( struct draqiePosData *data, int deviceId, unsigned char *order );
int draqiePosDataRemove( struct draqiePosData *data, int requestid );

#endif
