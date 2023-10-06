/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "setup.h"

/** initialize setup from file (todo - implement)
 */
struct setup *setupInit( struct setup *s, unsigned char *filename )
{
  if (!filename) return NULL;
  if (!s) s = (struct setup*)malloc(sizeof(struct setup));
  
  s->data.LoadFile((char*)filename,TIXML_DEFAULT_ENCODING);
  TiXmlElement *root = s->data.FirstChildElement((char*)"setup");
  if (!root)
  {
    free(s);
    #ifdef _DEBUG
      printf("setupInit couldn't load file(%s)\n",filename);
    #endif
    return NULL;
  }
  s->root = root;
  
  return s;
}

/** get an attribute of the setup
 *
 *    s 	- setup
 *    name 	- name of the object 
 *                  <setup>
 *                    <name>
 *    attr	- the attribute of the object "optional"
 *    to	- storage of the result
 */
int setupGet( struct setup *s, unsigned char *name, unsigned char *attr, unsigned char *to )
{
  if (!s||!name||!to) return 1;
  
  to[0] = 0x00;
  
  TiXmlElement *obj = s->root->FirstChildElement((char*)name);
  if (!obj)
  {
    #ifdef _DEBUG
      printf("setupGet couldn't find setup value(%s)\n",(char*)name);
    #endif
    return 1;
  }
  
  if (!attr)
  {
    if (obj->GetText())
      strcpy((char*)to,(char*)obj->GetText());
  }
  else
  {
    if (obj->Attribute((char*)attr))
      strcpy((char*)to,(char*)obj->Attribute((char*)attr));
  }
  
  return 0;
}
