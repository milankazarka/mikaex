/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "system.h"

#include <sys/types.h>
#include <sys/shm.h>

#define STATIC
#define SYSSEG		4096
#define STDMEMSIZE	SYSSEG*200;
#define STDACCESSDELAY	10000		// delay between trying to access a memmory object
#define STDACCESSWAIT	20		// number of cycles (time=STDACCESSDELAY*STDACCESSWAIT) we retry

typedef struct trop_Data
{
  short int		id;
  int			start;
  int			size;
  void			*data; 		// relative to access
  time_t		access; 	// -1 = released, value = still used
  int			attr;
  struct trop_Data	*next;
} trop_Data;

typedef struct trop_MemSeg
{
  void			*data;
  int			size;
  struct trop_Data	*objs;
  struct trop_MemSeg	*next;
} trop_MemSeg;

typedef struct trop_Mem
{
  int			max_wait;
  struct trop_MemSeg	*seg;
} trop_Mem;

int _WARNING_ACCESS = 0;
struct trop_Mem *mainMem;

void *trop_MemSegNew( );
int trop_MemAddObj( struct trop_Mem *mem, int size );
int trop_MemAddObjEx( struct trop_Mem *mem, int size, int attr );
struct trop_Data *trop_MemGetObj( struct trop_Mem *mem, int id );
int trop_MemFreeObj( struct trop_Mem *mem, int id );
int trop_MemInit( struct trop_Mem *mem );
void *trop_MemNew( );
void *_access( int id );
void *_release( struct trop_Data *obj );

void *trop_MemSegNew( )
{
  size_t size = STDMEMSIZE;
  struct trop_MemSeg *seg = (struct trop_MemSeg*)malloc(sizeof(struct trop_MemSeg));
  seg->size = STDMEMSIZE;
  seg->data = (void*)malloc(size);
  seg->objs = NULL;
  seg->next = NULL;
  return seg;
}

int trop_MemAddObj( struct trop_Mem *mem, int size )
{
  int id 	= -1;
  int high 	= -1;
  int low 	= -1;
  struct trop_MemSeg *currents 	= mem->seg;
  struct trop_Data *currento 	= NULL;
  struct trop_Data *newo 	= NULL;
  while(currents) // search for free space between objects
  {
    currento = currents->objs;
    while(currento)
    {
      if (currento->id < low || low == -1)
      {
        low = currento->id;
      }
      if (currento->id > high || high == -1)
      {
        high = currento->id;
      }
      if (!currento->next) break;
      if (currento->start + currento->size + size-1 < currento->next->start)
      {
        newo = (struct trop_Data*)malloc(sizeof(struct trop_Data));
        newo->start = currento->start + currento->size;
        newo->next = currento->next;
        currento->next = newo;
        break;
      }
      currento = currento->next;
    }
    if (newo) break;
    currents = currents->next;
  }
  if (!newo) // search for free space after the last object
  {
    currents = mem->seg;
    while(currents)
    {
      currento = currents->objs;
      while(currento)
      {
        if (currento->next == NULL && 
          (currento->start + currento->size + size <= currents->size))
        {
          newo = (struct trop_Data*)malloc(sizeof(struct trop_Data));
          newo->start = currento->start+size;
          newo->next = NULL;
          currento->next = newo;
          break;
        }
        currento = currento->next;
      }
      if (newo) break;
      currents = currents->next;
    }
  }
  if (!newo)
  {
    currents = mem->seg;
    while(currents)
    {
      if (currents->objs == NULL)
      {
        newo = (struct trop_Data*)malloc(sizeof(struct trop_Data));
        newo->start = 0;
        newo->next = NULL;
        currents->objs = newo;
        break;
      }	
      currents = currents->next;
    }
  }
  if (!newo)
  {
    // no free space, error or allocate new segment
    return -1;
  }
  newo->size = size;
  newo->access = -1;
  newo->attr = 0;
  
  if (low == -1)
    newo->id = 0;
  else
  {
    if (low != 0)
    {
      newo->id = low;
      newo->id--;
    }  
    else
    {
      newo->id = high;
      newo->id++;
    }
  }
  
  #ifdef _DEBUG
    if (DEBUG==TRUE) printf(":newo id:%d %d %d (low:%d high:%d)\n",
     newo->id,newo->start,newo->size,low,high);
  #endif
    
  return newo->id;
}

int trop_MemAddObjEx( struct trop_Mem *mem, int size, int attr )
{
  int ret = trop_MemAddObj(mem,size);
  if (ret != -1)
  {
    struct trop_Data *data = trop_MemGetObj(mem,ret);
    if (data) data->attr = attr;
  }
  return ret;
}

void _mem_access_delay( )
{
  usleep(STDACCESSDELAY);
}

struct trop_Data *trop_MemGetObj( struct trop_Mem *mem, int id )
{
  struct trop_Data *data 	= NULL;
  struct trop_MemSeg *currents 	= mem->seg;
  struct trop_Data *currento	= NULL;
  while(currents)
  {
    currento = currents->objs;
    while(currento)
    {
      if (currento->id == id)
      {
        data = currento;
        data->data = currents->data+data->start;
        break;
      }
      currento = currento->next;
    }
    if (data) break;
    currents = currents->next;
  }
  if (data) 
  {
    if (data->access == -1)
      data->access = time(NULL);
    else 
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf(":warning: double access, waiting\n");
      #endif
      //data = NULL; // NO_WAIT
      data = (void*)&_WARNING_ACCESS;
    }
  }
  return data;
}

int trop_MemFreeObj( struct trop_Mem *mem, int id )
{
  struct trop_Data *data	= NULL;
  struct trop_MemSeg *currents	= mem->seg;
  struct trop_Data *currento	= NULL;
  struct trop_Data *last	= NULL;
  while(currents)
  {
    currento = currents->objs;
    while(currento)
    {
      if (currento->id == id)
      {
        if (last)
          last->next = currento->next;
        else
          currents->objs = currento->next;
        free(currento);
        currento = NULL;
      }
      last = currento;
      if (currento) currento = currento->next;
      else currento = currents->objs;
    }
    currents = currents->next;
  }
  return 0;
}

int trop_MemInit( struct trop_Mem *mem )
{
  mem->max_wait = STDACCESSWAIT;
  mem->seg = trop_MemSegNew();
  return 0;
}

void *trop_MemNew( )
{
  struct trop_Mem *mem = (struct trop_Mem*)malloc(sizeof(struct trop_Mem));
  trop_MemInit(mem);
  return mem;
}

void *_access( int id )	
{
  if (!mainMem || id == -1) return NULL;
  int nw = 0;
  void *p = NULL;
  while( (p = trop_MemGetObj(mainMem,id)) == (void*)&_WARNING_ACCESS)
  {
    _mem_access_delay();
    nw++;
    if (nw == mainMem->max_wait) return NULL;
  }
  return p;
}

void *_release( struct trop_Data *obj )
{
  obj->access = -1;
  return NULL;
}

#ifdef STATIC
  int main( )
  {
    struct trop_Mem *mem = trop_MemNew();
    mainMem = mem;
    int obj1 = trop_MemAddObj(mem,24);
    int obj2 = trop_MemAddObj(mem,24);
    trop_MemFreeObj(mem,obj1);
    //struct trop_Data *obj = trop_MemGetObj(mem,obj2);
    //struct trop_Data *obj01 = trop_MemGetObj(mem,obj2);
    struct trop_Data *obj = _access(obj2);
    struct trop_Data *obj01 = _access(obj2);
    if (obj)
    {
      sprintf(obj->data,"hello universe");
      printf("obj: %s\n",obj->data);
    }
    obj = _release(obj);
    obj01 = _access(obj2);
    return 0;  
  }
#endif
