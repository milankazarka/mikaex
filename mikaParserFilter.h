/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _MIKA_PARSER_FILTER_H_
#define _MIKA_PARSER_FILTER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

typedef struct mikaParserFilterConfiguration
{
} mikaParserFilterConfiguration;

typedef struct mikaParserFilterEmail
{
  unsigned char address[256];
  struct mikaParserFilterEmail *next;
} mikaParserFilterEmail;

typedef struct mikaParserFilter
{
  struct mikaParserFilterConfiguration *configuration;
  FILE *fdIterate;
  void *(*callFilterElement)(struct mikaParserFilter *myFilter, unsigned char *element);
  void *(*callFilterElementProcess)(struct mikaParserFilter *myFilter, unsigned char *url);
  struct mikaParserFilterEmail *email;
} mikaParserFilter;

extern struct mikaParserFilter *filter;

struct mikaParserFilter *mikaParserFilterInit( );
int mikaParserFilterIterateFile( struct mikaParserFilter *myFilter, unsigned char *filename );
int mikaParserFilterFetchUrl( struct mikaParserFilter *myFilter, unsigned char *element, unsigned char *url );
int mikaParserFulterExtractEmails( struct mikaParserFilter *myFilter, unsigned char *filename );

struct mikaParserFilterEmail *mikaParserFilterEmailInit( );
int mikaParserFilterEmailInList( struct mikaParserFilterEmail *email, unsigned char *address );
int mikaParserFilterEmailIsCorrect( unsigned char *address );

#endif
