/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _MIKA_PARSER_EMAIL_FILTER_H_
#define _MIKA_PARSER_EMAIL_FILTER_H_

#include "mikaParser.h"

typedef struct mikaParserSimpleEmail
{
  unsigned char address[512];
  struct mikaParserSimpleEmail *next;
} mikaParserSimpleEmail;

typedef struct mikaParserEmailFilter
{
  FILE *infd;
  struct mikaParserSimpleEmail *emails;
  unsigned char input[256];
} mikaParserEmailFilter;

struct mikaParserSimpleEmail *mikaParserSimpleEmailInit( );
int mikaParserSimpleEmailPresent( struct mikaParserSimpleEmail *mySimpleEmail, unsigned char *address );
struct mikaParserSimpleEmail *mikaParserSimpleEmailAdd( struct mikaParserSimpleEmail *mySimpleEmail, unsigned char *address );
int mikaParserSimpleEmailValidate( unsigned char *address );

struct mikaParserEmailFilter *mikaParserEmailFilterInit( );
int mikaParserEmailFilterRun( struct mikaParserEmailFilter *emailFilter );

#endif
