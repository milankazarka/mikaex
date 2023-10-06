/** mikaCe - Mika Compositor Engine which manages
 *  graphical objects accessible through the network
 *
 */
#include "mikaCe.h"
#include "config.h"
#define _STATIC_NO
#define _MIKA_CE_CLOCKTIK 250000

mikaCe *dce = NULL;

mikaCe::mikaCe( )
{
  objects = NULL;
  parcels = NULL;
  pthread_mutex_init(&mutex,NULL);
  dce = this;
}

mikaCe::~mikaCe( )
{
}

void mikaCe::lock( )
{
  pthread_mutex_lock(&mutex);
}

void mikaCe::unlock( )
{
  pthread_mutex_unlock(&mutex);
}

void mikaCe::threadClocktik( )
{
}

void *engineThread(void*data)
{
  mikaCe *ce = (mikaCe*)data;
  while(1)
  {
    ce->lock();
    ce->threadClocktik( );
    ce->unlock();
    usleep(_MIKA_CE_CLOCKTIK);
  }
}

void mikaCe::runEngine( )
{
  pthread_t t;
  pthread_create(&t,NULL,engineThread,this);
  listener = new mikaCeNetworkListener(_MIKA_CE_LISTENER_PORT);
  listener->Listen();
}

int mikaCe::addObject( mikaCeObject *object )
{
  if (!object)
  {
    #ifdef _DEBUG
      printf("mikaCe::addObject error invalid arguments\n");
    #endif
    return 1;
  }
  object->next = objects;
  objects = object;
  return 0;
}

int mikaCe::mvObject( mikaCeObject *object, struct mikaFrame *frame )
{
  if (!object || !frame)
  {
    #ifdef _DEBUG
      printf("mikaCe::mvObject error invalid arguments\n");
    #endif
    return 1;	
  }
  
  object->frame = *frame;
  
  return 0;
}

/** fetch an object of a speciffic ID
 */
mikaCeObject *mikaCe::objectAtId( char *id )
{
  if (!id)
  {
    #ifdef _DEBUG
      printf("mikaCe::objectAtId error invalid arguments\n");
    #endif
    return NULL;
  }

  mikaCeObject *object = NULL;
  mikaCeObject *current = objects;
  while(current)
  {
    if (
      strncpy((char*)current->id,id,8)==0
    )
    {
      object = current;
      break;
    }
    current = current->next;
  }
  return object;
}

/** push an object to the top of the stack - the object must exist
 *  in the objects list
 */
int mikaCe::pushObject( mikaCeObject *object )
{
  mikaCeObject *current = objects;
  mikaCeObject *last = NULL;
  while(current)
  {
    if (current==object)
    {
      if (last)
        last->next = current->next;
      else
        objects = current->next;
      break;
    }
    last = current;
    current = current->next;
  }
  object->next = objects;
  objects = object;
  return 0;
}

/** remove an object
 */
int mikaCe::rmObject( mikaCeObject *object )
{
  return 0;
}

int mikaCe::addParcel( mikaCeParcel *parcel )
{
  if (!parcel)
  {
    #ifdef _DEBUG
      printf("mikaCe::addParcel error invalid arguments\n");
    #endif
    return 1;
  }
  parcel->next = parcels;
  parcels = parcel;
  return 0;
}

void mikaCe::listObjects( )
{
  mikaCeObject *current = objects;
  while(current)
  {
    #ifdef _DEBUG
      printf("mikaCe::listObjects object\n");
    #endif
    current = current->next;
  }
}

/** get the position of an object in a parcel
 *
 */
struct mikaFrame mikaCe::frameOfObjectInParcel( mikaCeParcel *parcel, mikaCeObject *object )
{
  struct mikaFrame frame;
  frame.x = 0;
  frame.y = 0;
  frame.w = 0;
  frame.h = 0;
  frame.active = 1;
  if (!parcel || !object)
  {
    #ifdef _DEBUG
      printf("mikaCe::frameOfObjectInParcel error invalid arguments\n");
    #endif
    frame.active = 0;
    return frame;
  }
  
  if (
    parcel->frame.x+parcel->frame.w <= object->frame.x ||
    parcel->frame.x > object->frame.x+object->frame.w ||
    parcel->frame.y > object->frame.y+object->frame.h ||
    parcel->frame.y+parcel->frame.h <= object->frame.y
  )
  {
    frame.active = 0;
    return frame;
  }
  else
  {
    frame.x = object->frame.x - parcel->frame.x;
    frame.y = object->frame.y - parcel->frame.y;
    frame.w = object->frame.w;
    frame.h = object->frame.h;
  }
  
  return frame;
}

/** list all objects belonging to a parcel
 *
 */
int mikaCe::listObjectsForParcel( struct mikaCeParcel *parcel )
{
  if (!parcel)
  {
    return 1;
  }
  mikaCeObject *current = objects;
  mikaFrame frame;
  while(current)
  {
    frame = frameOfObjectInParcel(parcel,current);
    if (frame.active==1)
    {
      #ifdef _DEBUG
        printf("mikaCe::listObjectsForParcel objects(%dx%d+%d+%d)\n",
          frame.w,frame.h,frame.x,frame.y);
      #endif
    }
    current = current->next;
  }
  return 0;
}

#ifdef _STATIC
  int main( )
  {
    mikaCe *ce = new mikaCe();
    if (ce)
    {
      ce->runEngine();
      while(1)
      { sleep(1); }
    }
    return 0;
  }
#endif
