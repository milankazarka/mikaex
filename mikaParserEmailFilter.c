/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "mikaParserEmailFilter.h"
#define _STATIC

struct mikaParser *parser = NULL;
unsigned char currentUrl[526];

struct mikaParserSimpleEmail *mikaParserSimpleEmailInit( )
{
  struct mikaParserSimpleEmail *mySimpleEmail = (struct mikaParserSimpleEmail*)malloc(sizeof(struct mikaParserSimpleEmail));
  mySimpleEmail->address[0] = 0x00;
  mySimpleEmail->next = NULL;
  return mySimpleEmail;
}

int mikaParserSimpleEmailPresent( struct mikaParserSimpleEmail *mySimpleEmail, unsigned char *address )
{
  if (!mySimpleEmail || !address)
    return 0;
  
  struct mikaParserSimpleEmail *current = mySimpleEmail;
  while(current)
  {
    if (
      strlen((char*)current->address) == strlen((char*)address)
    )
    {
      if (strcmp((char*)current->address,(char*)address)==0)
        return 1;
    }
    current = current->next;
  }  
  
  return 0;
}

int mikaParserSimpleEmailValidate( unsigned char *address )
{
  int valid = 0;
  char *section = NULL;
  if ((section = strstr(address,"@")))
  {
    if (strlen(section)>3)
    {
      if ((section = strstr(section+2,".")))
        valid = 1;
    }
  }
  return valid;
}

struct mikaParserSimpleEmail *mikaParserSimpleEmailAdd( struct mikaParserSimpleEmail *mySimpleEmail, unsigned char *address )
{
  if (!address)
    return mySimpleEmail;
  
  if (mikaParserSimpleEmailValidate(address)==0)
    return mySimpleEmail;
  
  if (!mySimpleEmail)
  {
    mySimpleEmail = mikaParserSimpleEmailInit();
    strcpy(mySimpleEmail->address,(char*)address);
  }
  else
  {
    if (mikaParserSimpleEmailPresent(mySimpleEmail,address)==1)
      return mySimpleEmail;
    
    struct mikaParserSimpleEmail *current = mySimpleEmail;
    while(current)
    {
      if (!current->next)
      {
        struct mikaParserSimpleEmail *newEmail = mikaParserSimpleEmailInit();
        current->next = newEmail;
        strcpy(newEmail->address,(char*)address);
        printf("http://%s %s\n",(char*)currentUrl,(char*)address);
        break;
      }
      current = current->next;
    }
  }
  
  return mySimpleEmail;
}

struct mikaParserEmailFilter *mikaParserEmailFilterInit( )
{
  struct mikaParserEmailFilter *myEmailFilter;
  myEmailFilter = (struct mikaParserEmailFilter*)malloc(sizeof(struct mikaParserEmailFilter));
  myEmailFilter->infd = NULL;
  myEmailFilter->emails = NULL;
  return myEmailFilter;
}

int mikaParserEmailFilterRun( struct mikaParserEmailFilter *myEmailFilter )
{
  if (!myEmailFilter)
    return 1;
  
  if ((myEmailFilter->infd=fopen((char*)myEmailFilter->input,"r")))
  {
    unsigned char line[4096];
    unsigned char *section;
    while(1)
    {
      fgets((char*)line,4095,myEmailFilter->infd);
      if (line[strlen((char*)line)-1]=='\n')
        line[strlen((char*)line)-1] = 0x00;
      
      if (!strstr((char*)line,"@"))
        strcpy((char*)currentUrl,(char*)line);
      
      myEmailFilter->emails = mikaParserSimpleEmailAdd(myEmailFilter->emails,(unsigned char*)line);
      
      if (feof(myEmailFilter->infd))
        break;
    }
    fclose(myEmailFilter->infd);
  }
  
  return 0;
}

#ifdef _STATIC
  int main( )
  {
    struct mikaParserEmailFilter *myFilter = mikaParserEmailFilterInit();
    strcpy((char*)myFilter->input,"mikaParserFilter20100107.log");
    mikaParserEmailFilterRun(myFilter);
    return 0;
  }
#endif
