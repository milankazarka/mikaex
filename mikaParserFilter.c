/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

/**
 *  ./mikaParserFilter ?input ?output
 */
#define _STATIC
#include "mikaParser.h"
#include "mikaParserFilter.h"

#define _PRINT_URL_NO
#define _RULES_ACCEPT
#define _RULES_REJECT

struct mikaParser *parser = NULL;
struct mikaParserFilter *filter = NULL;
static begin = 0;

struct mikaParserFilter *mikaParserFilterInit( )
{
  struct mikaParserFilter *myFilter = (struct mikaParserFilter*)malloc(sizeof(struct mikaParserFilter));
  myFilter->configuration = NULL;
  myFilter->callFilterElement = NULL;
  myFilter->callFilterElementProcess = NULL;
  myFilter->fdIterate = NULL;
  myFilter->email = NULL;
  return myFilter;
}

int mikaParserFilterIterateFile( struct mikaParserFilter *myFilter, unsigned char *filename )
{
  if (!myFilter || !filename)
    return 1;
  
  unsigned char line[4096];
  char *section;
  int nelement = 0;
  
  if ((myFilter->fdIterate = fopen((char*)filename,"r")))
  {
    while(1)
    {
      fgets((char*)line,4095,myFilter->fdIterate);
      if (line[strlen((char*)line)-1]=='\n')
        line[strlen((char*)line)-1] = 0x00;
        
      #ifdef _DEBUG
        printf("DRAQF100 mikaParserFilterIterateFile element(%s)\n",(char*)line);
      #endif
      if (nelement>begin)
        if (myFilter->callFilterElement)
          myFilter->callFilterElement(myFilter,(unsigned char*)line);
      
      nelement++;
      if (feof(myFilter->fdIterate)) break;
    }
    fclose(myFilter->fdIterate);
  }
    
  return 0;
}

int mikaParserFilterFetchUrl( struct mikaParserFilter *myFilter, unsigned char *element, 
  unsigned char *url )
{
  if (!myFilter || !url)
    return 1;
  
  char *section = NULL;
  unsigned char *position = element;
  
  while(1)
  {
    if ((section = strstr((char*)position,"http://")))
    {
      position=section+7;
      #ifdef _DEBUG
        printf("DRAQF100 mikaParserFilterFetchUrl url(%s)\n",position);
      #endif
    }
    else
      break;
  }
  
  if (strlen((char*)position)==0) return 1;
  if (position[0]==' ') position++;
  if ((section = strstr((char*)position," "))) return 1;
  if (!(section = strstr((char*)position,"."))) return 1;
  
  strcpy((char*)url,(char*)position);
    
  return 0;
}

int mikaParserFilterExtractEmailFromData( struct mikaParserFilter *myFilter, unsigned char *data )
{
  if (!myFilter || !data)
    return 1;
  
  return 0;
}

/**
 *  skeleton for parsing a file and fetching the email addresses
 */
int mikaParserFilterExtractEmails( struct mikaParserFilter *myFilter, unsigned char *filename )
{
  if (!myFilter || !filename)
    return 1;
  
  FILE *fd;
  unsigned char line[4096*8];
  char *section;
  
  if ((fd = fopen((char*)filename,"r")))
  {
    while(1)
    {
      fgets((char*)line,(4096*8)-1,fd);
      if (line[strlen((char*)line)-1]=='\n')
        line[strlen((char*)line)-1] = 0x00;
      
      if ((section = strstr((char*)line,"@")))
      {
      }
        
      if (feof(fd)) break;
    }
    fclose(fd);
    
    remove(filename);
  }
  
  return 0;
}

struct mikaParserFilterEmail *mikaParserFilterEmailInit( )
{
  struct mikaParserFilterEmail *email = (struct mikaParserFilterEmail*)malloc(sizeof(struct mikaParserFilterEmail));
  email->address[0] = 0x00;
  return email;
}

int mikaParserFilterEmailInList( struct mikaParserFilterEmail *email, unsigned char *address )
{
  if (!email || !address)
    return 0;
  return 0;
}

/** check if an email address has the correct format
 */
int mikaParserFilterEmailIsCorrect( unsigned char *address )
{
  if (!address)
    return 0;
  
  int n;
  int mode = 0;
  for(n = 0; n < strlen((char*)address); n++)
  {
    if (address[n]=='@') mode = 1;
    if (address[n]=='.' && mode==1) return 1;
  }
    
  return 0;
}

#ifdef _STATIC
  
  static unsigned char *input = NULL;
  static unsigned char *output = NULL;
  
  void *callFilterElement( struct mikaParserFilter *myFilter, unsigned char *element )
  {
    unsigned char url[4096];
    static int run = 0;
    if (!myFilter || !element)
      return NULL;
    
    if (mikaParserFilterFetchUrl(myFilter,element,(unsigned char*)url)==0)
    {
      #ifdef _DEBUG
        printf("DRAQF100 callFilterElement run(%d)\n",run);
      #endif
      #ifdef _PRINT_URL
        printf("%s\n",(char*)url);
      #endif
      if (myFilter->callFilterElementProcess)
        myFilter->callFilterElementProcess(myFilter,(unsigned char*)url);
    }
    
    run++;
    
    return NULL;
  }
  
  static int thread_counter = 0;
  static int run = 0;
  
  #define _SECURE
  #define _SELFPARSE_NO
  void *callFilterElementProcessThread( void *data )
  {
    if (!data)
      return NULL;
    
    unsigned char *url = (unsigned char*)data;  
    int myid;
    
    int n;
    for (n = 0; n < strlen((char*)url); n++)
    {
      if (url[n]==' ')
      {
        #ifdef _SECURE
          free(url);
        #endif
        return NULL;
      }
    }
    
    mikaParserLock(parser);
    thread_counter++;
    if (run==0) run = begin;
    run++;
    myid = run;
    
    unsigned char blackbox[256];
    sprintf((char*)blackbox,"echo %d > mikaParserFilterBlackbox.log",run);
    system((char*)blackbox);
    
    #ifdef _DEBUG
      printf("DRAQF100 callFilterElementProcessThread start(%d)\n",myid);
    #endif
    
    #ifdef _SELFPARSE
      unsigned char line[4096*8];
    #endif
    unsigned char filename[256];
    unsigned char command[4096];
    char *section = NULL;
    FILE *fd;
    sprintf((char*)filename,"pages/page%d.html",myid);
    sprintf((char*)command,"wget -q -Q50m --timeout=15 -l2 --ignore-case -r -O %s --accept=*php*,*html*,*aspx*,*asp* %s",
      (char*)filename,(char*)url);
    mikaParserUnlock(parser);
    system((char*)command);
    
    mikaParserLock(parser);
    sprintf((char*)command,"echo %s",(char*)url);
    system((char*)command);
    sprintf((char*)command,"perl -wne'while(/[\\w\\.]+@[\\w\\.]+\\w+/g){print \"$&\\n\"}' %s",
      (char*)filename);
    system((char*)command);
    FILE *fp = popen((char*)command, "r");
    unsigned char output[1024];
    if (fp)
    {
       while (fgets((char*)output, 1024, fp) != NULL) 
       {
         if (output[strlen((char*)output)-1]=='\n') output[strlen((char*)output)-1] = 0x00;
         sprintf((char*)command,"echo %s >> mikaParserFilter.log",(char*)output);
         printf("command(%s)\n",(char*)command);
         system((char*)command);
       }
       pclose(fp);
    }
    mikaParserUnlock(parser);
    
    #ifdef _SELFPARSE
      mikaParserFilterExtractEmails( filter, (unsigned char*)filename );
    #endif
            
    if ((fd = fopen((char*)filename,"r")))
    {
      #ifdef _SELFPARSE
        while(1)
        {
          fgets((char*)line,(4096*8)-1,fd);
          if (line[strlen((char*)line)-1]=='\n')
            line[strlen((char*)line)-1] = 0x00;
      
          if ((section = strstr((char*)line,"@")))
          {
            #ifdef _DEBUG
              printf("DRAQF100 callFilterElementProcessThread hit(%s)\n",(char*)url);
            #endif
          }
        
          if (feof(fd)) break;
        }
      #endif
      fclose(fd);
    
      remove(filename);
    }
    
    thread_counter--;
    #ifdef _DEBUG
      printf("DRAQF100 callFilterElementProcessThread end(%d)\n",myid);
    #endif
    
    #ifdef _SECURE
      free(url);
    #endif 
    
    return NULL;
  }
  
  #define _TIMEOUT 15
  #define _THREADS 5
  #define _MAXTHREADS 20
  #define _THREADWAIT 1
  void *callFilterElementProcess( struct mikaParserFilter *myFilter, unsigned char *url )
  {
    if (!myFilter || !url)
      return NULL;
    
    #ifdef _DEBUG
      printf("DRAQF100 callFilterElementProcess %s\n",url);
    #endif
    int counter = 0;
    while(1)
    {
      if (
        (thread_counter>=_THREADS && counter<_TIMEOUT) &&
        thread_counter<_MAXTHREADS)
      {
        counter++;
        sleep(1);
      }
      else if (thread_counter>=_MAXTHREADS)
        sleep(1);
      else
        break;
    }
    
    unsigned char *myUrl;
    myUrl = malloc(strlen(url)+1);
    sprintf(myUrl,url);
    pthread_t id;
    pthread_create(&id,NULL,callFilterElementProcessThread,(unsigned char*)myUrl);
    pthread_detach(id);
    
    sleep(_THREADWAIT);
    
    return NULL;
  }
  
  void _termination_handler( )
  { exit(0); }
  
  void _segfault_handler( )
  { exit(0); }
  
  int mikaParserParseBlackbox( struct mikaParser *myParser, unsigned char *filename )
  {
    FILE *fd;
    unsigned char line[256];
    if ((fd = fopen((char*)filename,"r")))
    {
      fgets((char*)line,255,fd);
      if (line[strlen((char*)line)]=='\n')
        line[strlen((char*)line)] = 0x00;
      begin = atoi((char*)line);
      fclose(fd);
    }
    return 0;
  }
  
  int main( int argc, char *argv[] )
  {
    if (argc>1) input = argv[1];
    else if (argc>2) output = argv[2];
  
    signal(SIGHUP,_termination_handler);
    signal(SIGINT,_termination_handler);
    signal(SIGTERM,_termination_handler);
    signal(SIGSEGV,_segfault_handler);
  
    parser = mikaParserInit();
    mikaParserParseBlackbox(parser,"mikaParserFilterBlackbox.log");
    filter = mikaParserFilterInit();
    filter->callFilterElement = callFilterElement;
    filter->callFilterElementProcess = callFilterElementProcess;
    
    if (input)		mikaParserFilterIterateFile(filter,input);
    else		mikaParserFilterIterateFile(filter,"mikaParser.log");
    
    system("echo end");
    while(1) 
    { 
      if (thread_counter==0)
        break;
      sleep(1); 
    }
    
    return 0;
  }
#endif
