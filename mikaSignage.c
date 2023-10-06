/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "mikaSignage.h"
#include "mikaConfig.h"

struct mikaSignage *signage = NULL;
struct mikaParser *parser = NULL;
struct mikaSignageMutex *signageMutex;
struct mikaSignageSem *signageSem;
struct mikaSignageWindow *window = NULL;

struct mikaSignage *mikaSignageInit( )
{
  struct mikaSignage *myMikaSignage = (struct mikaSignage*)malloc(sizeof(struct mikaSignage));
  curl_global_init(CURL_GLOBAL_DEFAULT);
  myMikaSignage->curl = curl_easy_init(); 
  myMikaSignage->ftpFile.filename = (const char*)malloc(256);
  myMikaSignage->ftpFile.stream = NULL;
  myMikaSignage->configuration = mikaSignageConfigurationInit();
  return myMikaSignage;
}

int mikaSignageRelease( struct mikaSignage *myMikaSignage )
{
  if (!myMikaSignage)
    return 1;
  curl_global_cleanup();
  return 0;
}

static size_t my_fwrite( void *buffer, size_t size, size_t nmemb, void *stream )
{
  struct FtpFile *out=(struct FtpFile *)stream;
  if(out && !out->stream) 
  {
    out->stream=fopen(out->filename, "wb");
    if(!out->stream)
      return -1;
  }
  return fwrite(buffer, size, nmemb, out->stream);
}

int mikaSignageDownload( struct mikaSignage *myMikaSignage, unsigned char *filename )
{
  static unsigned char url[1024];
  
  if (!myMikaSignage || !filename)
    return 1;
  
  sprintf((char*)url,"%s/%s/%s",(char*)myMikaSignage->server,
                                (char*)myMikaSignage->directory,
                                filename);
  sprintf((char*)myMikaSignage->ftpFile.filename,(const char*)filename);
  
  #ifdef _DEBUG
    printf("mikaSignageDownload download(%s)\n",(char*)url);
  #endif
  
  curl_easy_setopt(myMikaSignage->curl, CURLOPT_URL,(char*)url);
  curl_easy_setopt(myMikaSignage->curl, CURLOPT_WRITEFUNCTION, my_fwrite); 
  curl_easy_setopt(myMikaSignage->curl, CURLOPT_WRITEDATA, &myMikaSignage->ftpFile);
  curl_easy_setopt(myMikaSignage->curl, CURLOPT_VERBOSE, 1L);
    
  int res = curl_easy_perform(myMikaSignage->curl);
  curl_easy_cleanup(myMikaSignage->curl);
                  
  if(CURLE_OK != res)
    fprintf(stderr, "mikaSignageDownload curl failure(%d)\n", res);                                      
  
  return 0;
}

int mikaSignageSetServer( struct mikaSignage *myMikaSignage, unsigned char *server )
{
  if (!myMikaSignage || !server)
    return 1;
  
  if (strlen((const char*)server)>255)
    return 1;  
  
  strcpy((char*)myMikaSignage->server,(char*)server);
    
  return 0;
}

int mikaSignageSetDirectory( struct mikaSignage *myMikaSignage, unsigned char *directory )
{
  if (!myMikaSignage || !directory)
    return 1;
  
  if (strlen((const char*)directory)>255) return 1;
  
  strcpy((char*)myMikaSignage->directory,(char*)directory);
    
  return 0;
}

struct mikaSignageConfiguration *mikaSignageConfigurationInit( )
{
  struct mikaSignageConfiguration *myConfiguration = (struct mikaSignageConfiguration*)malloc(
    sizeof(struct mikaSignageConfiguration)
  );
  myConfiguration->content = NULL;
  return myConfiguration;
}

/** parse xml dom configuration retrieved from server
 */
int mikaSignageConfigurationParse( struct mikaSignage *myMikaSignage )
{
  if (!myMikaSignage)
    return 1;
  return 0;
}

int mikaSignageConfigurationGet( struct mikaSignage *myMikaSignage )
{
  if (!myMikaSignage)
    return 1;
  #ifdef _DEBUG
    printf("mikaSignageConfigurationGet\n");
  #endif
  return 0;
}

struct mikaSignageContent *mikaSignageContentInit( )
{
  struct mikaSignageContent *myContent = (struct mikaSignageContent*)malloc(sizeof(struct mikaSignageContent));
  myContent->id[0] = 0x00;
  myContent->filename[0] = 0x00;
  return myContent;
}

void *_mikaSignageIterationThread( void *data )
{
  if (!data)
    return NULL;
  
  struct mikaSignage *mySignage = (struct mikaSignage*)data;
  while(1)
  {
    if ( mikaSignageConfigurationGet(mySignage)==0 )
    {
    }
    sleep(60);
  }
  
  return NULL;
}

int mikaSignageIterate( struct mikaSignage *mySignage )
{
  if (!mySignage)
    return 1;
  
  pthread_t threadIteration;
  pthread_create(&threadIteration,NULL,_mikaSignageIterationThread,mySignage);
  
  return 0;
}

struct mikaSignageWindow *mikaSignageWindowInit( struct mikaSignageWindowAttr *attr )
{
  if (!attr)
    return NULL;
  
  struct mikaSignageWindow *myWindow = (struct mikaSignageWindow*)malloc(sizeof(struct mikaSignageWindow));
  
  myWindow->window = osWindowInit(&attr->osAttr);
  
  return myWindow;
}

int mikaSignageWindowEventLoop( struct mikaSignageWindow *myWindow )
{
  if (!myWindow)
    return 1;
  
  osWindowEventLoop(myWindow->window);  
    
  return 0;
}

int mikaSignageWindowHide( struct mikaSignageWindow *myWindow )
{
  if (!myWindow)
    return 1;
  osWindowHide(myWindow->window);
  return 0;
}

int mikaSignageWindowShow( struct mikaSignageWindow *myWindow )
{
  if (!myWindow)
    return 1;
  osWindowShow(myWindow->window);
  return 0;
}

struct mikaSignageMutex *mikaSignageMutexInit( )
{
  struct mikaSignageMutex *myMikaSignageMutex = (struct mikaSignageMutex*)malloc(sizeof(struct mikaSignageMutex));
  
  if ( pthread_mutex_init(&myMikaSignageMutex->mutex,NULL) != 0)
    exit(0);
  
  FILE *fd = NULL;
  unsigned char data[64];
  sprintf((char*)data,"%d\n",myMikaSignageMutex->mutex);
  
  if ((fd = fopen("mikaSignage.mutex","w")))
  {
    fwrite((void*)data,strlen((char*)data),1,fd);
    fclose(fd);
  }
  
  return myMikaSignageMutex;
}

int mikaSignageMutexLock( struct mikaSignageMutex *myMikaSignageMutex )
{
  if (!myMikaSignageMutex)
    return 1;
  return 0;
}

int mikaSignageMutexUnlock( struct mikaSignageMutex *myMikaSignageMutex )
{
  if (!myMikaSignageMutex)
    return 1;
  return 0;
}

/**  mika - initialize a Signage semaphore for IPC
 */
struct mikaSignageSem *mikaSignageSemInit( )
{
  struct mikaSignageSem *myMikaSignageSem = (struct mikaSignageSem*)malloc(sizeof(struct mikaSignageSem));
  
  myMikaSignageSem->id = semget(IPC_PRIVATE,1,IPC_CREAT);
  
  binarySemaphoreInitialize( myMikaSignageSem->id );
  
  FILE *fd = NULL;
  if ((fd = fopen("mikaSignage.sem","w")))
  {
    unsigned char data[64];
    sprintf((char*)data,"%d\n",myMikaSignageSem->id);
    fwrite((void*)data,strlen((char*)data),1,fd);
    fclose(fd);
  }
  
  return myMikaSignageSem;
}

int mikaSignagePlay( struct mikaSignage *mySignage )
{
  if (!mySignage)
    return 1;
  system("mplayer -vo x11 -zoom -x 1024 -y 768 media/*Faint.mpg");
  #ifdef _DEBUG
    printf("mikaSignagePlay stop\n");
  #endif
  return 0;
}

void *_mika_signage_watchdog_thread(void *data)
{
  #ifdef _DEBUG
    printf("_mika_signage_watchdog_thread\n");
  #endif
  while(1)
  {
    binarySemaphoreWait(signageSem->id);
    #ifdef _DEBUG
      printf("_mika_signage_watchdog_thread show\n");
    #endif
    mikaSignageWindowShow(window);
    mikaSignagePlay(signage);
  }
}

#define _TESTPOST_NO
void *_mika_signage_watchdog_testpost(void *data)
{
  #ifdef _TESTPOST
    #ifdef _DEBUG
      printf("_mika_signage_watchdog_testpost\n");
    #endif
    int n;
    for(n = 0; n < 20; n++)
      sleep(1);
    
    binarySemaphorePost(signageSem->id);
  #endif
  return NULL;
}

#ifdef _MIKA_SIGNAGE_APP

  void *_callButtonPress( int x, int y )
  {
    return NULL;
  }

  void *_callButtonRelease( int x, int y )
  {
    #ifdef _DEBUG
      printf("_callButtonRelease\n");
    #endif
    system("killall mplayer");
    osWindowHide(window->window);
    //pthread_t ttest;
    //pthread_create(&ttest,NULL,_mika_signage_watchdog_testpost,NULL);
    return NULL;
  }

  int main( )
  {
    osInit();
    
    parser = mikaParserInit();
    signage = mikaSignageInit();
    mikaSignageSetServer(signage,(unsigned char*)"ftp://ftp.kernel.org");
    mikaSignageSetDirectory(signage,(unsigned char*)"pub");
    //mikaSignageDownload(signage,"README");
    //mikaSignageRelease(signage);
    
    signageMutex = mikaSignageMutexInit();
    signageSem = mikaSignageSemInit();
    binarySemaphoreWait(signageSem->id);
    pthread_t twatchdog;
    pthread_t ttest;
    pthread_create(&twatchdog,NULL,_mika_signage_watchdog_thread,NULL);
    //pthread_create(&ttest,NULL,_mika_signage_watchdog_testpost,NULL);
    
    mikaSignageWindowAttr attr;
    attr.osAttr.x = 0;
    attr.osAttr.y = 0;
    attr.osAttr.w = 1024;
    attr.osAttr.h = 768;
    window = mikaSignageWindowInit( &attr );
    window->window->attr.events.callButtonPress = _callButtonPress;
    window->window->attr.events.callButtonRelease = _callButtonRelease;
    //printf("call p(%p)\n",_callButtonRelease);
    mikaSignageWindowHide(window);
    mikaSignageWindowEventLoop(window);
    mikaSignageRelease(signage);
    
    return 0;
  }
#endif
