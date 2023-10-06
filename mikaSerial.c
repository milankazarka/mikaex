#include "mikaSerial.h"

/** initialize mikaSerial
 */
struct mikaSerial *mikaSerialInit( struct mikaSerial *serial )
{
  #ifdef _DEBUG
    printf("mikaSerialInit\n");
  #endif
  if (!serial)
    serial = (struct mikaSerial*)malloc(sizeof(struct mikaSerial));
  
  serial->shmid = shmget( IPC_PRIVATE, sizeof(struct mikaSerialQueue), IPC_CREAT |
    IPC_EXCL | S_IRUSR | S_IWUSR | SHM_R | SHM_W );
  
  #ifdef _SERIAL_MUTEX
    pthread_mutex_init(&serial->mutex,NULL);
  #else
    serial->semid = semget(IPC_PRIVATE,1,IPC_CREAT);
    binarySemaphoreInitialize(serial->semid);
  #endif
  
  if (serial->shmid<0)
  {
    #ifdef _DEBUG
      printf("mikaSerialInit error, shared memmory could not be allocated\n");
    #endif
    return NULL;
  }
  
  #ifdef _SERIAL_MUTEX
    pthread_mutex_lock(&serial->mutex);
  #else
    binarySemaphoreWait(serial->semid);
  #endif
  void *shm = shmat(serial->shmid,0,0);
  struct mikaSerialQueue *queue = (struct mikaSerialQueue*)shm;
  int n;
  for(n = 0; n < _SERIAL_QUEUE; n++)
  {
    queue->events[n].active = 0;
    queue->events[n].type = -1;
    memset(&queue->events[n].next,0x00,sizeof(unsigned long));
  }
  
  memset(&queue->pevents,0x00,sizeof(unsigned long));
    
  shmdt(shm);
  #ifdef _SERIAL_MUTEX
    pthread_mutex_unlock(&serial->mutex);
  #else
    binarySemaphorePost(serial->semid);
  #endif
  
  unsigned char line[64];
  FILE *bridge = NULL;
  if (bridge = fopen("serial.log","w"))
  {
    #ifdef _DEBUG
      printf("mikaSerialInit semid %d shmid %d\n",
        serial->semid,serial->shmid);
    #endif
    sprintf((char*)line,"%d\n%d\n",
      #ifdef _SERIAL_MUTEX
        serial->mutex,
      #else
        serial->semid,
      #endif
      serial->shmid);
    fwrite((char*)line,strlen((char*)line),1,bridge);
    fflush(bridge);
    fclose(bridge);
  }
  #ifdef _SYNC
    sync();
  #endif
  
  return serial;
}

/** clone the semaphore id and shm id from the serial.log
 *  and retrieve a clone of the mikaSerial object
 */
struct mikaSerial *mikaSerialClone( )
{
  struct mikaSerial *serial = NULL;
  int semid = -1;
  int shmid = -1;
  int ln = 0;
  
  unsigned char line[32];
  FILE *bridge = NULL;
  
  #ifdef _DEBUG
    printf("mikaSerialClone\n");
  #endif
  
  if (bridge = fopen("serial.log","r"))
  {
    while(1)
    {
      fgets((char*)line,31,bridge);
      
      if (line[strlen((char*)line)-1]=='\n')
        line[strlen((char*)line)-1] = 0x00;
      
      if (feof(bridge))
      {
        fclose(bridge);
        break;
      }
      switch(ln)
      {
        case 0:
          semid = atoi((char*)line);
        break;
        case 1:
          shmid = atoi((char*)line);
        break;
      }
      ln++;
    }
  }
  
  if (semid!=-1 || shmid!=-1)
  {
    serial = (struct mikaSerial*)malloc(sizeof(struct mikaSerial));
    serial->semid = semid;
    serial->shmid = shmid;
  
    #ifdef _DEBUG
      printf("mikaSerialClone %d %d\n",serial->semid,serial->shmid);
    #endif
  }
  
  return serial;
}

void drqieSerialList( struct mikaSerial *serial )
{
  if (!serial)
    return;
}

/** add an event to the queue | 2 - fail, 0 - succeed
 */
int mikaSerialAddEvent( struct mikaSerial *serial, int type, unsigned char *eventData )
{
  static void 	*shm = NULL;
  static struct mikaSerialQueue *queue = NULL;
  static int 	n;
  static struct mikaSerialEvent *current = NULL;
  
  int ret = 0;
  
  if (!serial)
  {
    #ifdef _DEBUG
      printf("mikaSerialAddEvent error invalid arguments\n");
    #endif
    return 1;
  }
  #ifdef _DEBUG
    printf("mikaSerialAddEvent type %d\n",type);
  #endif
  #ifdef _SERIAL_MUTEX
    pthread_mutex_lock(&serial->mutex);
  #else
    binarySemaphoreWait(serial->semid);
  #endif
  shm = shmat(serial->shmid,0,0);
  if (shm)
  {
    queue = (struct mikaSerialQueue*)shm;
    for(n = 0; n < _SERIAL_QUEUE; n++)
    {
      #ifdef _DEBUG
        printf("mikaSerialAddEvent check %p\n",&queue->events[n]);
      #endif
      if (queue->events[n].active==0)
      {
        current = &queue->events[n];
        current->type = type;
        current->active = 1;
        
        memcpy((unsigned char*)current->data,eventData,strlen((char*)eventData));
        #ifdef _DEBUG
          printf("mikaSerialAddEvent %p\n",current);
        #endif
        break;
      }
    }
    if (!current)
    {
      ret = 2;
    }
    
    #ifdef _DEBUG
      printf("mikaSerialAddEvent added %p\n",
        current);
    #endif
    
    shmdt(shm);
  }
  #ifdef _SERIAL_MUTEX
    pthread_mutex_unlock(&serial->mutex);
  #else
    binarySemaphorePost(serial->semid);
  #endif
  
  return ret;
}

/** our own event handling | for retrieving events please use
 *  mikaSerialNextEvent
 */
void *mikaSerialEventLoop(void*data)
{
  if (!data)
    return NULL;
  struct mikaSerial *serial = (struct mikaSerial*)data;
  struct mikaSerialQueue *queue = NULL;
  struct mikaSerialEvent *event = NULL;
  int n;
  while(1)
  {
    #ifdef _DEBUG
      printf("mikaSerialEventLoop cycle\n");
    #endif
    #ifdef _SERIAL_MUTEX
      pthread_mutex_lock(&serial->mutex);
    #else
      binarySemaphoreWait(serial->semid);
    #endif
    queue = (struct mikaSerialQueue*)shmat(serial->shmid,0,0);
    if (queue)
    {
      event = queue->pevents;
      #ifdef _DEBUG
        while(event)
        {
          printf("mikaSerialEventLoop event %p\n",event);
          event = event->next;
        }
      #endif
      event = queue->pevents;
      if (event)
      {
        queue->pevents = event->next;
        event->active = 0;
      }
      shmdt(queue);
    }
    #ifdef _SERIAL_MUTEX
      pthread_mutex_unlock(&serial->mutex);
    #else
      binarySemaphorePost(serial->semid);
    #endif
    usleep(_SERIAL_TIK);
  }
}

int mikaSerialEventLoopRun( struct mikaSerial *serial )
{
  if (!serial)
    return 1;
  pthread_t t;
  pthread_create(&t,NULL,mikaSerialEventLoop,serial);
  return 0;
}

/** retrieve the next event | 0 - success, 1 - fail
 */
int mikaSerialNextEvent( struct mikaSerial *serial, struct mikaSerialEvent *nextEvent, int type )
{
  struct mikaSerialQueue 	*queue = NULL;
  static int n;
  
  if (!serial || !nextEvent)
    return 2;
  
  #ifdef _SERIAL_MUTEX
    pthread_mutex_lock(&serial->mutex);
  #else
    binarySemaphoreWait(serial->semid);
  #endif
  
  int ret = 1;

  queue = (struct mikaSerialQueue*)shmat(serial->shmid,0,0);
  if (queue)
  {
    struct mikaSerialEvent *pevent = NULL;
     
    #ifdef _DEBUG
    //  printf("mikaSerialNext event %p\n",
    //    pevent);
    #endif
      
    for(n = 0; n < _SERIAL_QUEUE; n++)
    {
      pevent = &queue->events[n];
      #ifdef _DEBUG
      //  printf("mikaSerialNext check %p\n",
      //    pevent);
      #endif
        
      if (
        (type==-1 || type==pevent->type) &&
        pevent->active == 1
      )
      {
        #ifdef _DEBUG
        //  printf("mikaSerialNext event ptr(%p) next(%p) type(%d)\n",
        //    pevent,pevent->next,pevent->type);
        #endif
        pevent->active = 0;
        ret = 0;
        break;
      }
    }
      
    shmdt(queue);
  }
  
  #ifdef _SERIAL_MUTEX
    pthread_mutex_unlock(&serial->mutex);
  #else
    binarySemaphorePost(serial->semid);
  #endif
  
  return ret;
}
