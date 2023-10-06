/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

/** a very simple hack to make a wholy different application
 *  communicate with our components
 * 
 */

#include "bridge.h"
#include "os.h"

#define _WIDGET_SURFACE_FILENAME "mikaWidgetSurface.log"

#define _STATIC
#ifndef _DEBUG
  #define _DEBUG
#endif

#ifndef _TEST
  #define _TEST
  #define _TEST_BUFFER_SIZE 320*320*4
#endif

unsigned char _test_buffer[_TEST_BUFFER_SIZE];

struct mikaBridge *mikaBridgeInit( unsigned char *name )
{
  struct mikaBridge *bridge = (struct mikaBridge*)malloc(sizeof(struct mikaBridge));
  if (!bridge)
    return NULL;
  bridge->w = 0;
  bridge->h = 0;
  strcpy((char*)bridge->name,(char*)name);
  bridge->semaphore = semget(IPC_PRIVATE,1,IPC_CREAT);
  binarySemaphoreInitialize(bridge->semaphore);
  
  FILE *fd;
  unsigned char line[64];
  unsigned char filename[128];
  sprintf((char*)line,"%d\n",bridge->semaphore);
  sprintf((char*)filename,"bridge%s.log",name);
  #ifdef _DEBUG
    printf("mikaBridgeInit filename(%s)\n",(char*)filename);
  #endif
  if ((fd = fopen((const char*)filename,"w")))
  {
    #ifdef _DEBUG
      printf("\twrite line(%s)\n",(char*)line);
    #endif
    fwrite((void*)line,strlen((char*)line),1,fd);
    fclose(fd);
  }
  
  binarySemaphoreWait(bridge->semaphore);
  
  return bridge;
}

int mikaBridgeRelease( struct mikaBridge *bridge )
{
  if (!bridge)
    return 1;
  free(bridge);
  return 0;
}

int mikaBridgeExposeBuffer( struct mikaBridge *bridge, void *buffer )
{
  if (!bridge || !buffer)
    return 1;
  
  #ifdef _DEBUG
    printf("mikaBridgeExposeBuffer\n");
    
    #ifdef _TEST
      int n;
      for(n = 0; n < _TEST_BUFFER_SIZE; n++)
        _test_buffer[n] = 0x00;
    #endif
  #endif
  binarySemaphorePost(bridge->semaphore);
  
  usleep(10000);  
  return 0;
}

struct mikaWidget *mikaWidgetAlloc( )
{
  struct mikaWidget *widget = (struct mikaWidget*)malloc(sizeof(struct mikaWidget));
  widget->semaphore = 0;
  widget->x = 0;
  widget->y = 0;
  widget->w = 0;
  widget->h = 0;
  return widget;
}

void *_mika_widget_thread( void *data )
{
  long run = 0;
  struct mikaWidget *widget = (struct mikaWidget*)data;
  while(1)
  {
    binarySemaphoreWait(widget->semaphore);
    
    // expose widget in interface
    #ifdef _DEBUG
      printf("_mika_widget_thread run(%d) semaphore(%d)\n",run,widget->semaphore);
    
      #ifdef _TEST
        int n;
        unsigned char c;
        for(n = 0; n < _TEST_BUFFER_SIZE; n++)
          c = _test_buffer[n];
      #endif
    #endif
    
    binarySemaphorePost(widget->semaphore);
    binarySemaphoreWait(widget->semaphore);
    run++;
  }
  return NULL;
}

#ifdef _STATIC
  int main( )
  {
    struct mikaBridge *bridge = mikaBridgeInit((unsigned char*)"Simple");
    if (!bridge)
      return 1;
    
    bridge->w = 300;
    bridge->h = 300;
    
    struct mikaWidget *widget = mikaWidgetAlloc();
    if (!widget)
      return 1;
    
    #ifdef _DEBUG
      printf("bridge semaphore(%d)\n",bridge->semaphore);
    #endif
    widget->semaphore = bridge->semaphore;
    pthread_t widget_thread;
    pthread_create(&widget_thread,NULL,_mika_widget_thread,widget);
    
    while(1)
    {
      //mikaBridgeExposeBuffer(bridge,widget);
      usleep(1000000);
    }
    
    return 0;
  }
#endif
