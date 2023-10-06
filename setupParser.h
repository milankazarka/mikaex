#ifndef _SETUP_PARSER_H_
#define _SETUP_PARSER_H_

#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"
extern "C" {
  #include "networkSetup.h"
}

typedef struct setupParser
{
  TiXmlDocument *doc;
} setupParser;

struct setupParser *setupParserInit( struct setupParser *parser, struct networkSetup *setup );
int setupParserGetScreensCount( struct setupParser *parser, struct networkSetup *setup, unsigned char *name );
TiXmlElement *setupParserGetScreenNamed( struct setupParser *parser, struct networkSetup *setup, unsigned char *name );
TiXmlElement *setupParserGetScreenNamedAtRoot( struct setupParser *parser, struct networkSetup *setup, unsigned char *name, TiXmlElement *root );
int setupParserScreenGetItemCount( struct setupParser *parser, struct networkSetup *setup, TiXmlElement *screen );
TiXmlElement *setupParserScreenGetItemAtIndex( struct setupParser *parser, struct networkSetup *setup, TiXmlElement *screen, int index );
// retrieve the count of elements of a certain name
int setupParserGetElementCount( struct setupParser *parser, struct networkSetup *setup, unsigned char *group, unsigned char *name );
TiXmlElement *setupParserGetElementAtIndex( struct setupParser *parser, struct networkSetup *setup, unsigned char *group, unsigned char *name, int index );
TiXmlElement *setupParserGetElementAtIndexAtRoot( struct setupParser *parser, struct networkSetup *setup, int index, unsigned char *named, TiXmlElement *root );

#endif
