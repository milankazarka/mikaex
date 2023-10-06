/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "rootController.h"
#include "rootControllerSync.h"
#include "rootSectors.h"
#include "rootScheduller.h"
#include "rootEvents.h"
#include "rootStress.h"
#include "rootModules.h"
#include "app.h"
#include "system.h"
#include "activityObject.h"
#ifdef USECAIRO
  #include <cairo/cairo.h>
#endif

#ifndef _AI
  #include "ai.h"
#endif
#define _CLOCKTIK 600
#define _STRESSTEST
#define _ANGLE_TEST_NO
#define _DUMMY_NO
#define _DRAQIE_SOCKET
#define _IDLE 20
#define _TRANSPARENCY

/**  we need to run 3 threads - one platform thread for 
 *   platform speciffic events, then the thread for receival
 *   of application messages and then the main thread
 *   processing the events from the later two
 *
 *   we need to make this more transparent
 */
static struct eventPlatform ePlatform;
static struct eventApi eApi;
struct rootDesktop *rdGlobal = NULL;
struct rootScheduller *scheduller = NULL;
static int currentx;
static int currenty;
pthread_t idleThread;
static int idle_mode = 0;

struct rootDesktop *mainRd = NULL;

struct eventPlatform eventPlatformMake( )
{
  struct eventPlatform ep;
  ep.state = _STATE_INACTIVE;
  return ep;
}

struct eventApi eventApiMake( )
{
  struct eventApi ea;
  ea.state = _STATE_INACTIVE;
  return ea;
}

int roOnPaint(
  struct rootDesktop *rd,
  struct rootObject *ro
)
{
  if (!rd || !ro)
    return -1;
  return 0;
}

/**
 *  refresh an object on the desktop - this only copies the
 *  the surface of the object to the desktop.
 *
 *  the objects refresh their surface themselves and they
 *  signal the desktop to copy the surface
 */
int rdRefreshObject(
  struct rootDesktop *rd,
  struct rootObject *ro
)
{
  if (!rd || !ro)
    return -1;
  
  return 0;
}

int rdRefresh(
  struct rootDesktop *rd
)
{
  if (!rd)
    return -1;
    
  struct rootObject *current = rd->children;
  while(current)
  {
    rdRefreshObject(rd,current);
    current = current->next;
  }  
    
  return 0;
}

/**  push object to the first position of children
 */
int rdPushObject(
  struct rootDesktop *rd,
  struct rootObject *ro
)
{
  if (!rd || !ro)
    return -1;
  
  struct rootObject *current = rd->children;
  struct rootObject *last = NULL;
  while(current)
  {
    if (current==ro)
    {
      if (last)
        last->next = current->next;
      else
        rd->children = current->next;
      break;
    }
    last = current;
    current = current->next;
  }
  
  ro->next = rd->children;
  rd->children = ro;
  
  return 0;
}

/** add an object to the root desktop
 */
int rdAddObject(
  struct rootDesktop *rd,
  struct rootObject *ro
)
{
  if (!rd || !ro)
    return -1;
  
  #ifdef _DEBUG
    printf("rdAddObject\n");
  #endif
  
  struct rootObject *current = rd->children;
  struct rootObject *last = NULL;
  
  while(current)
  {
    #ifdef _DEBUG
      printf("rdAddObject cycle\n");
    #endif
    if (current==ro)
    {
      if (last)
        last->next = current->next;
      else
        rd->children = current->next;
      break;
    }
    last = current;
    current = current->next;
  }
  
  current = rd->children;
  if (!current)
  {
    rd->children = ro;
  }
  else
  {
    while(current)
    {
      if (!current->next)
      {
        current->next = ro;
        break;
      }
      current = current->next;
    }
  }
  
  ro->next = NULL;
  
  return 0;
}

int rdRemoveObject(
  struct rootDesktop *rd,
  struct rootObject *ro
)
{

  if (!rd || !ro)
    return -1;
  
  #ifdef _DEBUG
    printf("rdRemoveObject %p\n",ro);
  #endif
  
  struct rootObject *last = NULL;
  struct rootObject *current = rd->children;
  while(current)
  {
    if (current==ro)
    {
      if (current==rd->children)
      {
        if (current->next)
        {
          rd->children = current->next;
        }
        else
        {
          rd->children = NULL;
        }
      }
      else
      {
        if (current->next)
        {
          if (last)
          {
            last->next = current->next;
          }
        }
        else
        {
          if (last)
          {
            last->next = NULL;
          }
        }
      }
      free(current);
    }
    last = current;
    current = current->next;
  }  
  
  return 0;
}

/** apply transparency to the surface of the object
 */
int rdObjectTransparency( void *surface, int w, int h, unsigned char trans )
{
  #ifndef _TRANSPARENCY
    return 0;
  #endif
  if (!surface)
    return 1;
  
  int n;
  int run = 0;
  unsigned char data[1];
  unsigned char clear[1];
  data[0] = trans;
  clear[0] = 0x00;
  for(n = 0; n < w*h*4; n++)
  {
    if (run==3)
    {
      if (memcmp(surface+n,(void*)clear,1)!=0)
        memcpy(surface+n,(void*)data,1);
      run = 0;
    }
    else
      run++;
  }  
    
  return 0;
}

/**  draw the object's surface onto the desktop
 *   needs to be sync
 */
#define _GENERAL_TRANSPARENCY // apply transparency to all objects
int rdDrawObjectSurfaceESYNC(
  struct rootDesktop *rd,
  struct rootObject *ro,
  cairo_t *cairo
)
{
  static int forceangle = -1;
  static void *surface = NULL;
  static cairo_surface_t *image = NULL;
  static int sector = _ROOT_SECTOR_NONE;
  static activityAttributes attributes;

  if (!rd || !ro)
    return -1;

  #ifdef _DEBUG
    printf("rdDRawObjectSurfaceESYNC\n");
  #endif
  
  ro->act->shm->data = shmat(ro->act->shm->id,0,0);
  if (!ro->act->shm->data)
  {
    #ifdef _DEBUG
      printf("rdDrawObjectSurfaceESYNC no surface returned, remove object\n");
    #endif
    return -1;
  }
  
  activitySHMGetAttributes(ro->act,&attributes);
  
  if (attributes.visible==1)
  {
    surface = activitySHMGetSurface(ro->act);
    if (surface)
    { 
      #ifndef _GENERAL_TRANSPARENCY
        if (ro->next)
      #endif
      rdObjectTransparency(surface,ro->frame.w,ro->frame.h,0xc0);
    
      image = cairo_image_surface_create_for_data(
          surface,
          CAIRO_FORMAT_ARGB32,
          ro->frame.w,ro->frame.h,
          ro->frame.w*4
      );
      #ifdef _ANGLE_TEST
        ro->frame.angle+=1;
      #endif
      
      forceangle = activityGetForceangle(ro->act,0);
      
      if (forceangle==-1)
      {
        sector = rootSectorByPosition(
          ro->frame.x,ro->frame.y,
          ro->frame.w,ro->frame.h,
          rd->frame.w,rd->frame.h
        );
    
        switch(sector)
        {
          case _ROOT_SECTOR_LEFT:
            ro->frame.angle=90;
          break;
          case _ROOT_SECTOR_RIGHT:
            ro->frame.angle=270;
          break;
          case _ROOT_SECTOR_UP:
            ro->frame.angle=180;
          break;
          case _ROOT_SECTOR_DOWN:
            ro->frame.angle=0;
          break;
          case _ROOT_SECTOR_LEFTUP:
            ro->frame.angle=135;
          break;
          case _ROOT_SECTOR_LEFTDOWN:
            ro->frame.angle=45;
          break;
          case _ROOT_SECTOR_RIGHTUP:
            ro->frame.angle=225;
          break;
          case _ROOT_SECTOR_RIGHTDOWN:
            ro->frame.angle=315;
          break;
        }
      }
      else
      {
        ro->frame.angle = forceangle;
      }
    
      activitySetAngle(ro->act,ro->frame.angle,0);
    
      cairo_save(cairo);
      
      // transformation
      cairo_translate(cairo,
          (double)(ro->frame.w/2)+ro->frame.x,
          (double)(ro->frame.h/2)+ro->frame.y);
      cairo_rotate(cairo,ro->frame.angle*3.14/180);
      cairo_scale(cairo,
        (double)(ro->frame.w/ro->frame.w),
        (double)(ro->frame.h/ro->frame.h));
      cairo_translate(cairo,
        (double)(-0.5*ro->frame.w),
        (double)(-0.5*ro->frame.h));
      
      cairo_set_source_surface(cairo,image,0,0);
      cairo_paint(cairo);
      cairo_surface_destroy(image);
    
      cairo_restore(cairo);
    }
  }
  shmdt(ro->act->shm->data);
  
  #ifdef _DEBUG
    printf("rdDRawObjectSurfaceESYNC end\n");  
  #endif
    
  return 0;
}

/**
 *  redraw all active object's surfaces to the desktop
 *  it's safer to recreate cairo on every draw although
 *  it's more time consuming
 */
#define _CAIRO_REUSE_NO
#define _BG_IMAGE_NO
int rdDrawObjectsSurfaceSYNC(
  struct rootDesktop *rd
)
{
  #ifdef _DEBUG
    printf("rdDrawObjectsSurfaceSYNC\n");
  #endif
  
  static struct rootObject *current;
  static cairo_t *cairo = NULL;
  static cairo_surface_t *destination = NULL;
  static cairo_surface_t *bg = NULL;

  if (!rd)
    return -1;
  
  binarySemaphoreWait(rd->sem->id);
  
  void *surfaceData = shmat(rd->shmSurface->id,0,0);
  memset(surfaceData,0x20,rd->frame.w*rd->frame.h*4);
  #ifdef _CAIRO_REUSE
    if (!cairo)
  #endif
  {
    destination = cairo_image_surface_create_for_data(
      surfaceData,
      CAIRO_FORMAT_ARGB32,
      rd->frame.w,
      rd->frame.h,
      4*rd->frame.w
    );
    cairo = cairo_create(destination);
  
    #ifdef _BG_IMAGE
      if (!bg)	
        bg = cairo_image_surface_create_from_png(_DEFAULT_BG);
      if (bg)
      {
        cairo_set_source_surface(cairo, bg, 0, 0);
        cairo_paint(cairo);
      }
    #endif
  }
  
  current = rd->children;
  while(current)
  {
    rdDrawObjectSurfaceESYNC(rd,current,cairo);
    current = current->next;
  }
  
  #ifndef _CAIRO_REUSE
    cairo_surface_destroy(destination);
    cairo_destroy(cairo);
  #endif
  
  shmdt(surfaceData);
  binarySemaphorePost(rd->sem->id);
  #ifdef _DEBUG
    printf("rdDrawObjectsSurfaceSYNC finish\n");
  #endif
}

/**  copy the pixels onto the platform speciffic window
 *
 */
int windowEventExpose(
  struct rootDesktop *rd
)
{
  static XImage *image = NULL;
  static XPixmapFormatValues	*formats = NULL;
  static XPixmapFormatValues	*format = NULL;
  
  #ifdef _DEBUG
    static unsigned long timestamp = 0;
    static unsigned long timestamp2 = 0;
    timestamp = schedullerTime();
    printf("windowEventExpose scheduller start(%d)\n",timestamp);
  #endif

  rdDrawObjectsSurfaceSYNC(rd);
  
  #ifdef _DEBUG
    printf("windowEventExpose wait.start\n");
  #endif        
  binarySemaphoreWait(rd->sem->id);
  #ifdef _DEBUG
    printf("windowEventExpose wait.finish\n");
  #endif
  
  if (!formats)
  {
    int count = 0;        
    formats = XListPixmapFormats(rd->window->dpy,&count);
    for(format = formats; count > 0; count--, format++)
    {
      if (format->depth == DefaultDepth(rd->window->dpy,rd->window->screen))
      {
        break;
      }
    }
  }
  
  void *data = shmat(rd->shmSurface->id,0,0);
          
  if (!image)
  {
    image = XCreateImage(
          rd->window->dpy,
          DefaultVisual(rd->window->dpy,rd->window->screen),
          DefaultDepth(rd->window->dpy,rd->window->screen),
          ZPixmap,
          0,
          data,
          rd->frame.w,
          rd->frame.h,
          format->scanline_pad,0);
  
          image->data = data;
  }
          
  XPutImage(
          rd->window->dpy,
          rd->window->id,
          DefaultGC(
            rd->window->dpy,
            rd->window->screen
          ),
          image,
          0,0,0,0,
          rd->frame.w,
          rd->frame.h
        );    
          
  shmdt(data);
  
  XFlush(rd->window->dpy);
  binarySemaphorePost(rd->sem->id);
  
  #ifdef _DEBUG
    timestamp2 = schedullerTime();
    printf("windowEventExpose scheduller finish(%d) dauer(%d)\n",
      timestamp2,timestamp2-timestamp);
  #endif
          
  return 0;
}

int windowEventPollReceiver(
  struct rootDesktop *rd,
  struct rootObject *ro
)
{
  if (!rd||!ro)
    return -1;
  // todo
  return 1;
}

/** is object1 higher than object2
 */
int _object_is_higher( 
  struct rootDesktop *rd,
  struct rootObject *object1, 
  struct rootObject *object2 )
{
  struct rootObject *current = rd->children;
  while(current)
  {
    if (object2==current)
      return 1;
    else if (object1==current)
      return 0;
    current = current->next;
  }
  return 0;
}

/** in a range of pixels we find the most upper object
 */
struct rootObject *_object_top( struct rootDesktop *rd, int *_x, int *_y )
{
  struct rootObject *object = NULL;
  
  int x = *_x;
  int y = *_y;
  
  int nx;
  int ny;
  struct rootObject *active = NULL;
  struct rootObject *topActive = NULL;
  for(
    ny = y-(IO_TOUCH_FRAME*IO_TOUCH_STEP); 
    ny < y+(IO_TOUCH_FRAME*IO_TOUCH_STEP); ny++)
  {
    nx = x-(IO_TOUCH_FRAME*IO_TOUCH_STEP);
    for(nx; nx < x+(IO_TOUCH_FRAME*IO_TOUCH_STEP); nx++ )
    {
      struct rootObject *active = rdPositionToObject(rd,nx,ny);
      
      if (!topActive && active)
      {
        topActive = active;
      }
      else if (topActive!=active && active)
      {
        if (_object_is_higher(rd,active,topActive))
        {
          *_x = nx;
          *_y = ny;
          topActive = active;
        }
      }
    }
  }
  
  object = topActive;
  
  return object;
}

/**  resend Button Down event through activity
 *
 */
#define _CREATE_ACTIVITY_NO
int windowEventButtonDown(
  struct rootDesktop *rd,
  struct rootObject *active,
  int x,
  int y
)
{
  if (!rd)
    return -1;
  
  #ifdef _DEBUG
    printf("windowEventButtonDown\n");
  #endif
  
  binarySemaphoreWait(rd->sem->id);
  #ifdef _AI
    if (active)
    {
      rdAddObject(rd,active);
      #ifdef _CREATE_ACTIVITY
        rx = x-active->frame.x;
        ry = y-active->frame.y;
        activityAddEventButtonDown(active->act,rx,ry);
      #endif
      rd->active = active;
    }
    else
    {
      // clicked on the Desktop
      #ifdef _DEBUG
        printf("windowEventButtonDown Desktop\n");
      #endif
    }
  #else // deprecated
    int rx;
    int ry;
    active = rdPositionToObject(rd,x,y);
    if (active)
    {
      rdAddObject(rd,active);
      #ifdef _CREATE_ACTIVITY
        rx = x-active->frame.x;
        ry = y-active->frame.y;
        activityAddEventButtonDown(active->act,rx,ry);
      #endif
      rd->active = active;
    }
    else
    {
      // clicked on the Desktop
      #ifdef _DEBUG
        printf("windowEventButtonDown Desktop\n");
      #endif
    }
  #endif
  binarySemaphorePost(rd->sem->id);
    
  return 0;
}

int windowEventButtonUp(
  struct rootDesktop *rd,
  struct rootObject *active,
  int x,
  int y
)
{
  if (!rd || !active)
    return 1;
  
  return 0;
}

int triggerExposeRESET(
  struct rootDesktop *rd,
  XEvent *event
)
{
  if (!rd)
    return -1;
  
  binarySemaphoreWait(rd->sem->id);
  if (eApi.state = _STATE_INACTIVE)
  {
    eApi.xevent = *event;
    eApi.state = _STATE_ACTIVE;
    binarySemaphorePost(rd->sem->id);
    return 1;
  }
  binarySemaphorePost(rd->sem->id);  
    
  return 0;
}

/**
 *  todo:
 *  triggerExposure (from any thread)
 *   +- insert thread into SHM
 *    +- fetch event in event loop (multiplatform)
 *     +- process and resend to platform
 *
 *  we should have two event loops - one to fetch events
 *  from the platform and one to process multiplatform
 *  events
 */
#define _EVENT_INJECT_DIRECT_NO
int triggerExpose(
  struct rootDesktop *rd
)
{
  if (!rd)
    return -1;
  
  #ifdef _DEBUG
    printf("triggerExpose\n");
  #endif
  
  XEvent event;
  event.xexpose.window = rd->window->id;
  event.xexpose.display = rd->window->dpy;
  event.xexpose.type = Expose;
  event.xexpose.send_event = True;
  event.xexpose.x = 0;
  event.xexpose.y = 0;
  event.xexpose.width = rd->frame.w;
  event.xexpose.height = rd->frame.h;
  
  #ifdef _EVENT_INJECT_DIRECT    
    XSendEvent(
      rd->window->dpy,
      rd->window->id, 0,
      ExposureMask,&event
    );
  #else
    while(triggerExposeRESET(rd,&event)!=1)
    {
      usleep(_CLOCKTIK/5);
    }
  #endif
  return 0;
}

/**  poll children's exposure
 */
#define _DIRECT_ACCESS_NO
int pollExposures(
  struct rootDesktop *rd
)
{
  static struct rootObject *current = NULL;
  static struct activityEvent activityEvent;
  static int redraw = 0;
  
  if (!rd)
    return -1;
  
  #ifdef _DEBUG
    printf("pollExposures\n");
  #endif
  
  redraw = 0;
  
  #ifndef _DUMMY
    #ifdef _DEBUG
      printf("pollExposures poll wait.start\n");
    #endif
    binarySemaphoreWait(rd->sem->id);
    #ifdef _DEBUG
      printf("pollExposures poll wait.finish\n");
    #endif
    
    current = rd->children;
    while(current)
    {
      if (activityEventActive(current->act)==1)
      {
        if (activityEventIsEvent(current->act,_ACTIVITY_EVENT_EXPOSE)==1)
        {
          activityEventGet(current->act,&activityEvent);
          redraw = 1;
        }
      }
      #ifdef _STRESSTEST
        redraw = 1; // test, force
      #endif
    
      current = current->next;
    }
    binarySemaphorePost(rd->sem->id);
  
    #ifdef _DEBUG
      printf("pollExposures expose\n");
    #endif
    
    if (redraw==1)
    {
      #ifdef _DIRECT_ACCESS
        windowEventExpose(rd);
      #else
        #ifdef _DEBUG
          printf("pollExposures triggerExpose start\n");
        #endif
        triggerExpose(rd);
        #ifdef _DEBUG
          printf("pollExposures triggerExpose finish\n");
        #endif
      #endif
    }
  #endif
  
  #ifdef _DEBUG
    printf("pollExposures end\n");
  #endif
    
  return 0;
}

void *pollExposuresThread( void *data )
{
  struct rootDesktop *rd = (struct rootDesktop*)data;
  while(1)
  {
    pollExposures(rd);
    usleep(_CLOCKTIK);
  }
}

/**  poll the objects for exposure events
 *
 */
int pollExposuresStart(
  struct rootDesktop *rd
)
{
  if (!rd)
    return -1;

  struct rootAtom *atom = ratomAlloc(rd,NULL);
  atom->data = rd;
  atom->call = pollExposuresThread;
  ratomInit(rd,atom);

  return 0;
}

int lowWindowEvent(
  struct rootDesktop *rd
)
{
  static Display *dpy = NULL;
  static XEvent xe;

  if (!rd)
    return -1;
  
  #ifdef XLIB
    if (!dpy)
      dpy = rd->window->dpy;
    XNextEvent(dpy,&xe);
    
    while(1)
    {
      binarySemaphoreWait(rd->sem->id);
      if (ePlatform.state == _STATE_INACTIVE) // 
      {
        binarySemaphorePost(rd->sem->id);
        break;
      }
      else
      {
        #ifdef _DEBUG
          printf("lowWindowEvent waiting for _STATE_INACTIVE\n");
        #endif
      }
        
      binarySemaphorePost(rd->sem->id);
      usleep(_CLOCKTIK/5);
    }
    
    binarySemaphoreWait(rd->sem->id);
    ePlatform.event = xe;
    ePlatform.state = _STATE_ACTIVE;
    binarySemaphorePost(rd->sem->id);
  #endif

  return 0;
}

int lowWindowLoop(
  struct rootDesktop *rd
)
{
  if (!rd)
    return -1;
  for(;;)
  {
    lowWindowEvent(rd);
  }
  return 0;
}

/** todo - wrap in a platform thread
 *
 */
void *lowWindowLoopThread(
  void *data
)
{
  struct rootDesktop *rd = (struct rootDesktop*)data;
  lowWindowLoop(rd);
}

#ifndef _USE_OVERRIDE
  #define _USE_OVERRIDE
#endif
struct lowWindow *lowWindowInit(
  struct rootDesktop *rd,
  struct lowWindow *lw
)
{
  if (!rd)
    return NULL;
  if (!lw)
  {
    lw = (struct lowWindow*)malloc(sizeof(struct lowWindow));
  }
  #ifdef XLIB
    lw->dpy = XOpenDisplay(NULL);
    lw->screen = DefaultScreen(lw->dpy);
    Window root;
    unsigned long valuemask;
    XWindowAttributes attr1;
    XSetWindowAttributes attr;
    attr.event_mask = ExposureMask | ButtonPressMask | ButtonReleaseMask |
      ButtonMotionMask | EnterWindowMask | LeaveWindowMask;
    root = RootWindow(lw->dpy,lw->screen);
    XGetWindowAttributes(lw->dpy,root,&attr1);
    valuemask = CWEventMask;
    #ifdef _USE_OVERRIDE
      valuemask += CWOverrideRedirect;
      attr.override_redirect = True;
    #endif
  
    XInitThreads();
  
    lw->id = XCreateWindow(
      lw->dpy,
      root,
      rd->frame.x,
      rd->frame.y,
      rd->frame.w,
      rd->frame.h,
      1, DefaultDepth(lw->dpy,lw->screen),
      CopyFromParent,
      DefaultVisual(lw->dpy,lw->screen),
      valuemask, &attr
    );
    
    XSetWindowBackground(lw->dpy,lw->id,
      BlackPixel(lw->dpy,lw->screen));
    XSelectInput(lw->dpy,lw->id,
      ExposureMask|ButtonPressMask|ButtonReleaseMask|ButtonMotionMask);
    XMapWindow(lw->dpy,lw->id);
  #endif
  return lw;
}

struct rootAtom *ratomAlloc(
  struct rootDesktop *rd,
  struct rootAtom *atom
)
{
  if (!rd)
    return NULL;
  
  if (!atom)
  {
    atom = (struct rootAtom*)malloc(sizeof(struct rootAtom));
  }  
    
  return atom;
}

/**
 *  run the thread
 */
int ratomInit(
  struct rootDesktop *rd,
  struct rootAtom *atom
)
{
  if (!rd || !atom)
    return -1; 
  
  pthread_create(&atom->id,NULL,atom->call,atom->data);
    
  return 0;
}

/** we process a ButtonRelease event only if it
 *  occurs in a (_EVENT_TIMER_CLICK*_EVENT_TIMER_WINDOW)
 *  time window, resp. after a time lapse
 */
#define _EVENT_TIMER_CLICK 100000
#define _EVENT_TIMER_WINDOW 10
XEvent 	eplast;
/** _event_timer_brelease has four states:
 *	0 - no button release event pending
 *	1 - button release pending
 *	2 - button release event execute
 *		+ reset to 0
 *	3 - reset timer
 */
int 	_event_timer_brelease = 0;
void *_event_timer(void*data)
{
  int counter = 0;
  struct rootDesktop *rd = (struct rootDesktop*)data;
  for(;;)
  {
    binarySemaphoreWait(rd->sem->id);
    
    if (_event_timer_brelease==3)
    {
      _event_timer_brelease = 1;
      counter = 0;
    }
      
    if (_event_timer_brelease==1)
    {
      if (counter!=_EVENT_TIMER_WINDOW)
      {
        counter++;
      }
      else
      {
        _event_timer_brelease = 2;
        ePlatform.state = _STATE_ACTIVE;
        ePlatform.event = eplast;
        counter = 0;
      }
    }
    binarySemaphorePost(rd->sem->id);
    usleep(_EVENT_TIMER_CLICK);
  }  
}

/** possibly reverse buttons after deremining the window
 */
#define _REVERSE_BUTTONS
int rdEventHandler(
  struct rootDesktop *rd
)
{
  if (!rd)
    return -1;
  
  XEvent epcopy;
  
  int x;
  int y;
  int rx;
  int ry;
  int oldx = 0;
  int oldy = 0;
  int sector;
  struct rootObject *active = NULL;
  
  #ifdef _AI
    pthread_t t;
    pthread_create(&t,NULL,_event_timer,rd);
  #endif
  
  for(;;)
  {
    binarySemaphoreWait(rd->sem->id);
    if (ePlatform.state == _STATE_ACTIVE)
    {
      ePlatform.state = _STATE_INACTIVE;
      epcopy = ePlatform.event;
      #ifdef _AI
        eplast = epcopy;
      #endif
      binarySemaphorePost(rd->sem->id);
      goto platformhit;
    }
    if (eApi.state == _STATE_ACTIVE)
    {
      eApi.state = _STATE_INACTIVE;
      epcopy = eApi.xevent;
      #ifdef _AI
        eplast = epcopy;
      #endif
      binarySemaphorePost(rd->sem->id);
      goto platformhit;
    }
    binarySemaphorePost(rd->sem->id);
    usleep(_CLOCKTIK);
    continue;
  platformhit:
    switch(epcopy.type)
    {
      case ButtonPress:
        idle_mode = 1;
        #ifdef _DEBUG
          printf("lowWindowEvent ButtonPress\n");
        #endif
        
        binarySemaphoreWait(rd->sem->id);
        #ifdef _AI
          active = _object_top(rd,&epcopy.xbutton.x,&epcopy.xbutton.y);
        #else
          active = rdPositionToObject(rd,epcopy.xbutton.x,epcopy.xbutton.y);
        #endif
        binarySemaphorePost(rd->sem->id);
        
        currentx = epcopy.xbutton.x;
        currenty = epcopy.xbutton.y;
        
        oldx = currentx;
        oldy = currenty;
        
        if (active)
        {
          rx = epcopy.xbutton.x-active->frame.x;
          ry = epcopy.xbutton.y-active->frame.y;
            
          if (1) // could cause synchronisation problems
          {
            struct _frame frame;// = *active->frame;
            memcpy(&frame,&active->frame,sizeof(struct _frame));
            sector = rootSectorByPosition(
                frame.x,frame.y,
                frame.w,frame.h,
                rd->frame.w,rd->frame.h);
          }
          
          if (sector!=_ROOT_SECTOR_CENTER)
              active->frame.sector = sector;
          switch(active->frame.sector)
            {
              case _ROOT_SECTOR_RIGHT:
                x = rx;
                rx = active->frame.w-ry;
                ry = x;
                break;
              case _ROOT_SECTOR_LEFT:
                y = ry;
                ry = active->frame.w-rx;
                rx = y;
                break;
              case _ROOT_SECTOR_UP:
                rx = active->frame.w-rx;
                ry = active->frame.w-ry;
                break;
              case _ROOT_SECTOR_DOWN:
                break;
            }
          
          rootSchedullerButtondown(scheduller,active,rx,ry);
          windowEventButtonDown(rd,active,epcopy.xbutton.x,epcopy.xbutton.y);
          #ifdef _DEBUG
            printf("_coordinates %d/%d\n",rx,ry);
          #endif
        }
      break;
      case ButtonRelease:
        #ifdef _DEBUG
          printf("lowWindoEvent ButtonRelease\n");
        #endif
        
        /** we only accept a Release if it's in a timely window
         *  _AI logic, which defeats broken sensor events
         *
         *  only a release is processed, which is in a timely
         *  window
         */
        #ifdef _AI
          if (rd->active)
          {
            binarySemaphoreWait(rd->sem->id);
            if (_event_timer_brelease==0)
            {
              _event_timer_brelease = 1;
              binarySemaphorePost(rd->sem->id);
              break;
            }
            else if (_event_timer_brelease==1)
            {
              _event_timer_brelease = 3;
              binarySemaphorePost(rd->sem->id);
              break;
            }
            else if (_event_timer_brelease==2)
            {
              #ifdef _DEBUG
                printf("lowWindowEvent _event_timer_brelease=2\n");
              #endif
              _event_timer_brelease = 0;
              binarySemaphorePost(rd->sem->id);
              goto button_release_continue;
            }
            else if (_event_timer_brelease==3);
            {
              binarySemaphorePost(rd->sem->id);
              goto button_release_continue;
            }
            binarySemaphorePost(rd->sem->id);
          }
      button_release_continue:
        #endif
        
        idle_mode = 1;
        rd->active = NULL;
        
          binarySemaphoreWait(rd->sem->id);
          #ifdef _AI
            active = _object_top(rd,&epcopy.xbutton.x,&epcopy.xbutton.y);
          #else
            active = rdPositionToObject(rd,epcopy.xbutton.x,epcopy.xbutton.y);
          #endif
          binarySemaphorePost(rd->sem->id);
          
          if (active)
          {
            rx = epcopy.xbutton.x-active->frame.x;
            ry = epcopy.xbutton.y-active->frame.y;
            
            if (1) // could cause synchronisation problems
            {
              struct _frame frame;// = *active->frame;
              memcpy(&frame,&active->frame,sizeof(struct _frame));
              sector = rootSectorByPosition(
                frame.x,frame.y,
                frame.w,frame.h,
                rd->frame.w,rd->frame.h);
            }
            
            if (sector!=_ROOT_SECTOR_CENTER)
              active->frame.sector = sector;
             
            switch(active->frame.sector)
            {
              case _ROOT_SECTOR_RIGHT:
                x = rx;
                rx = active->frame.w-ry;
                ry = x;
                break;
              case _ROOT_SECTOR_LEFT:
                y = ry;
                ry = active->frame.w-rx;
                rx = y;
                break;
              case _ROOT_SECTOR_UP:
                rx = active->frame.w-rx;
                ry = active->frame.w-ry;
                break;
              case _ROOT_SECTOR_DOWN:
                break;
            }
          
            #ifdef _DEBUG
              printf("_coordinates %d/%d\n",rx,ry);
            #endif
          
            if (
              ( 
              oldx-10<epcopy.xbutton.x &&
              oldx+10>epcopy.xbutton.x &&
              oldy-10<epcopy.xbutton.y &&
              oldy+10>epcopy.xbutton.y ) ||
              ( 
              oldx == 0 &&
              oldy == 0 )
              )
            {
              #ifdef _DEBUG
                printf("_coordinates activityAddEventButtonDown\n");
              #endif
              rootSchedullerButtonup(scheduller);
            }
          }
        
      break;
      case MotionNotify:
        idle_mode = 1;
        #ifdef _DEBUG
          printf("lowWindowEvent Motion (%d,%d)\n",
            epcopy.xmotion.x,epcopy.xmotion.y);
        #endif
        
        binarySemaphoreWait(rd->sem->id);
        
        if (rd->active)
        {
          active = rd->active;
          if (1) // could cause synchronisation problems
          {
            struct _frame frame;// = *active->frame;
            memcpy(&frame,&active->frame,sizeof(struct _frame));
            sector = rootSectorByPosition(
                frame.x,frame.y,
                frame.w,frame.h,
                rd->frame.w,rd->frame.h);
          }
          
          if (sector!=_ROOT_SECTOR_CENTER)
            active->frame.sector = sector;
  
          #ifdef _DEBUG
            printf("lowWindowEvent motion on active\n");
          #endif
          
          if (activityGetMovable(rd->active->act,0)==1)
          {
            rd->active->frame.x += epcopy.xmotion.x-currentx;
            rd->active->frame.y += epcopy.xmotion.y-currenty;
          }
          #ifdef _ANGLE_TEST
            rd->active->frame.angle += 1;
          #endif
          binarySemaphorePost(rd->sem->id);
          rootSchedullerExpose(scheduller);
          binarySemaphoreWait(rd->sem->id);
          
          if (
              (
              oldx+10<epcopy.xbutton.x ||
              oldx-10>epcopy.xbutton.x ||
              oldy+10<epcopy.xbutton.y ||
              oldy-10>epcopy.xbutton.y) &&
              rd->active
            )
          {
            rootSchedullerButtondownReset(scheduller);
          }
        }
        currentx = epcopy.xmotion.x;
        currenty = epcopy.xmotion.y;
        
        binarySemaphorePost(rd->sem->id);
        #ifdef _DEBUG
          printf("lowWindowEvent Motion finish processing\n");
        #endif
      break;
      case Expose:
        #ifdef _DEBUG
          printf("lowWindowEvent Expose\n");
        #endif
        rootSchedullerExpose(scheduller);
      break;
    }
    continue;
  apihit:
    continue;
  }
    
  return 0;
}

int rdEventHandlerSocket(
  struct rootDesktop *rd
)
{
  if (!rd)
    return -1;
  
  rdEventHandler(rd);  
    
  return 0;
}

/** initialize the root desktop object
 */
struct rootDesktop *rdInit(
  struct rootDesktop *rd,
  struct _frame frame
)
{
  if (!rd)
  {
    rd = (struct rootDesktop*)malloc(sizeof(struct rootDesktop));
    if (!rd)
    {
      printf("rdInit error - could not allocate rd\n");
    }
  }
  
  rdGlobal = rd;
  mainRd = rdGlobal;
  
  rd->children = NULL;
  rd->frame = frame;
  rd->ePlatform = &ePlatform;
  
  rd->shmSurface = rshmInit(rd,NULL);
  int ret = rshmAlloc(rd,rd->shmSurface,frame.w*frame.h*4);
  rd->sem = rsemInit(rd,NULL);
  
  // register the root semaphore in the activityObjects pool
  struct activityObjects *objects = activityObjectsClone();
  if (objects)
  {
    #ifdef _DEBUG
      printf("rdInit register rootsemaphore\n");
    #endif
    if (activityObjectRegister(objects,(unsigned char*)"rootsemaphore")==0)
    {
      #ifdef _DEBUG
        printf("rdInit register rootsemaphore ok\n");
      #endif
      struct activityObject osemaphore;
      activityObjectClone(objects,(unsigned char*)"rootsemaphore",&osemaphore);
      sprintf((char*)osemaphore.fastData,"%d",rd->sem->id);
      activityObjectSet(objects,(unsigned char*)"rootsemaphore",&osemaphore);
      #ifdef _DEBUG
        printf("rdInit register rootsemaphore lock test\n");
      #endif
      rootControllerLock();
      rootControllerUnlock();
    }
    #ifdef _DEBUG
      printf("rdInit register rootsemaphore finish\n");
    #endif
  }
  
  rd->atomActPoll = ratomAlloc(rd,NULL);
  rd->atomActPoll->data = rd;
  rd->atomActPoll->call = rdPollObjectsThread;
  #ifndef _DRAQIE_SOCKET
    ratomInit(rd,rd->atomActPoll);
  #endif
  
  // start the thread polling exposure events of objects
  #ifndef _DRAQIE_SOCKET
    pollExposuresStart(rd);
  #endif
  
  FILE *fd;
  unsigned char buf[_FILE_LINESIZE];
  #ifdef _DEBUG
    printf("rdInit open(%s)\n",(char*)activityFilename);
  #endif
  if ((fd = fopen((char*)activityFilename,"a"))==NULL)
  {
    #ifdef _DEBUG
      printf("rshmAlloc error could not open (%s)\n",
        _SHM_ID_FILENAME);
    #endif
  }
  
  sprintf((char*)buf,"SEMID %d\n",rd->sem->id);
  fwrite((void*)buf,strlen((char*)buf),1,fd);
  fclose(fd);
  
  #ifdef _DEBUG
    printf("rdInit shmSurface id(%d)\n",ret);
  #endif
  rd->window = lowWindowInit(rd,NULL);
  
  rd->atomPlatformPoll = ratomAlloc(rd,NULL);
  rd->atomPlatformPoll->data = rd;
  rd->atomPlatformPoll->call = lowWindowLoopThread;
  ratomInit(rd,rd->atomPlatformPoll);
  
  struct rootModules *myModules = rootModulesNew();
  rootModulesRun(myModules);
  
  #ifdef _DRAQIE_SOCKET
    rdEventHandlerSocket(rd);
  #else
    rdEventHandler(rd);
  #endif
  
  return rd;
}

/**
 * initialize an empty shm descriptor
 */
struct rootShm *rshmInit(
  struct rootDesktop *rd,
  struct rootShm *shm
)
{
  return os_rshmInit(shm);
}

/**
 * initialize a semaphor for inter-process syncing
 */
struct rootSem *rsemInit(
  struct rootDesktop *rd,
  struct rootSem *sem
)
{
  return os_rsemInit(sem);
}

/**
 * alocate the memmory for the shm object
 */
int rshmAlloc(
  struct rootDesktop *rd,
  struct rootShm *shm,
  int size
)
{
  if (!rd || !shm || size < 1)
    return -1;
  
  os_rshmAlloc(shm,size,_ACTIVITY_TYPE_CTRL,"ROOT");
  
  return shm->id;
}

/**
 * initialize rootObject
 */
struct rootObject *roInit(
  struct rootDesktop *rd,
  struct rootObject *ro,
  struct _frame frame
)
{
  if (!ro)
  {
    ro = (struct rootObject*)malloc(sizeof(struct rootObject));
    ro->frame = frame;
  }
  ro->shm = rshmInit(rd,NULL);
  ro->name[0] = 0x00;
  
  ro->frame.sector = _ROOT_SECTOR_DOWN;
  
  return ro;
}

/** set the objects to unpolled for this iteration
 */
int rdUnpollObjects(
  struct rootDesktop *rd
)
{
  struct rootObject *current;
  
  current = rd->children;
  while(current)
  {
    current->poll = 0;
    current = current->next;
  }

  return 0;
}

/**  check if an object of a name exists
 *   (0,1)
 */
int rdObjectNamedExists(
  struct rootDesktop *rd,
  unsigned char *name
)
{
  struct rootObject *current;
  
  current = rd->children;
  while(current)
  {
    if (current->name[0]!=0x00)
    {
      if (
        strlen((char*)current->name)==strlen((char*)name)
      )
      {
        if (strcmp((char*)current->name,(char*)name)==0)
        {
          current->poll = 1;
          return 1;
        }
      }
    }
    current = current->next;
  }
  
  return 0;
}

/**  remove activities, that no longer run
 */
int rdCleanupObjects(
  struct rootDesktop *rd
)
{
  
  struct rootObject *current;
  struct rootObject *next;
  
  current = rd->children;
  while(current)
  {
    if (current->poll==0)
    {
      next = current->next;
      rdRemoveObject(rd,current);
      current = next;
      continue;
    }
    current = current->next;
  }
  
  return 0;
}

/**  poll the activity directory for new activities
 */
#define _POLL_LIMIT 0
int rdPollObjects(
  struct rootDesktop *rd
)
{
  static unsigned char path[128];

  if (!rd)
    return -1;
  
  binarySemaphoreWait(rd->sem->id);
  rdUnpollObjects(rd);
  
  static struct _frame frame;
  struct dirent *dp;
  DIR *dir = opendir("./activity");
  
  while((dp=readdir(dir)) != NULL)
  {
    if (dp->d_name[0]=='.')
      continue;
    
    if (
      strlen((char*)dp->d_name)>3
    )
    {
      if (strncmp((char*)dp->d_name,"APP",3)!=0)
      {
        continue;
      }
    }
    
    sprintf((char*)path,"./activity/%s",dp->d_name);
    
    if (
      rdObjectNamedExists(rd,dp->d_name)==0
    )
    {
      #ifdef _DEBUG
        printf("rdPollObjects register activity\n");
      #endif
      
      // todo - this isn't needed
      frame.w = _FRAME_DEFAULTW;
      frame.h = _FRAME_DEFAULTH;
      frame.x = (rd->frame.w/2)-(frame.w/2);
      frame.y = (rd->frame.h/2)-(frame.h/2); // will be reset in next steps
      
      frame.angle = 0;
      rootObject *ro = roInit(rd,NULL,frame);
      
      ro->act = activityAlloc(NULL);
      if ( activityInit(ro->act,path)==0 ) // this is a copy of an activity (refactor)
      {
        #ifdef _DEBUG
          printf("rdPollObjects initialized activity SHMID(%d) SEMID(%d)\n",
            ro->act->shm->id,ro->act->sem->id);
        #endif
      }
      
      // make separate method
      void *shmdata = shmat(ro->act->shm->id,0,0);
      ro->act->shm->data = shmdata;
      struct activityAttributes attributes;
      activitySHMGetAttributes(ro->act,&attributes);
      ro->frame.w = attributes.w;
      ro->frame.h = attributes.h;
      ro->frame.x = (rd->frame.w/2)-(ro->frame.w/2);
      ro->frame.y = (rd->frame.h/2)-(ro->frame.h/2);
      shmdt(shmdata);
      
      sprintf((char*)ro->name,"%s",dp->d_name);
      ro->poll = 1;
      ro->next = NULL;
      #ifdef _DEBUG
        printf("rdPollObjects rdAddObject %p\n",ro);
      #endif
      rdAddObject(rd,ro);
    }
  }
  closedir(dir);
  
  rdCleanupObjects(rd);
  binarySemaphorePost(rd->sem->id);
  
  return 0;
}

#define _POLL_OBJECTS_INTERVAL 500000
void *rdPollObjectsThread(
  void *data
)
{
  struct rootDesktop *rd = (struct rootDesktop*)data;
  int pollCount = 0;
  for(;;)
  {
    rdPollObjects(rd);
    usleep(_POLL_OBJECTS_INTERVAL);
    pollCount++;
    if (_POLL_LIMIT!=0)
    {
      if (pollCount==_POLL_LIMIT)
      {
        break;
      }
    }
  }
}

/**  get object for a position
 */
struct rootObject *rdPositionToObject(
  struct rootDesktop *rd,
  int x, int y
)
{
  struct rootObject *object = NULL;
  struct rootObject *current = NULL;
  
  if (!rd)
    return NULL;
  
  current = rd->children;
  while(current)
  {
    if (
      x > current->frame.x &&
      x < current->frame.x+current->frame.w &&
      y > current->frame.y &&
      y < current->frame.y+current->frame.h
    )
    {
      if (activityGetVisible(current->act,0)==1)
        object = current;
    }
    current = current->next;
  }
  
  return object;
}

/**
 *  handling incomming connections on draqie.sock
 */
void *_socket_handler(
  void *data
)
{
  if (!data)
    return;
  #ifdef UNIX
    struct _trop_ipc_uds *uds = data;
    static struct _frame frame;
    int connfd;
    int nbytes;
    while((connfd = accept(uds->sockfd, (struct sockaddr*)&uds->address,
      &uds->address_length)) > -1)
    {
      #ifdef _DEBUG
        printf("_socket_handler connection\n");
      #endif
      uds->recv[0] = 0x00;
      nbytes = read(connfd,(char*)uds->recv,255);
      if (nbytes>0)
      {
        uds->recv[nbytes] = 0x00;
        #ifdef _DEBUG
          printf("_socket_handler data(%s)\n",(char*)uds->recv);
        #endif
        
        struct activity *act = activityAlloc(NULL);
        activityInitData(act,(unsigned char*)uds->recv);
        switch(act->type)
        {
          case _ACTIVITY_TYPE_EVENT:
            #ifdef _DEBUG
              printf("_socket_handler _ACTIVITY_TYPE_EVENT\n");
            #endif
            free(act->shm);
            free(act->sem);
            free(act);
            _trop_ipc_udp_parser(uds);
            write(connfd,(char*)uds->send,strlen((char*)uds->send));
            close(connfd);
            
            pollExposures(rdGlobal); // __SYNC
            
            #ifdef _DEBUG
              printf("_socket_handler pollExposures finish\n");
            #endif
            continue;
          break;
          case _ACTIVITY_TYPE_APP:
            #ifdef _DEBUG
              printf("_socket_handler _ACTIVITY_TYPE_EVENT\n");
            #endif
            if (rdGlobal)
            {
              binarySemaphoreWait(rdGlobal->sem->id);
              if (
                rdObjectNamedExists(rdGlobal,(unsigned char*)act->name)==0
              )
              {
                #ifdef _DEBUG
                  printf("_socket_handler register activity\n");
                #endif
                frame.w = _FRAME_DEFAULTW; // todo, remove, not needed
                frame.h = _FRAME_DEFAULTH;
                frame.x = (rdGlobal->frame.w/2)-(frame.w/2);
                frame.y = (rdGlobal->frame.h/2)-(frame.h/2);
      
                frame.angle = 0;
                rootObject *ro = roInit(rdGlobal,NULL,frame);
                
                // todo - make separate method
                void *shmdata = shmat(act->shm->id,0,0);
                act->shm->data = shmdata;
                struct activityAttributes attributes;
                activitySHMGetAttributes(act,&attributes);
                ro->frame.w = attributes.w;
                ro->frame.h = attributes.h;
                ro->frame.x = (rdGlobal->frame.w/2)-(ro->frame.w/2);
                ro->frame.y = (rdGlobal->frame.h/2)-(ro->frame.h/2);
                shmdt(shmdata);
                
                ro->act = act;
                ro->poll = 1;
                ro->next = NULL;
                #ifdef _DEBUG
                  printf("_socket_handler rdAddObject %p\n",ro);
                #endif
                rdAddObject(rdGlobal,ro);
              }
              else
              {
                free(act->shm);
                free(act->sem);
                free(act);
              }
              binarySemaphorePost(rdGlobal->sem->id);
            }
            else
            {
              free(act->shm);
              free(act->sem);
              free(act);
            }
          break;
        }
        
        _trop_ipc_udp_parser(uds);
        write(connfd,(char*)uds->send,strlen((char*)uds->send));
      }
      close(connfd);
    }
    close(uds->sockfd);
    unlink((char*)uds->path);
  #endif
}

void *_idle_thread_call(void*data)
{
  while(1)
  {
    if (idle_mode==2)
    {
      sleep(1);
      continue;
    }
    
    int n;
    for(n = 0; n < _IDLE; n++)
    {
      if (idle_mode==1)
      {
        idle_mode = 0;
        goto _SKIP;
      }
      sleep(1);
    }
    FILE *fd = NULL;
    unsigned char data[64];
    if ((fd = fopen("draqieSignage.sem","r")))
    {
      fread((char*)data,1,63,fd);
      int semid = atoi((char*)data);
      binarySemaphorePost(semid);
      fclose(fd);
      idle_mode = 2;
    }
_SKIP:
    continue;
  }
  return NULL;
}

#ifdef _STATIC
  #ifndef _ROOT_STRESS
    #define _ROOT_STRESS
  #endif
  int main( )
  {
    struct _frame frame;
    struct _frame smallframe;
    frame.x = 0;
    frame.y = 0;
    frame.w = _ROOT_DEFAULT_WIDTH;
    frame.h = _ROOT_DEFAULT_HEIGHT;
    smallframe.x = 0;
    smallframe.y = 0;
    smallframe.w = 320;
    smallframe.h = 320;
    
    appRegisterAll();
    
    osInit();
    activityObjectsInit();
    
    pthread_create(&idleThread,NULL,_idle_thread_call,NULL);
    
    /** scheduller
     */
    #ifdef _DEBUG
      unsigned long ts = schedullerTime();
      usleep(18000);
      ts = schedullerTime();
      printf("schedullerTime 18000/1000 ms(%d)\n",ts);
    #endif
    
    #ifdef _DRAQIE_SOCKET
      trop_IPC *ipc = trop_IPCInit(NULL);
      if (ipc)
      {
        trop_IPCUdsInit(ipc,"./draqie.sock");
        trop_IPCUdsCustomQueue(ipc,_socket_handler);
      }
      else
      {
        printf("could not create ./draqie.sock\n");
        exit(2);
      }
    #endif
    
    #ifdef _ROOT_STRESS
      struct rootStress *stress = rootStressNew();
      stress->loopTime = 10000;
      if (0) rootStressStart(stress);
    #endif
    
    scheduller = rootSchedullerNew();
    rootSchedullerRun(scheduller);
    struct rootDesktop *rd = rdInit(NULL,frame);
    
    return 0;
  }
#endif
