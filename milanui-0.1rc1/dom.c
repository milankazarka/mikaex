/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "dom.h"
#include "libTinyXml/tinyxml.h"
#include "config.h"

dom *dom_InitNamed(
  unsigned char *filename
)
{
  dom *dom = NULL;
  
  if (!filename)
  {
    return NULL;
  }
  
  TiXmlDocument *doc = new TiXmlDocument((const char*)filename);
  if (!doc)
  {
    return NULL;
  }
  bool loading = doc->LoadFile();
  if (!loading)
  {
    return NULL;
  }
  
  dom = (struct _dom*)malloc(sizeof(struct _dom));
  if (!dom)
  {
    if (DEBUG==TRUE) printf("dom_InitNamed failed\n");
  }
  
  dom->low = (void*)doc;
  
  return dom;
}

domObject *dom_Object(
  dom *dom,
  domObject *parent,
  unsigned char *named
)
{
  domObject *child = NULL;
  
  if (!dom || !named)
  {
    return NULL;
  }
  
  TiXmlDocument *doc = (TiXmlDocument*)dom->low;
  if (!doc)
  {
    return NULL;
  }
  
  child = (struct _domObject*)malloc(sizeof(struct _domObject));
  child->low = doc->FirstChildElement((const char*)named);
  if (child->low)
  {
    free(child);
  }

  return child;
}
