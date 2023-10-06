/**  
  Copyright (C) 2012 Milan Kazarka
  milan.kazarka.office@gmail.com
      
  You may distribute under the terms of the Artistic License, 
  as specified in the README file. You can find a copy of the
  license at LICENSE.
*/

/** 
 *  this is a basic manager of a set of applications
 *  
 */
#include "mikaApps.h"
#include "microDebug.h"
#include "config.h"

static mikaApps *globalApps = NULL;

mikaAppsDelegate::mikaAppsDelegate( )
{
  mprintf("mikaAppsDelegate::mikaAppsDelegate\n");
}

mikaAppsDelegate::~mikaAppsDelegate( )
{
}

mikaApp::mikaApp( unsigned char *appRoot )
{
  mprintf("mikaApp::mikaApp\n");
  if (!appRoot)
  {
    mprintf("mikaApp::mikaApp warning application root not defined\n");
  }
  else
  {
    delegate = NULL;
    strcpy((char*)myAppRoot,(char*)appRoot);
    strcpy((char*)myAppName," ");
    struct dirent *dp;
    DIR *dir = opendir((char*)appRoot);
    if (dir)
    {
      while((dp = readdir(dir)) != NULL)
      {
        if (dp->d_name[0]=='.')
          continue;
        mprintf("mikaApp::mikaApp root entry(%s)\n",
          (char*)dp->d_name);
        // testing purposes only
        strcpy((char*)myAppName,(char*)dp->d_name);
      }
    }
    else
    {
      mprintf("mikaApp::mikaApp warning directory(%s) error\n",
        (char*)appRoot);
    }
  }
  uid = new mikaUID();
  pID = 0;
}

mikaApp::mikaApp( unsigned char *command, int root )
{
  mprintf("mikaApp::mikaApp %s\n",(char*)command);
  delegate = NULL;
  myAppRoot[0] = 0x00;
  strcpy((char*)myAppName,(char*)command);
  uid = new mikaUID();
  pID = 0;
}

mikaApp::~mikaApp( )
{
  delete uid;
}

int mikaApp::onFinished( )
{
  mprintf("mikaApp::onFinished\n");
  if (delegate)
  {
    if (delegate->onFinish)
    {
      delegate->onFinish(this);
    }
  }
  return 0;
}

/** register all applications available in the appDirectory
 */
mikaApps::mikaApps( unsigned char *appDirectory )
{
  mprintf("mikaApps::mikaApps\n");
  if (!appDirectory)
  {
    mprintf("mikaApps::mikaApps warning, no apps root defined, using standard\n");
    appDirectory = (unsigned char*)_MIKA_APP_APPS_ROOT;
  }
  if (globalApps)
  {
    mprintf("mikaApps::mikaApps warning instance of apps already running\n");
  }
  globalApps = this;
  
  pthread_mutex_init(&mutex,NULL);
  sprintf((char*)myAppDirectory,"%s",appDirectory);
  apps = NULL;
  struct dirent *dp;
  unsigned char appRoot[_MIKA_APP_NAMELEN*2];
  mikaApp *app = NULL;
  DIR *dir = opendir((char*)myAppDirectory);
  if (dir)
  {
    while((dp = readdir(dir)) != NULL)
    {
      if (dp->d_name[0]=='.')
        continue;
      sprintf((char*)appRoot,"%s/%s",(char*)myAppDirectory,(char*)dp->d_name);
      mprintf("mikaApps::mikaApps app root(%s)\n",
        (char*)appRoot);
      app = new mikaApp((unsigned char*)appRoot);
      app->next = apps;
      apps = app;
    }
    closedir(dir);
  }
  parentPID = getpid();
}

/** create mikaApps with manualy ading applications 
 *  later
 */
mikaApps::mikaApps( )
{
  globalApps = this;
  
  pthread_mutex_init(&mutex,NULL);
  myAppDirectory[0] = 0x00;
  apps = NULL;
  parentPID = getpid();
}

mikaApps::~mikaApps( )
{
  mprintf("mikaApps::~mikaApps\n");
}

int mikaApps::addApp( mikaApp *app, int sync )
{
  mprintf("mikaApps::addApp\n");
  if (sync==1) pthread_mutex_lock(&mutex);
  app->next = apps;
  apps = app;
  if (sync==1) pthread_mutex_unlock(&mutex);
  return 0;
}

/** run the managed application
 */
void *runChild( mikaApp *app, mikaApps *apps )
{
  #ifdef _DEBUG
    if (app->myAppRoot[0]!=0x00)
      printf("runChild path(%s)\n",
        (char*)app->myAppRoot);
  #endif
  unsigned char output[4096];
  unsigned char *command = NULL;
  /** run an app from a defined directory:
   */
  if (app->myAppRoot[0] != 0x00)
  {
    command = (unsigned char*)malloc(
      strlen((char*)app->myAppRoot)+
      strlen((char*)app->myAppName)+2
      );
  
    sprintf((char*)command,"%s/%s",
      (char*)app->myAppRoot,
      (char*)app->myAppName);
  }
  /** make system search for the app
   */
  else
  {
    command = (unsigned char*)malloc(
      strlen((char*)app->myAppName)+2
      );
  
    sprintf((char*)command,"%s",
      (char*)app->myAppName);
  }
  mprintf("runChild command(%s)\n",
    (char*)command);
  if (1)
  {
    FILE *inapp = popen((char*)command,"r");
    while(fgets((char*)output,4096,inapp))
    {
      printf("%s",(char*)output);
    }
    pclose(inapp);
  }
  mprintf("runChild finished %d\n",getpid());
}

void _terminationHandlerEx( int signal )
{
  mprintf("_terminationHandler signal(%d) mikaApps pid(%d) parent(%d)\n",
    signal,getpid(),globalApps->parentPID);
  if (signal!=17)
    exit(0);
}

/** fork the child process of the application app
 *
 */
void *runManaged( mikaApp *app, mikaApps *apps )
{ 
  mprintf("runManaged app(%s)\n",
      (char*)app->uid->uuid);
  int ppid = getpid();
  int pID = fork();
  if (pID == 0)
  {
    signal(SIGHUP,_terminationHandlerEx);
    signal(SIGINT,_terminationHandlerEx);
    signal(SIGTERM,_terminationHandlerEx);
    signal(SIGCHLD,_terminationHandlerEx);
    signal(SIGKILL,_terminationHandlerEx);
    apps->parentPID = ppid;
    globalApps = apps;
    runChild(app,apps);
    exit(0);
  }
  else if (pID < 0)            // failed to fork
  {
    exit(1);
  }
  else                        // parent
  {
    mprintf("runManaged managing pid(%d)\n",pID);
    app->pID = pID;
  }
  return NULL;
}

/** run all applications
 */
int mikaApps::runAll( )
{
  mikaApp *current = apps;
  while(current)
  {
    runManaged(current,this);
    current = current->next;
  }
  return 0;
}

int mikaApps::runApp( mikaApp *app)
{
  mprintf("mikaApps::runApp uuid(%s)\n",
    app->uid->uuid);
  runManaged(app,this);
  return 0;
}

/** remove an app from the list of processed apps
 */
int mikaApps::removeApp( mikaApp *app)
{
  if (!app)
    return 1;
  
  mprintf("mikaApps::removeApp\n");
  
  mikaApp *current = apps;
  mikaApp *last = NULL;
  mikaApp *next = NULL;
  while(current)
  {
    next = current->next;
    if (current==app)
    {
      if (last)
        last->next = next;
      else
        apps = next;
      mprintf("mikaApps::removeApp uuid(%s)\n",
        (char*)current->uid->uuid);
      delete current;
      return 0;
    }
    last = current;
    current = next;
  }
  
  return 0;
}

/** application processing thread
 */
void *mikaAppsProcessing(void*data)
{
  if (!data)
    return NULL;
    
  mprintf("mikaAppsProcessing\n");
  mikaApps *apps = (mikaApps*)data;
  mikaApp *current;
  int status;
  int n;
  while(1)
  {
    pthread_mutex_lock(&apps->mutex);
    current = apps->apps;
    while(current)
    {
      if (current->pID==0)
      {
        usleep(_MIKA_APPS_CLOCKTIK);
        current = current->next;
        continue;
      }
      if (0) printf("mikaAppsProcessing getpid(%d)\n",getpid());
      if ( waitpid(current->pID, &status, WNOHANG) != 0)
      {
        if (WIFEXITED(status)) {
          printf("mikaAppsProcessing process(%d) exited, status=%d\n", current->pID, WEXITSTATUS(status));
          current->onFinished();
          apps->removeApp(current);
          break;
        } else if (WIFSIGNALED(status)) {
          printf("mikaAppsProcessing process(%d) killed by signal %d\n", current->pID, WTERMSIG(status));
          current->onFinished();
          apps->removeApp(current);
          break;
        } else if (WIFSTOPPED(status)) {
          printf("mikaAppsProcessing process(%d) stopped by signal %d\n", current->pID, WSTOPSIG(status));
          current->onFinished();
          apps->removeApp(current);
          break;
        } else if (WIFCONTINUED(status)) {
          printf("mikaAppsProcessing process(%d) continued\n",current->pID);
        }
      }
      current = current->next;
      n++;
    }
    pthread_mutex_unlock(&apps->mutex);
    usleep(_MIKA_APPS_CLOCKTIK);
  }
}

/** process the states of child processes
 */
int mikaApps::runProcessing( )
{
  pthread_t t;
  pthread_create(&t,NULL,mikaAppsProcessing,this);
  return 0;
}

int mikaApps::stopAll( )
{
  mprintf("mikaApps::stopAll\n");
  pthread_mutex_lock(&mutex);
  mikaApp *current = apps;
  while(current)
  {
    mprintf("mikaApps::kill pid(%d)\n",current->pID);
    kill(current->pID,2);
    current = current->next;
  }
  pthread_mutex_unlock(&mutex);
  return 0;
}

/** this is a function to kill child processes of a parent
 *  process in case we don't know their PID. This is Linux
 *  speciffic, since it uses an external command's output.
 */
int mikaApps::stopChildrenNamed( unsigned char *name )
{
  mprintf("mikaApps::stopChildrenNamed name(%s) pid(%d)\n",
    (char*)name,getpid());
  FILE *po = NULL;
  unsigned char output[1024];
  unsigned char complete[4096];
  unsigned char command[1024];
  
  sprintf((char*)command,"ps -ejH | grep %d",getpid());
  
  po = popen((char*)command,"r");
  if (po)
  {
    strcpy((char*)complete,"");
    while(fgets((char*)output,4096,po))
    {
      mprintf("\tout: %s\n",(char*)output);
      strcat((char*)complete,(char*)output);
      if (strstr((char*)output,(char*)name))
      {
        mprintf("\thit(%s)\n",(char*)output);
        int n;
        int end = 0;
        int start = -1;
        for(n = 0; n < strlen((char*)output); n++)
        {
          if (output[n]!=' ')
          {
            if (start==-1)
              start = n;
          }
          else
          {
            if (start!=-1)
            {
              end = n;
              break;
            }
          }
        }
        if (start!=-1 && end!=0)
        {
          unsigned char schild[16];
          memcpy((char*)schild,output+start,end-start);
          schild[end-start] = 0x00;
          printf("\tkill(%s) start(%d) end(%d)\n",
            (char*)schild,start,end);
          kill(atoi((char*)schild),SIGTERM);
        }
      }
    }
    pclose(po);
  }
  
  return 0;
}
