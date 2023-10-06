/** 
 *  this is a basic manager of a set of applications
 *  
 *  crated by - Milan Kazarka
 */
#include "mikaApps.h"
#include "config.h"

static mikaApps *globalApps = NULL;

mikaAppsDelegate::mikaAppsDelegate( )
{
  #ifdef _DEBUG
    printf("mikaAppsDelegate::mikaAppsDelegate\n");
  #endif
}

mikaAppsDelegate::~mikaAppsDelegate( )
{
}

mikaApp::mikaApp( unsigned char *appRoot )
{
  #ifdef _DEBUG
    printf("mikaApp::mikaApp\n");
  #endif
  if (!appRoot)
  {
    #ifdef _DEBUG
      printf("mikaApp::mikaApp warning application root not defined\n");
    #endif
  }
  else
  {
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
        #ifdef _DEBUG
          printf("mikaApp::mikaApp root entry(%s)\n",
            (char*)dp->d_name);
        #endif
        // testing purposes only
        strcpy((char*)myAppName,(char*)dp->d_name);
      }
    }
    else
    {
      #ifdef _DEBUG
        printf("mikaApp::mikaApp warning directory(%s) error\n",
          (char*)appRoot);
      #endif
    }
  }
  uid = new mikaUID();
}

mikaApp::~mikaApp( )
{
}

/** register all applications available in the appDirectory
 */
mikaApps::mikaApps( unsigned char *appDirectory )
{
  if (!appDirectory)
  {
    #ifdef _DEBUG
      printf("mikaApps::mikaApps warning, no apps root defined, using standard\n");
    #endif
    appDirectory = (unsigned char*)_MIKA_APP_APPS_ROOT;
  }
  if (globalApps)
  {
    #ifdef _DEBUG
      printf("mikaApps::mikaApps warning instance of apps already running\n");
    #endif
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
      #ifdef _DEBUG
        printf("mikaApps::mikaApps app root(%s)\n",
          (char*)appRoot);
      #endif
      app = new mikaApp((unsigned char*)appRoot);
      app->next = apps;
      apps = app;
    }
    closedir(dir);
  }
}

/** create mikaApps with manualy ading applications 
 *  later
 */
mikaApps::mikaApps( )
{
}

mikaApps::~mikaApps( )
{
  #ifdef _DEBUG
    printf("mikaApps::~mikaApps\n");
  #endif
}

/** run the managed application
 */
void *runChild( mikaApp *app, mikaApps *apps )
{
  #ifdef _DEBUG
    printf("runChild path(%s)\n",
      (char*)app->myAppRoot);
  #endif
  unsigned char *command = (unsigned char*)malloc(
    strlen((char*)app->myAppRoot)+
    strlen((char*)app->myAppName)
    );
  unsigned char output[4096];
  sprintf((char*)command,"%s/%s",
    (char*)app->myAppRoot,
    (char*)app->myAppName);
  #ifdef _DEBUG
    printf("runChild command(%s)\n",
      (char*)command);
  #endif
  FILE *inapp = popen((char*)command,"r");
  while(fgets((char*)output,4096,inapp))
  {
    #ifdef _DEBUG
      printf("%s",(char*)output);
    #endif
  }
  #ifdef _DEBUG
    printf("runChild finished\n");
  #endif
}

/** fork the child process of the application app
 *
 */
void *runManaged( mikaApp *app, mikaApps *apps )
{
  int pID = fork();
  if (pID == 0)
  {
    runChild(app,apps);
    exit(0);
  }
  else if (pID < 0)            // failed to fork
  {
    exit(1);
  }
  else                        // parent
  {
    #ifdef _DEBUG
      printf("runManaged managing pid(%d)\n",pID);
    #endif
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

/** remove an app from the list of processed apps
 */
int mikaApps::removeApp( mikaApp *app)
{
  if (!app)
    return 1;
  
  #ifdef _DEBUG
    printf("mikaApps::removeApp\n");
  #endif
  
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
  #ifdef _DEBUG
    printf("mikaAppsProcessing\n");
  #endif
  mikaApps *apps = (mikaApps*)data;
  mikaApp *current;
  int status;
  while(1)
  {
    pthread_mutex_lock(&apps->mutex);
    current = apps->apps;
    while(current)
    {
      if (waitpid(current->pID, &status, WNOHANG)!=0)
      {
        if (WIFEXITED(status)) {
          printf("mikaAppsProcessing process(%d) exited, status=%d\n", current->pID, WEXITSTATUS(status));
          apps->removeApp(current);
          break;
        } else if (WIFSIGNALED(status)) {
          printf("mikaAppsProcessing process(%d) killed by signal %d\n", current->pID, WTERMSIG(status));
          apps->removeApp(current);
          break;
        } else if (WIFSTOPPED(status)) {
          printf("mikaAppsProcessing process(%d) stopped by signal %d\n", current->pID, WSTOPSIG(status));
          apps->removeApp(current);
          break;
        } else if (WIFCONTINUED(status)) {
          printf("mikaAppsProcessing process(%d) continued\n",current->pID);
        }
      }
      current = current->next;
    }
    pthread_mutex_unlock(&apps->mutex);
    usleep(10000);
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
