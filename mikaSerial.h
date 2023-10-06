#ifndef _MIKA_SERIAL_H_
#define _MIKA_SERIAL_H_

#include "config.h"
#include "os.h"
#include <pthread.h>
#define _SERIAL_QUEUE	10
#define _SERIAL_TIK	50000
#define _SERIAL_MUTEX_NO

typedef struct mikaSerialEvent
{
  int 	x;	// simple values
  int 	y;
  int	state;
  
  unsigned char		data[512];
  int			parentid;
  int			type;
  int 			active;
  unsigned long		next;
} mikaSerialEvent;

typedef struct mikaSerialQueue
{
  struct mikaSerialEvent 	events[_SERIAL_QUEUE]; // real data
  unsigned long			pevents; // data list
} mikaSerialQueue;

typedef struct mikaSerial
{
  #ifdef _SERIAL_MUTEX
    pthread_mutex_t		mutex;
  #else
    int				semid;
  #endif
  int				shmid;
} mikaSerial;

struct mikaSerial *mikaSerialInit( struct mikaSerial *serial );
struct mikaSerial *mikaSerialClone( );
int mikaSerialAddEvent( struct mikaSerial *serial, int type, unsigned char *eventData );
int mikaSerialNextEvent( struct mikaSerial *serial, struct mikaSerialEvent *nextEvent, int type );
void *mikaSerialEventLoop(void*data);
int mikaSerialEventLoopRun( struct mikaSerial *serial );

#endif
