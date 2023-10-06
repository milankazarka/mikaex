/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

/** todo - implement dom handler
 */

#include "mikaServer.h"

struct mikaDomObject *mikaDomObjectInit( struct mikaDom *dom )
{
  if (!dom)
    return NULL;
  
  struct mikaDomObject *object = (struct mikaDomObject*)malloc(sizeof(struct mikaDomObject));
  object->children = NULL;
  object->next = NULL;
  
  return object;
}

int mikaDomInitValues( struct mikaDom *dom )
{
  if (!dom)
    return 1;
  
  dom->objects = NULL; 
  dom->doc = NULL;
    
  return 0;
}

struct mikaDom *mikaDomInit( struct mikaDom *dom )
{
  if (!dom)
    dom = (struct mikaDom*)malloc(sizeof(struct mikaDom));

  mikaDomInitValues(dom);

  return dom;
}

int mikaDomRelease( struct mikaDom *dom )
{
  if (!dom)
    return 1;
  
  if (dom->doc)
    free(dom->doc);  
  free(dom);
    
  return 0;
}

/** process a chunk of data
 */
int mikaDomProcessData( struct mikaDom *dom, unsigned char *data )
{
  if (!dom || !data)
    return 1;
  
  return 0;
}

/** process whole xml data structure
 */
int mikaDomParseData( struct mikaDom *dom, unsigned char *data )
{
  if (!dom || !data)
    return 1;
  
  TiXmlDocument *doc = new TiXmlDocument();
  
  doc->Parse((const char*)data,0,TIXML_ENCODING_UTF8);
  dom->doc = doc;
  
  return 0;
}
