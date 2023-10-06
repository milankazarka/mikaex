/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

/** a non finished implementation to handle application launching, which
 *  should replace our current very simple system
 */

#include "app.h"

struct app *apps = NULL;

struct app *appNew( )
{
  struct app *myApp = NULL;
  
  myApp = (struct app*)malloc(sizeof(struct app));
  myApp->name[0] = 0x00;
  myApp->next = NULL;
  
  if (!apps)
    apps = myApp;
  else
  {
    myApp->next = apps;
    apps = myApp;
  }
  
  return myApp;
}

void appRelease( struct app *myApp )
{
  if (!myApp)
    return;
  
  free(myApp);
}

int appIsRegistered( unsigned char *name )
{
  struct app *current = NULL;
  
  if (!name)
    return 0;
  
  current = apps;
  while(current)
  {
    if (current->name[0]!=0x00)
      if (strcmp((char*)name,(char*)current->name)==0)
        return 1;
    current = current->next;
  }
  return 0;
}

void appListApplications( )
{
  #ifdef _DEBUG
    struct app *current = apps;
    while(current)
    {
      if (current->name[0]!=0x00)
      {
        fprintf(stderr,"appListApplications app(%s)\n",
          (char*)current->name);
      }
      current = current->next;
    }
  #endif
}

/** register all applications
 */
void appRegisterAll( )
{
  struct app *current = NULL;  
  struct dirent *dp;  
  DIR *dir = opendir(_APPS_DIRECTORY);
  unsigned char path[256];
  
  if (!dir)
  {
    #ifdef _DEBUG
      fprintf(stderr,
        "appRegisterAll error no applications directory\n");
    #endif
    return;
  }  
  
  while((dp=readdir(dir)) != NULL)
  {
    if (dp->d_name[0]=='.')
      continue;
    
    if (appIsRegistered((unsigned char*)dp->d_name))
      continue;
      
    sprintf((char*)path,"%s/%s/app.bin",_APPS_DIRECTORY,(char*)dp->d_name);
    if( access((char*)path, F_OK ) != -1 )
    {
      current = appNew();
      strcpy((char*)current->name,(char*)dp->d_name);
    } 
    else 
    {
      #ifdef _DEBUG
        fprintf(stderr,
          "appRegisterAll warning application %s missing binary\n",
            (char*)dp->d_name);
      #endif
    }
  }
  closedir(dir);
  appListApplications();
}
