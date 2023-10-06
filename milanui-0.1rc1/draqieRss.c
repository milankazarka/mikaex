/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "draqieRss.h"
#include "mikaServer.h"
#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#ifndef _DEBUG
  #define _DEBUG
#endif

struct FtpFile
{
  const char *filename;
  FILE *stream;
};

struct draqieRss *draqieRssInit( )
{
  struct draqieRss *myDraqieRss = (struct draqieRss*)malloc(sizeof(struct draqieRss));
  myDraqieRss->parseCall = NULL;
  myDraqieRss->server[0] = 0x00;
  myDraqieRss->remotePath[0] = 0x00;
  return myDraqieRss;
}

int draqieRssRelease( struct draqieRss *myDraqieRss )
{
  if (!myDraqieRss)
    return 1;
  
  free(myDraqieRss);
  
  return 0;
}

void *_parse_call_general( struct draqieRss *myDraqieRss )
{
  if (!myDraqieRss)
    return NULL;
  
  #ifdef _DEBUG
    printf("_parse_call_general\n");
  #endif  
  
  TiXmlDocument doc;
  doc.Parse(
    (const char*)myDraqieRss->data,
    0,
    TIXML_ENCODING_UTF8
  ); 
    
  return NULL;
}

/**
 *  parse fetched RSS data
 */
int draqieRssParse( struct draqieRss *myDraqieRss, unsigned char *local )
{
  if (!myDraqieRss || !local)
    return 1;
  
  unsigned char line[4096*8];
  FILE *fd;
  
  strcpy((char*)myDraqieRss->data," ");
  if ((fd=fopen((const char*)local,"rw")))
  {
    while(1)
    {
      fgets((char*)line,(4096*8)-1,fd);
      if (feof(fd))
      {
        fclose(fd);
        break;
      }
      
      if (strlen((char*)myDraqieRss->data)+strlen((char*)line)<1024*256)
        strcpy((char*)myDraqieRss->data,(char*)line);
      fclose(fd);
    }
  }
  
  if (myDraqieRss->parseCall)
    myDraqieRss->parseCall(myDraqieRss);
  else
    _parse_call_general(myDraqieRss);
    
  return 0;
}

static size_t my_fwrite( void *buffer, size_t size, size_t nmemb, void *stream )
{
  struct FtpFile *out=(struct FtpFile *)stream;
  if (out && !out->stream)
  {
    out->stream=fopen(out->filename,"wb");
    if (!out->stream)
      return -1;
  }
  return fwrite(buffer, size, nmemb, out->stream);
}

int draqieRssGetData( struct draqieRss *myDraqieRss, unsigned char *local )
{
  if (!myDraqieRss)
    return 1;
  
  if (
    myDraqieRss->server[0]==0x00 ||
    myDraqieRss->remotePath[0]==0x00)
  {
    #ifdef _DEBUG
      printf("draqieRssGetData error no server or remote path defined\n");
    #endif
    return 1;
  }
  
  curl_global_init(CURL_GLOBAL_DEFAULT);
  CURL *curl = curl_easy_init();
  if (!curl)
  {
    #ifdef _DEBUG
      printf("draqieRssGetData error could not init curl\n");
    #endif
    return 1;
  }
  
  FtpFile ftpFile;
  unsigned char url[1024];
  
  ftpFile.filename = (const char*)local;
  ftpFile.stream = NULL;
  
  sprintf((char*)url,"%s/%s",(char*)myDraqieRss->server,
                             (char*)myDraqieRss->remotePath);
  #ifdef _DEBUG
    printf("draqieRssGetData download(%s)\n",(char*)url);
  #endif
  
  curl_easy_setopt(curl, CURLOPT_URL,(char*)url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpFile);
  curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
  
  int res = curl_easy_perform(curl);
  curl_easy_cleanup(curl);
  curl_global_cleanup();
  
  if (CURLE_OK != res)
  {
    #ifdef _DEBUG
      fprintf(stderr, "draqieRssGetData curl failure(%d)\n",res);
    #endif
    return 1;
  }
    
  return 0;
}
