/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _CSV_H_
#define _CSV_H_

#include "system.h"

#define _CSV_LINELEN 4096

typedef struct trop_Csv
{
  unsigned char 	*data;
  int 			len;
  struct trop_Csv 	*members;
  struct trop_Csv 	*next;
  
  int			specId;		// usecase speciffic ID
} trop_Csv;

struct trop_Csv *trop_CsvInit( struct trop_Csv *csv, unsigned char *filename );
int trop_CsvGetDataAtPosition( struct trop_Csv *csv, int posList, int posMember, int maxLen, unsigned char *data, int *lenData );
int trop_CsvGetListSize( struct trop_Csv *csv );
int trop_CsvGetListMemberSize( struct trop_Csv *csv, int posList );

#endif
