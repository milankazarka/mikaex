/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "mikaRss.h"
#include "mikaServer.h"
#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"

#include "libMrss/mrss.h"

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

struct mikaRss *mikaRssInit( )
{
  struct mikaRss *myMikaRss = (struct mikaRss*)malloc(sizeof(struct mikaRss));
  myMikaRss->parseCall = NULL;
  myMikaRss->server[0] = 0x00;
  myMikaRss->remotePath[0] = 0x00;
  return myMikaRss;
}

int mikaRssRelease( struct mikaRss *myMikaRss )
{
  if (!myMikaRss)
    return 1;
  
  free(myMikaRss);
  
  return 0;
}

void *_parse_call_general( struct mikaRss *myMikaRss )
{
  if (!myMikaRss)
    return NULL;
  
  #ifdef _DEBUG
    printf("_parse_call_general\n");
  #endif  
  
  TiXmlDocument doc;
  doc.Parse(
    (const char*)myMikaRss->data,
    0,
    TIXML_ENCODING_UTF8
  ); 
    
  return NULL;
}

/**
 *  parse fetched RSS data
 */
int mikaRssParse( struct mikaRss *myMikaRss, unsigned char *local )
{
  if (!myMikaRss || !local)
    return 1;
  
  unsigned char line[4096*8];
  FILE *fd;
  
  strcpy((char*)myMikaRss->data," ");
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
      
      if (strlen((char*)myMikaRss->data)+strlen((char*)line)<1024*256)
        strcpy((char*)myMikaRss->data,(char*)line);
      fclose(fd);
    }
  }
  
  if (myMikaRss->parseCall)
    myMikaRss->parseCall(myMikaRss);
  else
    _parse_call_general(myMikaRss);
    
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

int mikaRssGetData( struct mikaRss *myMikaRss, unsigned char *local )
{
  if (!myMikaRss)
    return 1;
  
  if (
    myMikaRss->server[0]==0x00 ||
    myMikaRss->remotePath[0]==0x00)
  {
    #ifdef _DEBUG
      printf("mikaRssGetData error no server or remote path defined\n");
    #endif
    return 1;
  }
  
  curl_global_init(CURL_GLOBAL_DEFAULT);
  CURL *curl = curl_easy_init();
  if (!curl)
  {
    #ifdef _DEBUG
      printf("mikaRssGetData error could not init curl\n");
    #endif
    return 1;
  }
  
  FtpFile ftpFile;
  unsigned char url[1024];
  
  ftpFile.filename = (const char*)local;
  ftpFile.stream = NULL;
  
  sprintf((char*)url,"%s/%s",(char*)myMikaRss->server,
                             (char*)myMikaRss->remotePath);
  #ifdef _DEBUG
    printf("mikaRssGetData download(%s)\n",(char*)url);
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
      fprintf(stderr, "mikaRssGetData curl failure(%d)\n",res);
    #endif
    return 1;
  }
    
  return 0;
}
