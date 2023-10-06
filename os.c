/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "os.h"
#include "activity.h"
#include "config.h"

char *_trop_conn_in;
char *_trop_conn_out;

unsigned char *activityFilename = NULL;

/**
 * simple binary semaphore wrapper
 */
int binarySemaphoreWait( int semid )
{
  #ifdef _DEBUG
  //  printf("binarySemaphoreWait id(%d)\n",
  //    semid);
  #endif
  struct sembuf operations[1];
  operations[0].sem_num = 0;
  operations[0].sem_op = -1;
  operations[0].sem_flg = SEM_UNDO;
  
  int ret = semop(semid, operations, 1);
  return ret;
}

int binarySemaphorePost( int semid )
{
  struct sembuf operations[1];
  operations[0].sem_num = 0;
  operations[0].sem_op = 1;
  operations[0].sem_flg = SEM_UNDO;
  int ret;
  
  ret = semop(semid, operations, 1);
  return ret;
}

union _semun
{
  int val;
  struct semid_ds *buf;
  unsigned short *array;
  struct seminfo *__buf;
};

int binarySemaphoreInitialize( int semid )
{
  union _semun argument;
  unsigned short values[1];
  values[0] = 1;
  argument.array = values;
  return semctl(semid, 0, SETALL, argument);
}

struct rootShm *os_rshmInit(
  struct rootShm *shm
)
{
  if (!shm)
  {
    shm = (struct rootShm*)malloc(sizeof(struct rootShm));
  }
  shm->data = NULL;
  shm->size = 0;
  shm->id = -1;
    
  return shm;
}

struct rootSem *os_rsemInit( 
  struct rootSem *sem
)
{
  if (!sem)
  {
    sem = (struct rootSem*)malloc(sizeof(struct rootSem));
  } 
    
  sem->id = semget(IPC_PRIVATE,1,IPC_CREAT);
    
  binarySemaphoreInitialize(sem->id);  
  binarySemaphoreWait(sem->id);
  binarySemaphorePost(sem->id);
    
  return sem;
}

/**  allocate the shm
 */
int os_rshmAlloc(
  struct rootShm 	*shm,
  int	 		size,
  int			activityType,
  unsigned char 	*activityName
)
{
  unsigned char buf[_FILE_LINESIZE];
  FILE *fd = NULL;
  
  if (!activityName)
  {
    printf("os_rshmAlloc !activityName\n");
    return -1;
  }
  
  #ifdef _DEBUG
    printf("os_rshmAlloc activityName(%s)\n",activityName);
  #endif
  
  if (!shm)
    shm = (struct rootShm*)malloc(sizeof(struct rootShm));
    
  shm->id = shmget( IPC_PRIVATE, size, IPC_CREAT | IPC_EXCL | 
    S_IRUSR | S_IWUSR | SHM_R | SHM_W );
  if (shm->id<0)
  {
    return -1;
  }
  
  void *test = shmat(shm->id,0,0);
  memcpy(test,"test",4);
  shmdt(test);
  
  unsigned char strActivityType[32];
  sprintf((char*)strActivityType,"DEFAULT");
  switch(activityType)
  {
    case _ACTIVITY_TYPE_CTRL:
      sprintf((char*)strActivityType,"CTRL");
    break;
    case _ACTIVITY_TYPE_APP:
      sprintf((char*)strActivityType,"APP");
    break;
    case _ACTIVITY_TYPE_CUSTOM:
      sprintf((char*)strActivityType,"CUSTOM");
    break;
  }
  sprintf((char*)activityFilename,"activity/%s_%s",
    (char*)strActivityType,activityName);
  
  shm->size = size;
  if ((fd = fopen((char*)activityFilename,"w"))==NULL)
  {
    #ifdef _DEBUG
      printf("rshmAlloc error could not open (%s)\n",
        (char*)activityFilename);
    #endif
    return -1;
  }
  
  sprintf((char*)buf,"SHMID %d\n",shm->id);
  fwrite((void*)buf,strlen((char*)buf),1,fd);
  fflush(fd);
  fclose(fd);
  
  return shm->id;
}

/** log the semaphore id to a filename
 */
int os_rsemLog(
  struct rootSem *sem,
  unsigned char *filename
)
{
  #ifdef _DEBUG
    printf("os_rsemLog sem:%d\n",
      sem->id);
  #endif
  
  if (!sem || !filename)
    return -1;
  
  FILE *fd;
  unsigned char buf[_FILE_LINESIZE];
  if ((fd = fopen((char*)filename,"a"))==NULL)
  {
    #ifdef _DEBUG
      printf("os_rsemLog error could not open (%s)\n",
        filename);
    #endif
  }
  
  sprintf((char*)buf,"SEMID %d\n",sem->id);
  fwrite((void*)buf,strlen((char*)buf),1,fd);
  fflush(fd);
  fclose(fd);
  
  return 0;
}

/** log the shm info to a filename
 */
int os_rshmszLog(
  struct rootShm *shm,
  unsigned char *filename
)
{

  if (!shm || !filename)
    return -1;
  
  FILE *fd;
  unsigned char buf[_FILE_LINESIZE];
  if ((fd = fopen((char*)filename,"a"))==NULL)
  {
    #ifdef _DEBUG
      printf("os_rshmszLog error could not open (%s)\n",
        filename);
    #endif
  }
  
  sprintf((char*)buf,"SHMSZ %d\n",shm->size);
  fwrite((void*)buf,strlen((char*)buf),1,fd);
  fclose(fd);
  return 0;
}

#define _USE_OVERRIDE_REDIRECT // avoid being registered by the Window Manager
int lowWindowCreate( struct osWindow *myOsWindow )
{
  if (!myOsWindow)
    return 1;
  
  #ifdef _CF_GUI_XLIB
    #ifdef _DEBUG
      printf("lowWindoCreate %dx%d+%d+%d\n",
        myOsWindow->attr.w,
        myOsWindow->attr.h,
        myOsWindow->attr.x,
        myOsWindow->attr.y);
    #endif
  
    #ifdef _XTHREADS
      XInitThreads();
    #endif
  
    myOsWindow->low.dpy = XOpenDisplay(NULL);
    myOsWindow->low.screen = DefaultScreen(myOsWindow->low.dpy);
    Window root;
    unsigned long valuemask;
    XWindowAttributes attr1;
    XSetWindowAttributes attr;
    attr.event_mask = ExposureMask | ButtonPressMask | ButtonReleaseMask |
      ButtonMotionMask | EnterWindowMask | LeaveWindowMask;
    attr.backing_store = Always;
    root = RootWindow(myOsWindow->low.dpy,myOsWindow->low.screen);
    XGetWindowAttributes(myOsWindow->low.dpy,root,&attr1);
    valuemask = CWEventMask;
    #ifdef _USE_OVERRIDE_REDIRECT
      valuemask += CWOverrideRedirect;
      attr.override_redirect = True;
    #endif
    
    myOsWindow->low.id = XCreateWindow(
      myOsWindow->low.dpy,
      root,
      myOsWindow->attr.x,
      myOsWindow->attr.y,
      myOsWindow->attr.w,
      myOsWindow->attr.h,
      1, DefaultDepth(myOsWindow->low.dpy,myOsWindow->low.screen),
      CopyFromParent,
      DefaultVisual(myOsWindow->low.dpy,myOsWindow->low.screen),
      valuemask, &attr
    );
    
    FILE *fd = fopen("mikaSignageWindow.log","w");
    if (fd)
    {
      unsigned char data[32];
      sprintf((char*)data,"%d\n",myOsWindow->low.id);
      fwrite((char*)data,strlen((char*)data),1,fd);
      fclose(fd);
    }
    
    #ifdef _DEBUG
      printf("lowWindowCreate id(%d)\n",myOsWindow->low.id);
    #endif
    
    XSetWindowBackground(
      myOsWindow->low.dpy,myOsWindow->low.id,
      BlackPixel(myOsWindow->low.dpy,myOsWindow->low.screen)
    );
    XSelectInput(
      myOsWindow->low.dpy,myOsWindow->low.id,
      ExposureMask|ButtonPressMask|ButtonReleaseMask|ButtonMotionMask
    );
    osWindowShow(myOsWindow);
    #ifdef _XFLUSH
      XFlush(myOsWindow->low.dpy);
    #endif
    
  #endif  
    
  return 0;
}

/** create a platform speciffic window
 */
struct osWindow *osWindowInit( struct osWindowAttr *attr )
{
  if (!attr)
    return NULL;
  struct osWindow *myOsWindow = (struct osWindow*)malloc(sizeof(struct osWindow));
  myOsWindow->attr.x = attr->x;
  myOsWindow->attr.y = attr->y;
  myOsWindow->attr.w = attr->w;
  myOsWindow->attr.h = attr->h;
  myOsWindow->attr.events.callButtonPress = attr->events.callButtonPress;
  myOsWindow->attr.events.callButtonRelease = attr->events.callButtonRelease;
  if ( lowWindowCreate(myOsWindow) != 0 )
  {
    #ifdef _DEBUG
      printf("osWindowInit error, lowWindow not created\n");
    #endif
  }
  return myOsWindow;
}

int osWindowEventLoop( struct osWindow *window )
{
  if (!window)
    return 1;
  
  #ifdef _DEBUG
    printf("osWindowEventLoop\n");
  #endif
  
  #ifdef _CF_GUI_XLIB
    static XEvent xe;
    for(;;)
    {
      XNextEvent(window->low.dpy,&xe);
      switch(xe.type)
      {
        case ButtonPress:
          if (window->attr.events.callButtonPress)
            window->attr.events.callButtonPress(xe.xbutton.x,xe.xbutton.y);
          break;
        case ButtonRelease:
          #ifdef _DEBUG
            printf("osWindowEventLoop ButtonRelease %d,%d\n",xe.xbutton.x,xe.xbutton.y);
          #endif
          if (window->attr.events.callButtonRelease)
            window->attr.events.callButtonRelease(0,0);
          break;
      }
    }
  #endif
    
  return 0;
}

int osWindowHide( struct osWindow *window )
{
  if (!window)
    return 1;
  
  #ifdef _CF_GUI_XLIB
    XUnmapWindow(window->low.dpy,window->low.id);
  #endif
    
  return 0;
}

int osWindowShow( struct osWindow *window )
{
  if (!window)
    return 1;
  
  #ifdef _CF_GUI_XLIB
    XMapWindow(window->low.dpy,window->low.id);
    #ifdef _XFLUSH
      XFlush(window->low.dpy);
    #endif
  #endif
    
  return 0;
}

int osInit( )
{
  activityFilename = (unsigned char*)malloc(256);
  return 0;
}
