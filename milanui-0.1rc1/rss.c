/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "system.h"

struct trop_Rss *trop_RssInit( struct trop_Rss *rss )
{
  if (rss==NULL)
  {
    rss = (struct trop_Rss*)malloc(sizeof(struct trop_Rss));
  }
  rss->items = NULL;
  return rss;
}

int trop_RssParseFile( struct trop_Rss *rss, char *filename )
{
  int ret;
  struct trop_RssItem *titem;
  #ifdef _RSSTYPE_MRSS
    mrss_item_t *item;
  #endif
  
  if (!rss||!filename) return -1;
  
  #ifdef _RSSTYPE_MRSS
    ret = mrss_parse_file(filename,&rss->mrss);
    if (ret)
    {
      printf("error: rss parsing (%s)\n",filename);
    }
    item = rss->mrss->item;
    while(item)
    {
      titem = (struct trop_RssItem*)malloc(sizeof(struct trop_RssItem));
      titem->desc[0] = 0x00;
      titem->title[0] = 0x00;
      
      if (strlen(item->description)<RSS_LEN_CONTENT) strcpy((char*)titem->desc,item->description);
      else strncpy((char*)titem->desc,item->description,RSS_LEN_CONTENT-1);
      
      if (strlen(item->title)<RSS_LEN_TITLE) strcpy((char*)titem->title,item->title);
      else strncpy((char*)titem->title,item->title,RSS_LEN_TITLE-1);
      
      rss->items = trop_ListAdd(rss->items,titem);
      item = item->next;
    }
    
  #endif
  
  return 0;
}
