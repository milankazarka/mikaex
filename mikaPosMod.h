/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _MIKA_POS_MOD_H_
#define _MIKA_POS_MOD_H_

#include <stdio.h>

typedef struct mikaPosRequest
{
  unsigned char line[4096];
  unsigned char order[4096];
  int deviceid;
  int state;
  int requestid;
  struct mikaPosRequest *next;
} mikaPosRequest;

typedef struct mikaPosData
{
  struct mikaPosRequest *requests;
} mikaPosData;

void *_process_data_mika_pos( unsigned char *in, unsigned char *out );
struct mikaPosData *mikaPosDataInit( );
int mikaPosDataRelease( struct mikaPosData *data );
int mikaPosDataRead( struct mikaPosData *data, unsigned char *filename );
int mikaPosDataWrite( struct mikaPosData *data, unsigned char *filename );
int mikaPosDataAdd( struct mikaPosData *data, int deviceId, unsigned char *order );
int mikaPosDataRemove( struct mikaPosData *data, int requestid );

#endif
