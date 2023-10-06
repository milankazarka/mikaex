/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "mikaConfigServerMod.h"
#include "mikaServer.h"
#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"

/** Server mod prototype, with the help of which we retrieve
 *  configuration from the server
 */

pthread_mutex_t mikaConfigServerMutex;

void *_process_data_config_server( unsigned char *in, unsigned char *out )
{
  static int run = 0;
  static unsigned char activityPath[_ACTIVITY_PATH_LEN];
  
  if (!in || !out)
    return NULL;
  
  #ifdef _DEBUG
    printf("_process_data_config_server\n");
  #endif  
  
  if (run==0)
    pthread_mutex_init(&mikaConfigServerMutex,NULL);
  
  pthread_mutex_lock(&mikaConfigServerMutex);
  TiXmlDocument doc;
  doc.Parse(
    (const char*)in,
    0, TIXML_ENCODING_UTF8
  );
  TiXmlElement *root = doc.FirstChildElement("msg");
  if (root)
  {
    TiXmlElement *config = root->FirstChildElement("config");
    if (config)
    {
      if (strlen(config->GetText())+strlen(_ACTIVITIES_ROOT)+2<_ACTIVITY_PATH_LEN)
      {
        sprintf((char*)activityPath,"%s/%s",_ACTIVITIES_ROOT,config->GetText());
        mikaConfigServerDirectoryListing((unsigned char*)activityPath,out);
      }
    }
  }
  
  run++;
  pthread_mutex_unlock(&mikaConfigServerMutex);
    
  return NULL;
}

int mikaConfigServerDirectoryListing( unsigned char *path, unsigned char *out )
{
  if (!path || !out)
    return 1;
  
  static unsigned char filename[_ACTIVITY_PATH_LEN+512];
  
  #ifdef _DEBUG
    printf("mikaConfigServerDirectoryListing path(%s)\n",(char*)path);
  #endif
  
  struct dirent *dp;
  DIR *dir = opendir((const char*)path);  
  if (!dir)
    return 1;
  while((dp=readdir(dir)) != NULL)
  {
    if (strcmp(dp->d_name,".")==0 ||
      strcmp(dp->d_name,"..")==0)
      continue;
    if (strlen(dp->d_name)+strlen((char*)path)+2<_ACTIVITY_PATH_LEN+512)
    {
      sprintf((char*)filename,"%s/%s",(char*)path,dp->d_name);
      #ifdef _DEBUG
        printf("mikaConfigServerDirectoryListing file(%s)\n",(char*)filename);
      #endif
    }
  }  
  closedir(dir);
    
  return 0;
}
