/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "rootControllerSync.h"
#include "activity.h"
#include "activityObject.h"
#include "os.h"
#include "config.h"

int rootDesktopSemaphore = -1;

int _setSemaphore( struct activityObjects *objects )
{
  if (!objects) return 1;
  
  if (rootDesktopSemaphore==-1)
  {
    struct activityObject semObject;
    if (activityObjectClone(objects,(unsigned char*)"rootsemaphore",&semObject)==0)
    {
      #ifdef _DEBUG
        printf("_setSemaphore set\n");
      #endif
      if (semObject.fastData[0]!=0x00)
      {
        rootDesktopSemaphore = atoi((char*)semObject.fastData);
      }
      #ifdef _DEBUG
        printf("_setSemaphore set finish\n");
      #endif
    }
  }
  if (rootDesktopSemaphore==-1)	return 1;
  else				return 0;
}
/**
int rootControllerLock( )
{
  #ifdef _DEBUG
    printf("rootControllerLock\n");
  #endif
  struct activityObjects *objects = activityObjectsClone();
  if (objects)
  {
    if (_setSemaphore(objects)==0)
    {
      #ifdef _DEBUG
        printf("rootControllerLock start\n");
      #endif
      binarySemaphoreWait(rootDesktopSemaphore);
      #ifdef _DEBUG
        printf("rootControllerLock finish\n");
      #endif
    }
    else
    {
      printf("rootControllerLock error, no semaphore present\n");
      return 1;
    }
  }
  else
  {
    printf("rootControllerLock error, no activityObjects\n");
    return 1;
  }
  return 0;
}

int rootControllerUnlock( )
{
  #ifdef _DEBUG
    printf("rootControllerLock_Unlock\n");
  #endif
  struct activityObjects *objects = activityObjectsClone();
  if (objects)
  {
    if (_setSemaphore(objects)==0)
    {
      #ifdef _DEBUG
        printf("rootControllerLock_Unlock start\n");
      #endif
      binarySemaphorePost(rootDesktopSemaphore);
      #ifdef _DEBUG
        printf("rootControllerLock_Unlock finish\n");
      #endif
    }
    else
    {
      printf("rootControllerLock_Unlock error, no semaphore present\n");
      return 1;
    }
  }
  else
  {
    printf("rootControllerLock_Unlock error, no activityObjects\n");
    return 1;
  }
  return 0;
}
*/