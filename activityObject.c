/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "activityObject.h"
#include "config.h"

struct activityObjects *actObjects = NULL;

/** please try not to lock/unlock outside of these functions
 */
int activityObjectsLock( struct activityObjects *objects )
{
  if (!objects) return 1;
  if (!objects->sem) return 1;
  #ifdef _DEBUG
    printf("activityObjectsLock\n");
  #endif
  binarySemaphoreWait(objects->sem->id);
  #ifdef _DEBUG
    printf("activityObjectsLock finish\n");
  #endif
  return 0;
}

int activityObjectsUnlock( struct activityObjects *objects )
{
  if (!objects) return 1;
  if (!objects->sem) return 1;
  #ifdef _DEBUG
    printf("activityObjectsUnlock\n");
  #endif
  binarySemaphorePost(objects->sem->id);
  #ifdef _DEBUG
    printf("activityObjectsUnlock finish\n");
  #endif
  return 0;
}

/** should be called by a controller, not by applications themselves
 */
struct activityObjects *activityObjectsInit( )
{
  #ifdef _DEBUG
    printf("activityObjectsInit\n");
  #endif
  
  if (actObjects) return actObjects;
  
  struct activityObjects *objects = 
    (struct activityObjects*)malloc(sizeof(struct activityObjects));
  
  actObjects = objects;
  
  objects->shm = (struct rootShm*)malloc(sizeof(struct rootShm));
  objects->sem = (struct rootSem*)malloc(sizeof(struct rootSem));
  
  // todo, should create a unified interface for activities & others
  os_rshmAlloc(objects->shm,_ACTIVITY_OBJECTS_STORAGE,_ACTIVITY_TYPE_CUSTOM,(unsigned char*)"OBJECTS");
  os_rsemInit(objects->sem);
  
  os_rsemLog(objects->sem,"activity/CUSTOM_OBJECTS");
  
  void *shmdata = shmat(objects->shm->id,0,0);
  if (shmdata)
  {
    memset(shmdata,0x00,_ACTIVITY_OBJECTS_STORAGE);
    shmdt(shmdata);
  }
  else
    printf("drink red bull and find a bug kid\n");
  
  #ifdef _DEBUG
    printf("activityObjectsInit shmid(%d) semid(%d)\n",
      objects->shm->id,objects->sem->id);
  #endif
  
  return objects;
}

/** get the data to create a clone of the proto activityObjects
 */
struct activityObjects *activityObjectsClone( )
{
  if (actObjects) return actObjects;
  
  struct activityObjects *objects = 
    (struct activityObjects*)malloc(sizeof(struct activityObjects));
  
  objects->shm = (struct rootShm*)malloc(sizeof(struct rootShm));
  objects->sem = (struct rootSem*)malloc(sizeof(struct rootSem));
  
  FILE *fd = NULL;
  unsigned char line[256];
  unsigned char tmp[256];
  
  if ((fd = fopen("activity/CUSTOM_OBJECTS","r"))==NULL)
  {
    #ifdef _DEBUG
      printf("activityInit could not open file(activity/CUSTOM_OBJECTS)\n");
    #endif
  }
  else
  {
    while(1)
    {
      fgets((char*)line,256,fd);
      if (feof(fd))
        break;
      if (line[strlen((char*)line)-1])
      {
        line[strlen((char*)line)-1] = 0x00;
      }
      
      if (strlen((char*)line)>6)
      {
        if (strncmp((char*)line,"SHMID",5)==0)
        {
          #ifdef _DEBUG
            printf("\tSHMID\n");
          #endif
          strncpy((char*)tmp,(char*)line+6,strlen((char*)line)-6);
          tmp[strlen((char*)line)-6] = 0x00;
          objects->shm->id = atoi((char*)tmp);
        }
        if (strncmp((char*)line,"SHMSZ",5)==0)
        {
          #ifdef _DEBUG
            printf("\tSHMSZ\n");
          #endif
          strncpy((char*)tmp,(char*)line+6,strlen((char*)line)-6);
          tmp[strlen((char*)line)-6] = 0x00;
          objects->shm->size = atoi((char*)tmp);
        }
        if (strncmp((char*)line,"SEMID",5)==0)
        {
          #ifdef _DEBUG
            printf("\tSEMID\n");
          #endif
          strncpy((char*)tmp,(char*)line+6,strlen((char*)line)-6);
          tmp[strlen((char*)line)-6] = 0x00;
          objects->sem->id = atoi((char*)tmp);
        }
      }
    }
    fclose(fd);
  }
  
  return objects;
}

int activityObjectInit( struct activityObject *object )
{
  if (!object)
    return 1;
  
  #ifdef _DEBUG
    printf("activityObjectInit\n");
  #endif
  
  object->active = 0x02;
  object->size = sizeof(struct activityObject);
  object->name[0] = 0x00;
  object->fastData[0] = 0x00;
  
  return 0;
}

int activityObjectRegister( struct activityObjects *objects, unsigned char *name )
{
  #ifdef _DEBUG
    printf("activityObjectRegister shmid(%d) semid(%d)\n",
      objects->shm->id,objects->sem->id);
  #endif
  
  if (!objects || !name)
    return 1;
  
  binarySemaphoreWait(objects->sem->id);
  void *shmdata = shmat(objects->shm->id,0,0);
  int pos = 0;
  if (shmdata)
  {
    unsigned char tmp[1];
    tmp[0] = 0x00;
    while(1)
    {
      if (memcmp((void*)shmdata+pos,(void*)tmp,1)==0)
      {
        #ifdef _DEBUG
          printf("activityObjectRegister found free position\n");
        #endif
        break;
      }
      else
      {
        pos += sizeof(struct activityObject);
      }
    }
    #ifdef _DEBUG
      printf("activityObjectRegister at(%d)\n",pos);
    #endif
    struct activityObject newObject;
    activityObjectInit(&newObject);
    strcpy((char*)newObject.name,(char*)name);
    memcpy(shmdata+pos,&newObject,sizeof(struct activityObject));
    shmdt(shmdata);
  }
  binarySemaphorePost(objects->sem->id);
  
  return 0;
}

int activityObjectClone( struct activityObjects *objects, unsigned char *name, struct activityObject *clone )
{
  binarySemaphoreWait(objects->sem->id);
  void *shmdata = shmat(objects->shm->id,0,0);
  int pos = 0;
  int ret = 1;
  if (shmdata)
  {
    unsigned char tmp[1];
    tmp[0] = 0x00;
    struct activityObject *obj = NULL;
    while(1)
    {
      if (pos+sizeof(struct activityObject)>=_ACTIVITY_OBJECTS_STORAGE)
        break;
      if (memcmp((void*)shmdata+pos,(void*)tmp,1)==0)
        break;
      else
      {
        obj = shmdata+pos;
        if (obj->name[0]!=0x00)
        {
          if (strncmp((char*)obj->name,(char*)name,strlen((char*)name))==0)
          {
            memcpy(clone,obj,sizeof(struct activityObject));
            ret = 0;
            break;
          }
        }
        pos += sizeof(struct activityObject);
      }
    }
    shmdt(shmdata);
  }
  binarySemaphorePost(objects->sem->id);
  return ret;
}

int activityObjectSet( struct activityObjects *objects, unsigned char *name, struct activityObject *from )
{
  binarySemaphoreWait(objects->sem->id);
  void *shmdata = shmat(objects->shm->id,0,0);
  int pos = 0;
  int ret = 1;
  if (shmdata)
  {
    unsigned char tmp[1];
    tmp[0] = 0x00;
    struct activityObject *obj = NULL;
    while(1)
    {
      if (pos+sizeof(struct activityObject)>=_ACTIVITY_OBJECTS_STORAGE)
        break;
      if (memcmp((void*)shmdata+pos,(void*)tmp,1)==0)
        break;
      else
      {
        obj = shmdata+pos;
        if (obj->name[0]!=0x00)
        {
          if (strncmp((char*)obj->name,(char*)name,strlen((char*)name))==0)
          {
            memcpy(obj,from,sizeof(struct activityObject));
            ret = 0;
            break;
          }
        }
        pos += sizeof(struct activityObject);
      }
    }
    shmdt(shmdata);
  }
  binarySemaphorePost(objects->sem->id);
  return ret;
}

int activityObjectGet( struct activityObjects *objects, unsigned char *name )
{
  if (!objects || !name)
    return 1;
  return 0;
}
