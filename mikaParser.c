/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "mikaParser.h"
#define _STATIC_NO

#ifdef _STATIC
  struct mikaParser *parser = NULL;
#endif

int mikaParserLock( struct mikaParser *myParser )
{
  if (!myParser)
    return 1;
  pthread_mutex_lock(&myParser->mutexId);  
    
  return 0;
}

int mikaParserUnlock(	struct mikaParser *myParser )
{
  if (!myParser)
    return 1;
  pthread_mutex_unlock(&myParser->mutexId);  
    
  return 0;
}

struct mikaParser *mikaParserInit( )
{
  struct mikaParser *myParser = (struct mikaParser*)malloc(sizeof(struct mikaParser));
  pthread_mutex_init(&myParser->mutexId,NULL);
  myParser->iterator = mikaParserIteratorInit();
  myParser->masterIndex = 0;
  return myParser;
}

void *mikaParserRunIteratorThread( void *data )
{
  struct mikaParserIterator *myIterator = (struct mikaParserIterator*)data;

  unsigned char base[512];
  unsigned char command[1024];
  unsigned char local[128];
  int index;
  
  while(1)
  {
    mikaParserLock(parser);
    myIterator->callNextPage(myIterator);
    index = myIterator->index;
    strcpy((char*)base,(char*)myIterator->base);
    mikaParserUnlock(parser);
    
    sprintf((char*)local,"pages/%s%d.html",(char*)myIterator->id,index);
    sprintf((char*)command,"wget -o wget.log -O %s %s",local,(char*)base);
    system((char*)command);
    
    if (myIterator->callPageLevel02)
      myIterator->callPageLevel02(myIterator,local);
    
    if (myIterator->limit!=0)
      if (myIterator->limit<=index)
        break;
        
    if (myIterator->status!=0)
      break;
  }

  return NULL;
}

int mikaParserRunIterator( struct mikaParserIterator *myIterator )
{
  if (!myIterator)
    return 1;
  
  pthread_t t;
  pthread_create(&t,NULL,mikaParserRunIteratorThread,myIterator);
  
  return 0;
}

struct mikaParserIterator *mikaParserIteratorInit( )
{
  struct mikaParserIterator *myIterator = (struct mikaParserIterator*)malloc(sizeof(struct mikaParserIterator));
  myIterator->base[0] = 0x00;
  myIterator->root[0] = 0x00;
  myIterator->index = 0;
  myIterator->limit = 0;
  myIterator->status = 0;
  myIterator->callNextPage = NULL;
  myIterator->callPageLevel02 = NULL;
  myIterator->callPageLevel03 = NULL;
  myIterator->callPageExternal = NULL;
  return myIterator;
}

/**  locate a section in html data, then locate a line with an id of ident
 *   and place it's content into storage and return
 */
int mikaHtmlLineForSection( struct mikaParserIterator *myIterator,
  FILE *fd, unsigned char *starter, unsigned char *ident, unsigned char *storage )
{
  if (!myIterator)
    return 0;
  
  unsigned char line[4096*8];
  char *section = NULL;
  int mode = 0;
  
  while(feof(fd))
  {
    fgets((char*)line,(4096*8)-1,fd);
    if (feof(fd)) 
    {
      fclose(fd);
      break;
    }
    switch(mode)
    {
      case 0:
        if ((section = strstr((char*)line,(const char*)starter)))
          mode = 1;
      break;
      case 1:
        if ((section = strstr((char*)line,(const char*)ident)))
        {
          strcpy((char*)storage,(char*)line);
          return 1;
        }
      break;
    }
  }
  
  return 0;
}

#ifdef _STATIC
  
  void *callNextPage( struct mikaParserIterator *myIterator )
  {
    if (!myIterator)
      return NULL;
    
    unsigned char tmp[512];
    
    int n;
    for(n = 0; n < strlen((char*)myIterator->base); n++)
    {
      if (myIterator->base[n]=='=')
        break;
    }
    strncpy((char*)tmp,(char*)myIterator->base,n);
    myIterator->base[n+1] = 0x00;
    sprintf((char*)tmp,"%s%d",(char*)myIterator->base,myIterator->index);
    strcpy((char*)myIterator->base,(char*)tmp);
    
    #ifdef _DEBUG
      printf("DRAQP100 callNextPage index(%d) base(%s)\n",
        myIterator->index,(char*)myIterator->base);
    #endif
    
    myIterator->index++;
    
    return NULL;
  }
  
  void *callPageLevel02( struct mikaParserIterator *myIterator, unsigned char *filename )
  {
    if (!myIterator || !filename)
      return NULL;
    
    FILE *fd = NULL;
    unsigned char filenameEx[256];
    unsigned char line[4096*8];
    unsigned char name[128];
    unsigned char command[256];
    char *section = NULL;
    int sectionIndex = 0;
    int index = 0;
    int n;
    int linemode = 0;
    int start = 0;
    int length = 0;
    
    if ((fd = fopen((char*)filename,"r")))
    {
      while(1)
      {
        fgets((char*)line,(4096*8)-1,fd);
        if (feof(fd)) break;
        
        // locating section in page
        if ((section = strstr((const char*)line,"Venue-Result")))
        {
          sectionIndex = 1;
          continue;
        }
        
        if (sectionIndex==1)
        {
          // locating line in section
          if ((section = strstr((const char*)line,"<a id")))
          {
            // locating position in line
            if ((section = strstr((const char*)section,"href=\"")))
            {
              linemode = 0;
              start = 0;
              length = 0;
              for(n = 0; n < strlen((char*)section);n++)
              {
                switch(linemode)
                {
                  case 0:
                    if (section[n]=='"')
                    {
                      start = n+1;
                      linemode = 1;
                    }
                    break;
                  case 1:
                    if (section[n]=='"')
                      linemode = 2;
                    else
                      length++;
                    
                    break;
                }
              }
              if (length!=0)
              {
                strncpy((char*)name,section+start,length);
                name[length] = 0x00;
                #ifdef _DEBUG
                  printf("DRAQP100 callPageLevel02 found(%s)\n",(char*)name);
                #endif
                
                sprintf((char*)filenameEx,"%s%d.html",filename,index);
                
                sprintf((char*)command,"wget -o wget.log -O %s %s%s",
                  (char*)filenameEx,(char*)myIterator->root,(char*)name);
                system((char*)command);
                
                if (myIterator->callPageLevel03)
                  myIterator->callPageLevel03(myIterator,(char*)filenameEx);
                
                index++;
              }
            }
            sectionIndex = 0;
          }
        }
      }
      if (filename[0]!='/')
        remove((char*)filename);
      fclose(fd);
    }
    else
    {
      myIterator->status = 1;
    }
    
    return NULL;
  }
  
  void *callPageLevel03( struct mikaParserIterator *myIterator, unsigned char *filename )
  {
    if (!myIterator || !filename)
      return NULL;
    
    FILE *fd = NULL;
    unsigned char line[4096*8];
    unsigned char name[128];
    char *section = NULL;
    int linemode = 0;
    int start = 0;
    int length = 0;
    int n;
    
    if ((fd = fopen((char*)filename,"r")))
    {
      while(1)
      {
        fgets((char*)line,(4096*8)-1,fd);
        if (feof(fd)) break;
        
        if ((section = strstr((char*)line,"Website")))
        {
          if ((section = strstr((char*)line,"href=\"")))
          {
            linemode = 0;
            start = 0;
            length = 0;
            for(n = 0; n < strlen((char*)section);n++)
            {
                switch(linemode)
                {
                  case 0:
                    if (section[n]=='"')
                    {
                      start = n+1;
                      linemode = 1;
                    }
                    break;
                  case 1:
                    if (section[n]=='"')
                      linemode = 2;
                    else
                      length++;
                    
                    break;
                }
            }
            if (length>0)
            {
              strncpy((char*)name,section+start,length);
              name[length] = 0x00;
              #ifdef _DEBUG
                printf("DRAQP100 callPageLevel03 found(%s)\n",(char*)name);
              #endif
              name[length] = '\n';
              name[length+1] = 0x00;
              mikaParserLock(parser);
              parser->log = fopen("mikaParser.log","a");
              fwrite((char*)name,strlen((char*)name),1,parser->log);
              fclose(parser->log);
              mikaParserUnlock(parser);
            }
          }
        }
      }
      // remove the file, since we could eat much space
      if (filename[0] != '/')
        remove((char*)filename);
      fclose(fd);
    }
    else
    {
      myIterator->status = 1;
    }
    
    return NULL;
  }
  
  void *callPageExternal( struct mikaParserIterator *myIterator, unsigned char *url )
  {
    if (!myIterator || !url)
      return NULL;
    return NULL;
  }
  
  void runIteratorAtAddress( unsigned char *address, unsigned char *id )
  {
    static int myIndex = 1;
    
    if (!address)
      return;
        
    struct mikaParserIterator *iterator = mikaParserIteratorInit();
    iterator->callNextPage = callNextPage;
    iterator->callPageLevel02 = callPageLevel02;
    iterator->callPageLevel03 = callPageLevel03;
    iterator->index = myIndex;
    iterator->limit = 0;
    strcpy((char*)iterator->root,"http://www.clubplanet.com");
    strcpy((char*)iterator->id,id);
    strcpy((char*)iterator->base,address); 
    mikaParserRunIterator(iterator);
  }
  
  int main()
  {
  
    parser = mikaParserInit();
    parser->iterator->callNextPage = callNextPage;
    parser->iterator->callPageLevel02 = callPageLevel02;
    parser->iterator->callPageLevel03 = callPageLevel03;
    parser->iterator->index = 2;
    parser->iterator->limit = 0;
    parser->masterIndex = 1;
    strcpy((char*)parser->iterator->root,"http://www.clubplanet.com");
    
    runIteratorAtAddress("http://www.clubplanet.com/US/New-York/venues?currentPage=","NY");
    runIteratorAtAddress("http://www.clubplanet.com/US/Las-Vegas/venues?currentPage=","LV");
    runIteratorAtAddress("http://www.clubplanet.com/US/Miami/venues?currentPage=","MI");
    runIteratorAtAddress("http://www.clubplanet.com/US/Chicago/venues?currentPage=","CH");
    runIteratorAtAddress("http://www.clubplanet.com/US/Los-Angeles/venues?currentPage=","LA");
    runIteratorAtAddress("http://www.clubplanet.com/US/Boston/venues?currentPage=","BN");
    
    while(1)
    {
      usleep(750000);
    }
  
    return 0;
  }
#endif
