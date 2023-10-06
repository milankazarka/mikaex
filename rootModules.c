/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

/** todo - create a better way of starting and stopping applications, which
 *  use the rootController
 */

#include "rootModules.h"

struct rootModules *rootModulesNew( )
{
  struct rootModules *myModules = (struct rootModules*)malloc(sizeof(struct rootModules));
  return myModules;
}

/** not finished in current version
 */
int rootModulesRun( struct rootModules *myModules )
{
  if (!myModules)
    return 1;
  
  struct dirent *dp;
  DIR *dir = opendir("./modules");  
  unsigned char path[256];
  
  if (dir)
  {
    while((dp=readdir(dir)) != NULL)
    {
      if (dp->d_name[0]=='.')
        continue;
      sprintf((char*)path,"./modules/%s &",(char*)dp->d_name);
      printf("rootModulesRun command(%s)\n",(char*)path);
      system((char*)path);
    }
  
    closedir(dir);
  }
    
  return 0;
}
