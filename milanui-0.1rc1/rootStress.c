/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

/** rootStress is a small helper to perform stress tests on the rootController
 *
 */

#include "rootStress.h"
#include "rootController.h"

struct rootStress *rootStressNew( )
{
  struct rootStress *myStress;
  myStress = (struct rootStress*)malloc(sizeof(struct rootStress));
  myStress->loopTime = 0;
  myStress->thread = -1;
  return myStress;
}

int rootStressRelease( struct rootStress *myStress )
{
  if (!myStress)
    return 1;
  free(myStress);
  return 0;
}

void *stressThread(void*data)
{
  if (!data)
    return NULL;
  
  int x = 50;
  int y = 50;
  
  struct rootStress *myStress = (struct rootStress*)data;
  while(1)
  {
    if (mainRd)
    {
      binarySemaphoreWait(mainRd->sem->id);
      #ifdef _DEBUG
        printf("stressThread insert event BDOWN\n");
      #endif
      mainRd->ePlatform->state = _STATE_ACTIVE;
      mainRd->ePlatform->event.xbutton.x = x;
      mainRd->ePlatform->event.xbutton.y = y;
      mainRd->ePlatform->event.type = ButtonPress;
      binarySemaphorePost(mainRd->sem->id);
      usleep(100000);
      
      while(1)
      {
        binarySemaphoreWait(mainRd->sem->id);
        if (mainRd->ePlatform->state != _STATE_ACTIVE)
        {
          binarySemaphorePost(mainRd->sem->id);
          break;
        }
        binarySemaphorePost(mainRd->sem->id);
        usleep(1000);
      }
      
      binarySemaphoreWait(mainRd->sem->id);
      #ifdef _DEBUG
        printf("stressThread insert event BUP\n");
      #endif
      mainRd->ePlatform->state = _STATE_ACTIVE;
      mainRd->ePlatform->event.xbutton.x = x;
      mainRd->ePlatform->event.xbutton.y = y;
      mainRd->ePlatform->event.type = ButtonRelease;
      binarySemaphorePost(mainRd->sem->id);
      
      while(1)
      {
        binarySemaphoreWait(mainRd->sem->id);
        if (mainRd->ePlatform->state != _STATE_ACTIVE)
        {
          binarySemaphorePost(mainRd->sem->id);
          break;
        }
        binarySemaphorePost(mainRd->sem->id);
        usleep(1000);
      }
      
      binarySemaphoreWait(mainRd->sem->id);  
      #ifdef _DEBUG
        printf("stressThread insert event MOTION\n");
      #endif
      mainRd->ePlatform->state = _STATE_ACTIVE;
      mainRd->ePlatform->event.xmotion.x = x+7;
      mainRd->ePlatform->event.xmotion.y = y+7;
      mainRd->ePlatform->event.type = MotionNotify;
      binarySemaphorePost(mainRd->sem->id);
      
      while(1)
      {
        binarySemaphoreWait(mainRd->sem->id);
        if (mainRd->ePlatform->state != _STATE_ACTIVE)
        {
          binarySemaphorePost(mainRd->sem->id);
          break;
        }
        binarySemaphorePost(mainRd->sem->id);
        usleep(1000);
      }
      
      binarySemaphoreWait(mainRd->sem->id);
      #ifdef _DEBUG
        printf("stressThread insert event MOTION BACK\n");
      #endif
      mainRd->ePlatform->state = _STATE_ACTIVE;
      mainRd->ePlatform->event.xmotion.x = x-7;
      mainRd->ePlatform->event.xmotion.y = y-7;
      mainRd->ePlatform->event.type = MotionNotify;
      binarySemaphorePost(mainRd->sem->id);
    }
    if (x<640&&y<480)
    {
      x+=1;
      y+=1;
    }
    else
    {
      x = 50;
      y = 50;
    }
    usleep(myStress->loopTime);
  }
}

int rootStressStart( struct rootStress *myStress )
{
  if (!myStress)
    return 1;
  
  pthread_create(&myStress->thread,NULL,stressThread,myStress);  
  
  return 0;
}
