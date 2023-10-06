/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

/** deprecated layout management - remove
 */

#include "layout.h"

#define _MSG_BUFFER 	4096*8
#define _MSG_LINE	4096

struct layoutEx *layoutNew( )
{
  struct layoutEx *myLayout = (struct layoutEx*)malloc(sizeof(struct layoutEx));
  return myLayout;
}

void layoutRelease( struct layoutEx *myLayout )
{
  if (!myLayout)
    return;
}

int layoutInitWithPath( struct layoutEx *myLayout, unsigned char *path )
{
  if (!path||!myLayout)
    return 1;
 
  FILE *fd = NULL;
  static unsigned char msg[_MSG_BUFFER];
  static unsigned char line[_MSG_LINE];
 
  msg[0] = 0x00;
 
  if ((fd = fopen((char*)path,"r")))
  {
    strcpy((char*)msg,"");
    while(1)
    {
      fgets((char*)line,4095,fd);
      strcat((char*)msg,(char*)line);
    }
    fclose(fd);
  }
  
  if (msg[0]==0x00)
    return 1;
  
  myLayout->doc.Parse(
    (const char*)msg,
    0,TIXML_ENCODING_UTF8
  );
  
  /** test
   */
  TiXmlElement *root = myLayout->doc.FirstChildElement("node");
  if (!root)
  {
    #ifdef _DEBUG
      fprintf(stderr,"layoutInitWithPath file(%s) no root element\n",
        path);
    #endif
  }
  
  return 0;
}
