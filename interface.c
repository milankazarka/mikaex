/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

/** the interface provides a way of how to manage events in our
 *  applications and forward themto either the operating system
 *  dependant components or to multi-platform ones
 *
 *  NOTE: do not lock the rootController through the app interface
 */

#include "interface.h"
#include "rootController.h"
#include "activity.h"
#include "activityObject.h"
#include "mikaSerial.h"
#include "mikaEvent.h"

#include "config.h"
#include "gui.h"
#include "system.h"
#include "global.h"
#include "agg.h"
#include "access.h"
#define _IDLE 5

struct interface_t 	*bigInterface = NULL;
unsigned char 		*interfaceName = NULL;
pthread_t 		idle_t;
static int 		idle_mode = 0;

Display *dpy = NULL;

#define _FORCE_DOUBLE_BUFFER
#define _VIRTUAL_WINDOW		// no real window on desktop
#define _BACKSIDE_PIXEL 0x60
#ifdef _STRESS
  #define _CLOCKTIK	10
#else
  #define _CLOCKTIK	3000	// interval in which we poll events - this would be the
                                // response time to all events
#endif
#define _UDS_EVENTS_NO		// we either register events over UDS or activityObjects

#ifdef _CF_GUI_XLIB
  
  #define _OVERRIDE
  #define _REFRESH_ROTATION_NO
  /**
   *  this is for direct blitting of the XImage to the main Window,
   *  move transformation elsewhere
   *  gui_Flush()
   */
  int refreshGuiLow( struct platform_low_t *platform_low )
  {
    static int run = 0;
    static int rw = 0;
    static int rh = 0;
    static int xn = 0;
    static int yn = 0;
    static int _xn = 0;
    static int _yn = 0;
    static unsigned long pixel;
    static XImage *xi;
    static activityObject oExpose;
    static unsigned char eventData[256];
    static struct mikaEvent devent;
   
    if (!platform_low) return -1;
   
    #ifdef _DEBUG
      printf("__interface UEVENT EVENTFLOW refreshGuiLow\n");
    #endif
   
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("__interface refreshGuiLow dpy(%d) screen(%d) UEVENT\n",
        platform_low->dpy,
        platform_low->screen);
    #endif
   
    rw = bigInterface->width;
    rh = bigInterface->height;
   
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("__interface refreshGuiLow w(%d) h(%d) UEVENT\n",
        rw,rh);
    #endif
   
    #ifdef _REFRESH_ROTATION
      #ifdef USECAIRO
        cairo_surface_t *image = cairo_image_surface_create_for_data(
          bigInterface->surfaceData,
          CAIRO_FORMAT_ARGB32,
          rw,rh,
          4*rw
        );
        cairo_rotate(cairo,45.0*3.14/180);
        cairo_set_source_surface(cairo,image,0,0);
        cairo_paint(cairo);
        cairo_surface_destroy(image);
      #endif
    #endif
   
    #ifdef _FORCE_DOUBLE_BUFFER
      xi = platform_low->_surface;
    #else
      if (rotation==ROT_NONE)
        xi = platform_low->surface;
      else
        xi = platform_low->_surface;
    #endif
     
    if (
        rotation==ROT_LEFT ||
        rotation==ROT_RIGHT
        )
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("__interface refreshGuiLow rotation presentn\n");
      #endif
      rw = bigInterface->height;
      rh = bigInterface->width;
    }
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("__interface refreshGuiLow geom(%dx%d)\n",rw,rh);
    #endif
    
    #ifndef _VIRTUAL_WINDOW
      // responsible for the final copy of the surface to the OS window
      XPutImage(
        platform_low->dpy,
        platform_low->id,
        DefaultGC(
                  platform_low->dpy,
                  platform_low->screen
                  ),
        xi,
        0,0,0,0,
        rw,
        rh
      );
      XFlush(platform_low->dpy);
      /**
      XEvent event;
      event.xexpose.window = platform_low->id;
      event.xexpose.display = platform_low->dpy;
      event.xexpose.type = Expose;
      event.xexpose.send_event = True;
      event.xexpose.x = 0;
      event.xexpose.y = 0;
      event.xexpose.width = rw;
      event.xexpose.height = rh;
      
      XSendEvent(
        platform_low->dpy,
        platform_low->id,0,
        ExposureMask,&event
      );
      */
    #else    
      
      devent.x = 0;
      devent.y = 0;
      memcpy(&eventData,&devent,sizeof(struct mikaEvent));
      eventData[sizeof(struct mikaEvent)] = 0x00;
      while( mikaSerialAddEvent(bigInterface->serial,1,(unsigned char*)eventData) !=0 )
      { usleep(1000); }
      
    #endif
    
    if (bigInterface->delegate)
    {
      if (bigInterface->delegate->interfaceOnEvent)
        bigInterface->delegate->interfaceOnEvent(NULL);
    }
   
    return 0;
  }
  
  /**  deprecated
   */
  int mapGuiWindowLow( struct platform_low_t *platform_low )
  {
    if (!platform_low) return -1;
    
    #ifndef _VIRTUAL_WINDOW
      XMapWindow(platform_low->dpy,platform_low->id);
      XFlush(platform_low->dpy);
    #endif
    refreshGuiLow(platform_low);
    
    return 0;
  }
  
  int getHWWindowIdLow( struct platform_low_t *platform_low, long *id )
  {
    if (!platform_low||!id) return -1;
    *id = platform_low->id;
    return 0;
  }
  
  int initGuiLow( struct platform_low_t *platform_low )
  {
    if (!platform_low) return -1;
    
    int rw;
    int rh;
    
    #ifdef _XTHREADS
      XInitThreads(); // multi threaded access support
    #endif
    platform_low->dpy = XOpenDisplay(NULL);
    platform_low->screen = DefaultScreen(platform_low->dpy);
    Window root;
    unsigned long valuemask;
    XWindowAttributes attr1;
    XSetWindowAttributes attr;
    attr.event_mask = ExposureMask | ButtonPressMask | ButtonReleaseMask |
      ButtonMotionMask | EnterWindowMask | LeaveWindowMask;
    attr.backing_store = Always;
    root = RootWindow(platform_low->dpy,platform_low->screen);
    XGetWindowAttributes(platform_low->dpy,root,&attr1);
    valuemask = CWEventMask;
    #ifdef _OVERRIDE
    //  valuemask += CWOverrideRedirect;
    //  attr.override_redirect = True;
    #endif
  
    if (
        rotation==ROT_LEFT ||
        rotation==ROT_RIGHT
        )
    {
      rh = bigInterface->width;
      rw = bigInterface->height;
    }
    else
    {
      rw = bigInterface->width;
      rh = bigInterface->height;
    }
    
    #ifdef _DEBUG
      printf("XCreateWindow pre dpy(%d) screen(%d) geom(%dx%d)\n",
        platform_low->dpy,platform_low->screen,rw,rh);
    #endif
    
    #ifndef _VIRTUAL_WINDOW
      platform_low->id = XCreateWindow(
                                    platform_low->dpy,root,
                                    bigInterface->x,
                                    bigInterface->y,
                                    rw,
                                    rh,
                                    1, DefaultDepth(platform_low->dpy,platform_low->screen),
                                    CopyFromParent, 
                                    DefaultVisual(platform_low->dpy,platform_low->screen),
                                    valuemask, &attr
                                  );
      #ifdef _DEBUG
        printf("XCreateWindow after\n");
      #endif
      if (platform_low->id < 0)
      {
        fprintf(_stdout,":Could not create Xlib Window\n");
      }
      else
      {
        #ifdef _DEBUG
          printf(":Xlib wid %p\n",platform_low->id);
        #endif
      }
      XSetWindowBackground(platform_low->dpy,platform_low->id,
          BlackPixel(platform_low->dpy,platform_low->screen));
      XSelectInput(platform_low->dpy,platform_low->id,
          ExposureMask|ButtonPressMask|ButtonReleaseMask|ButtonMotionMask);
      XMapWindow(platform_low->dpy,platform_low->id);
    #endif
    
    int 		        count = 0;
    XPixmapFormatValues		*formats = NULL;
    XPixmapFormatValues		*format = NULL;
    
    formats = XListPixmapFormats(platform_low->dpy,&count);
    
    for(format = formats; count > 0; count--, format++)
    {
      if (format->depth == DefaultDepth(platform_low->dpy,platform_low->screen))
      {
      
        #ifdef _DEBUG
          printf("__interface platform_low->surface creating (%dx%d)\n",rw,rh);
        #endif
        platform_low->surface = XCreateImage(
                                platform_low->dpy,
                                DefaultVisual(platform_low->dpy,platform_low->screen),
                                DefaultDepth(platform_low->dpy,platform_low->screen),
                                ZPixmap,
                                0,
                                bigInterface->surfaceData,
                                rw,
                                rh,
                                format->scanline_pad,
                                0
                              );
        
        if (
          rotation==ROT_LEFT ||
          rotation==ROT_RIGHT
          )
        {
          rw = bigInterface->height;
          rh = bigInterface->width;
        }
        
        platform_low->_surface = XCreateImage(
                                platform_low->dpy,
                                DefaultVisual(platform_low->dpy,platform_low->screen),
                                DefaultDepth(platform_low->dpy,platform_low->screen),
                                ZPixmap,
                                0,
                                bigInterface->_surfaceData,
                                rw,
                                rh,
                                format->scanline_pad,
                                0
                              );
        
        if (
          !platform_low->surface ||
          !platform_low->_surface
          )
        {
          printf("could not create surface\n");
          exit(2);
        }
      }
    }
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("__interface platform_low surface created\n");
    #endif
    
    return 0;
  }
  
  /**  commit surface to desktop
   */
  int guiFlushLow( struct platform_low_t *platform_low )
  { 
    if (!platform_low) return -1;
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("__interface guiFlushLow EVENTFLOW\n");
    #endif
    
    #ifndef _VIRTUAL_WINDOW
      #ifdef _XFLUSH
        XFlush(platform_low->dpy);
      #endif
    #endif
    refreshGuiLow(platform_low);
    
    return 0;
  }
  
  /**  get next event
   */
  int nextGuiEventLow( struct platform_low_t *platform_low, struct interface_event_t *event )
  {
    static XEvent xe;
    static int run = 0;
    static struct activityEvent activityEvent;
    
    if (!platform_low||!event) return -1;
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("nextGuiEventLow\n");
    #endif
    
    if (run==0)
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("nextGuiEventLow run0\n");
      #endif
      xe.type = Expose;
      xe.xexpose.window = platform_low->id;
      xe.xexpose.x = 0;
      xe.xexpose.y = 0;
      xe.xexpose.width = 640;
      xe.xexpose.height = 480;
    }
    else
    {
      if (bigInterface->newEvent.type!=INTERFACE_EVENT_NONE)
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("nextGuiEventLow push event\n");
        #endif
        gui_WidgetPushEvent(&bigInterface->newEvent);
      }
      else
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("nextGuiEventLow XNextEvent platform_low(%p)\n",
                                  platform_low);
        #endif
        
        #ifdef _VIRTUAL_WINDOW
          while(1)
          { 
            binarySemaphoreWait(bigInterface->activity.sem->id);
            
            if ( activityEventActive(&bigInterface->activity)==1)
            {
              #ifdef _DEBUG
                printf("nextGuiEventLow activityEventActive = 1\n");
              #endif
              //  events ignored by application, which are intended for
              //  the rootController
              if (
                activityEventIsEvent(&bigInterface->activity,_ACTIVITY_EVENT_EXPOSE)==1
                )
              {
                binarySemaphorePost(bigInterface->activity.sem->id);
                return 0;
              }
             
              activityEventGet(&bigInterface->activity,&activityEvent);
              
              switch(activityEvent.type)
              {
                case _ACTIVITY_EVENT_BUTTONDOWN:
                  #ifdef _DEBUG
                    if (DEBUG==TRUE) printf("nextGuiEventLow _ACTIVITY_EVENT_BUTTONDOWN\n");
                  #endif
                  xe.type = ButtonPress;
                  xe.xbutton.x = activityEvent.button.x;
                  xe.xbutton.y = activityEvent.button.y;
                  // if not set, then we send an X event
                  if (bigInterface->onButtonDown)
                    bigInterface->onButtonDown(xe.xbutton.x,xe.xbutton.y);
                    
                  break;
                case _ACTIVITY_EVENT_BUTTONUP:
                  #ifdef _DEBUG
                    if (DEBUG==TRUE) printf("nextGuiEventLow _ACTIVITY_EVENT_BUTTONUP\n");
                  #endif
                  xe.type = ButtonRelease;
                  xe.xbutton.x = activityEvent.button.x;
                  xe.xbutton.y = activityEvent.button.y;
                  if (bigInterface->onButtonUp)
                    bigInterface->onButtonUp(xe.xbutton.x,xe.xbutton.y);
                    
                  break;
                case _ACTIVITY_EVENT_REPAINT:
                  #ifdef _DEBUG
                    if (DEBUG==TRUE) printf("nextGuiEventLow _ACTIVITY_EVENT_REPAINT\n");
                  #endif
                  xe.type = Expose;
                  break;
              }
              
              #ifdef _DEBUG
                if (DEBUG==TRUE) printf("nextGuiEventLow unknown event type\n");
              #endif
              binarySemaphorePost(bigInterface->activity.sem->id);
              break;
            }
          
            binarySemaphorePost(bigInterface->activity.sem->id);
            usleep(_CLOCKTIK);
          }
        #else  
          XNextEvent(platform_low->dpy,&xe);
        #endif
      }
    }
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("nextGuiEventLow got event %d\n",xe.type);
    #endif
    
    switch(xe.type)
    {
      case ButtonPress:
        event->type = INTERFACE_EVENT_BDOWN;
        event->x = xe.xbutton.x;
        event->y = xe.xbutton.y;
        break;
      case LeaveNotify:
        event->type = INTERFACE_EVENT_LEAVE;
        event->x = xe.xcrossing.x;
        event->y = xe.xcrossing.y;
        break;
      case ButtonRelease:
        event->type = INTERFACE_EVENT_BUP;
        event->x = xe.xbutton.x;
        event->y = xe.xbutton.y;
        break;
      case MotionNotify:
        event->type = INTERFACE_EVENT_MOTION;
        event->x = xe.xmotion.x;
        event->y = xe.xmotion.y;
        break;
      case Expose:
        event->type = INTERFACE_EVENT_EXPOSE;
        event->x = xe.xexpose.x;
        event->y = xe.xexpose.y;
        event->width = xe.xexpose.width;
        event->height = xe.xexpose.height;
        break;
    }
    
    run++;
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("nextGuiEventLow finish\n");
    #endif
    
    return 0;
  }
  
  int addGuiEventLow( struct platform_low_t *platform, struct interface_event_t *event )
  {
    if (!platform||!event) return -1;
    return 0;
  }
  
  /**  in top level called by gui_TriggerExpose
   *
   *   generates an internal repaint event
   */
  #define _SLOPPY_EXPOSURE_NO
  #define _ASYNC_EXPOSURE_TRIGGER
  #define _TRIGGER_EXPOSE_DEBUG
  #define _TRIGGER_EXPOSE_CHECKS_NO
  int triggerExposeLow( struct platform_low_t *platform_low )
  {
    struct activityObject oExpose;
    static long long run = 0;
    unsigned char eventData[256];
    struct mikaEvent devent;
    
    if (!platform_low) return -1;
    
    #ifdef _TRIGGER_EXPOSE_DEBUG
      #ifdef _DEBUG
        printf("triggerExposeLow UEVENT %d EVENTFLOW\n",run);
      #endif
    #endif
        
        if (!dpy)
          dpy = XOpenDisplay(NULL);
        
        while(XPending(dpy)>20)
          usleep(100000);
        
        binarySemaphoreWait(bigInterface->activity.sem->id);
        activityAddEventRepaint(&bigInterface->activity);
        devent.x = 0;
        devent.y = 0;
        memcpy(&eventData,&devent,sizeof(struct mikaEvent));
        eventData[sizeof(struct mikaEvent)] = 0x00;
        binarySemaphorePost(bigInterface->activity.sem->id);
       
        #ifdef _VIRTUAL_WINDOW
          while( mikaSerialAddEvent(bigInterface->serial,1,(unsigned char*)eventData)!=0 )
          { usleep(1000); }
        #else
        //guiFlushLow(platform_low);
          
        XEvent event;
        event.xexpose.window = platform_low->id;
        event.xexpose.display = platform_low->dpy;
        event.xexpose.type = Expose;
        event.xexpose.send_event = True;
        event.xexpose.x = 0;
        event.xexpose.y = 0;
        event.xexpose.width = 100;
        event.xexpose.height = 100;
      
        XSendEvent(
          platform_low->dpy,
          platform_low->id,0,
          ExposureMask,&event
        );
      
        #endif
        
        #ifdef _TRIGGER_EXPOSE_DEBUG
          #ifdef _DEBUG
            if (DEBUG==TRUE) printf("triggerExposeLow poll cycle UEVENT %d\n",run);
          #endif
        #endif
        
      #ifdef _TRIGGER_EXPOSE_DEBUG
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf("triggerExposeLow poll finish UEVENT %d\n",run);
        #endif
      #endif
       
    run++;
    return 0;
  }
  
#endif

/**
 *  commit the surface of the app to the root window
 *
 *  this is a very simple dirty way of doing things
 */
#define _DIRECT_BLIT_NO
int platformRefreshCommitToRoot(
  struct platform_t *platform
)
{
  return -1; // deprecated, remove
}

/**  commit the main surface to the shm, so that it may be fetched
 *   by the rootController
 */
#define _PLATFORM_REFRESH_COMMIT_TO_SHM_DUMMY_NO
int platformRefreshCommitToSHM(
  struct platform_t *platform
)
{
  static unsigned int   rw;
  static unsigned int   rh;
  static unsigned int	backplanew;
  static unsigned int	backplaneh;
  static unsigned int	nx;
  static unsigned int	ny;
  static unsigned char	pixel[4];
  static unsigned char  blank[1] = {0x00};
  static int	 	npixel;
  void 			*shmpbuffer = NULL;
  void 			*surface = NULL;

  if (!platform)
    return -1;
  
  rw = bigInterface->width;
  rh = bigInterface->height;
  
  #ifdef _DEBUG
    printf("platformRefreshCommitToSHM UEVENT size(%dx%d)\n",
      rw,rh);
  #endif
  
  rw = bigInterface->width;
  rh = bigInterface->height;
  
  #ifdef _DEBUG
    if (DEBUG==TRUE) printf("platformRefreshCommitToSHM UEVENT\n");
  #endif
  
  binarySemaphoreWait(bigInterface->activity.sem->id);
  shmpbuffer = shmat(bigInterface->activity.shm->id,0,0);
  bigInterface->activity.shm->data = shmpbuffer;
  surface = activitySHMGetSurface(&bigInterface->activity);
  if (!surface)
  {
    #ifdef _DEBUG
        printf("platformRefreshCommitToSHM error at accesing surface(%dx%d)\n",
          rw,rh);
    #endif
    // exit strategy
  }
  #ifndef _PLATFORM_REFRESH_COMMIT_TO_SHM_DUMMY
    
    #ifdef _INTERFACE_BACKPLANE
      #ifdef _DEBUG
        printf("platformRefreshCommitToSHM copy backplane\n");
      #endif
      
      bigInterface->backplane->pixels = shmat(bigInterface->backplane->shmid,0,0);
      
      // first we copy the standard surface and then the 'backplane' - this basically
      // sounds like the backplane is actually an 'overlay' - need to remake
      //
      npixel=0;
      for(ny = 0; ny < rh; ny++)
      {
        for(nx = 0; nx < rw; nx++)
        {
          npixel++;
              
          memcpy(
            surface+(npixel*4),
            bigInterface->surfaceData+(npixel*4),4);
      
        }
      }
      
      backplanew = bigInterface->backplane->w;
      backplaneh = bigInterface->backplane->h;
      #ifdef _DEBUG
        printf("platformRefreshCommitToSHM backplane (%dx%d)\n",backplanew,backplaneh);
      #endif
      npixel=0;
      for(ny = 0; ny < backplaneh; ny++)
      {
        for(nx = 0; nx < backplanew; nx++)
        {
          npixel++;
          
          memcpy(
            (void*)pixel,
            bigInterface->backplane->pixels+
            (ny*rw*4)+(nx*4),
            4
          );
          memcpy(
            surface+(npixel*4),
            (void*)pixel,4);
        }
      }
      shmdt(bigInterface->backplane->pixels);
    
    #else
      memcpy(
        surface,bigInterface->surfaceData,
        rw*rh*4
        );
    #endif
  #else
    memset(surface,0x00,rw*rh*4);
  #endif
  shmdt(shmpbuffer);  
  binarySemaphorePost(bigInterface->activity.sem->id);
  #ifdef _DEBUG
    if (DEBUG==TRUE) printf("platformRefreshCommitToSHM finish\n");
  #endif
  return 0;
}

#ifdef _VIRTUAL_WINDOW
  #define _VIRTUAL_REFRESH
#endif
void *platformRefresh( struct platform_t *platform )
{
  if (!platform) return NULL;
  
  #ifdef _DEBUG
    if (DEBUG==TRUE) printf("__interface platformRefresh UEVENT\n");
  #endif
  
  /**
   *  rotation transformation
   */
  static unsigned int	xn;
  static unsigned int   yn;
  static unsigned long  pixel;
  static struct gui_GC  *neutral = NULL;
  static unsigned int   rx;
  static unsigned int   ry;
  static unsigned int   rw;
  static unsigned int   rh;
  static unsigned int	torw;
  static unsigned int	torh;
  static unsigned int	boundw;
  static unsigned int	boundh;
  
  rx = 0;
  ry = 0;
    
  rw = bigInterface->width;
  rh = bigInterface->height;
  
  if (rotation == ROT_LEFT || rotation == ROT_RIGHT)
  {
    torw = rh;
    torh = rw;
  }
  else
  {
    torw = rw;
    torh = rh;
  }
  
  boundw = gui_bounds.width;
  boundh = gui_bounds.height;
  
  gui_bounds.width = torw;
  gui_bounds.height = torh;
  
  #ifdef _DIRECT_BLIT
    platformRefreshCommitToRoot(platform);
  #else
    platformRefreshCommitToSHM(platform);
  #endif
   
  #ifndef _VIRTUAL_REFRESH
  
    #ifndef _FORCE_DOUBLE_BUFFER
      if (rotation == ROT_LEFT || rotation == ROT_RIGHT || rotation == ROT_DOWN)
    #endif
    {
      for (yn = 0; yn < bigInterface->height; yn++)
      {
        for (xn = 0; xn < bigInterface->width; xn++)
        {
            __get_pixel(
              bigInterface->surfaceData,
              rw,
              rh,
              xn,yn,
              bigInterface->bpp/8,
              (char*)&pixel
            );
          
          // we don't have a need to do rotation in the interface
          // if we use the rootController
          if (rotation == ROT_NONE)
          {
            rx = xn;
            ry = yn;
          }
          else if (rotation == ROT_LEFT)
          {
            rx = bigInterface->height-yn;
            ry = xn;
          }
          else if (rotation == ROT_RIGHT)
          {
            rx = yn;
            ry = bigInterface->width-xn;
          }
          else if (rotation == ROT_DOWN)
          {
            rx = bigInterface->width-xn;
            ry = bigInterface->height-yn;
          }
          
          __put_pixel(
                bigInterface->_surfaceData,
                torw,
                torh,
                rx,ry,
                bigInterface->bpp/8,
                (char*)&pixel);
        }
      }
    }
    #ifndef _FORCE_DOUBLE_BUFFER
      else
      {
      }
    #endif
  
    gui_bounds.width = boundw;
    gui_bounds.height = boundh;
  
    refreshGuiLow(&platform->low);
  #endif
  
  return NULL;
}

int _platformInit( struct platform_t *platform )
{
  if (!platform) return -1;
  platform->refresh = platformRefresh;
  return 0;
}

/**
 *  belonging to the interface
 */

void *stdCreateSurface( struct interface_t *interface )
{
  int rw;
  int rh;
  int datasize;
  
  if (!interface) return NULL;
  
  interface->scanline =  (interface->bpp/8)*interface->width;
  datasize = interface->scanline*interface->height;
  
  #ifdef _DEBUG
    if (DEBUG==TRUE) printf("__interface creating pixel buffer size(%d)\n",
      interface->scanline*interface->height);
  #endif
  interface->surfaceData = malloc(
                                  datasize
                                  );
  
  /**
   *  set the memmory location of the SHM pixel buffer
   */                                
  binarySemaphoreWait(interface->activity.sem->id);
  void *shmdata = shmat(interface->activity.shm->id,0,0);
  interface->_surfaceData = shmdata;
  shmdt(shmdata);
  binarySemaphorePost(interface->activity.sem->id);
  
  /**
   *  todo, exchange old gui surface functions
  */
  interface->surface = gui_SurfaceInitEx(
                                         NULL, // todo, window
                                         NULL,
                                         interface->width,
                                         interface->height
                                         );
  
  initGuiLow(&interface->platform.low);
  
  return NULL;
}

void *stdMapMainWindow( struct interface_t *interface )
{
  if (!interface) return NULL;
  
  mapGuiWindowLow(&interface->platform.low);
  
  return NULL;
}

void *stdGetHWWindowId( struct interface_t *interface, long *id )
{
  if (!interface||!id) return NULL;
  
  getHWWindowIdLow(&interface->platform.low,id);
  
  return NULL;
}

void *stdGuiFlush( struct interface_t *interface )
{
  if (!interface) return NULL;
  
  guiFlushLow(&interface->platform.low);
  
  return NULL;
}

void *stdNextGuiEvent( struct interface_t *interface, struct interface_event_t *event )
{
  if (!interface||!event) return NULL;
  
  nextGuiEventLow(&interface->platform.low,event);
  
  return NULL;
}

void *stdAddGuiEvent( struct interface_t *interface, struct interface_event_t *event )
{
  if (!interface||!event) return NULL;
  
  addGuiEventLow(&interface->platform.low,event);
  
  return NULL;
}

void *stdTriggerExpose( struct interface_t *interface )
{
  if (!interface) return NULL;
  
  #ifdef _DEBUG
    if (DEBUG==TRUE) printf("stdTriggerExpose\n");
  #endif
  
  triggerExposeLow(&interface->platform.low);
  
  return NULL;
}

/**
 *  this enables us to connect to an external pixel buffer
 *
 */
int initController( struct interface_t *interface )
{
  FILE *fd = NULL;
  char line[256];
  char tmp[32];
  
  unsigned char *filename = "activity/CTRL_ROOT";
  
  if ((fd = fopen(filename,"r"))==NULL)
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("initController could not open file (%s)\n",
        _SHM_ID_FILENAME);
    #endif
    return -1;
  }

  while(1)
  {
    fgets((char*)line,256,fd);
    if (feof(fd))
      break;
    if (line[strlen((char*)line)-1]=='\n')
    {
      line[strlen((char*)line)-1] = 0x00;
    }
    
    if (strlen((char*)line)>6)
    {
      if (strncmp((char*)line,"SHMID",5)==0)
      {
        strncpy((char*)tmp,(char*)line+6,strlen((char*)line)-6);
        tmp[strlen((char*)line)-6] = 0x00;
        interface->controller.SHMID = atoi((char*)tmp);
      }
      
      if (strncmp((char*)line,"SEMID",5)==0)
      {
        strncpy((char*)tmp,(char*)line+6,strlen((char*)line)-6);
        tmp[strlen((char*)line)-6] = 0x00;
        interface->controller.SEMID = atoi((char*)tmp);
      }
    }
  }  

  fclose(fd);
  
  return 0;
}

/**
 *  initialize the 'activity' of this application and set it's name
 */
#define _ACTIVITY_COMPLEX
int initActivity( struct interface_t *interface, unsigned char *name )
{
  if (!interface || !name)
    return -1;
  
  sprintf((char*)interface->activity.name,"%s",name);  
  #ifdef _DEBUG
    if (DEBUG==TRUE) printf("initActivity (%dx%d)\n",
      interface->width,interface->height);
  #endif
  
  #ifdef _ACTIVITY_COMPLEX
    interface->activity.type = _ACTIVITY_TYPE_APP;
    activityAlloc(&interface->activity);
    os_rsemInit(interface->activity.sem);
    os_rshmInit(interface->activity.shm);
    
    if (activitySHMCreate(
      &interface->activity,
      interface->width*interface->height*4
      )!=0)
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("initActivity could not allocate shm\n");
      #endif
    }
    interface->activity.sem = os_rsemInit(interface->activity.sem);
  
    void *shm = shmat(interface->activity.shm->id,0,0);
    struct activityAttributes attributes;
    attributes.x = interface->x;
    attributes.y = interface->y;
    attributes.w = interface->width;
    attributes.h = interface->height;
    attributes.movable 		= 1;
    attributes.visible 		= 1;
    attributes.toplevel 	= 0;
    attributes.angle		= 0;
    attributes.forceangle	= -1;
    activitySHMSetAttributes(&interface->activity,&attributes);
    shmdt(shm);
  #else
    interface->activity.sem = os_rsemInit(NULL);
    interface->activity.shm = os_rshmInit(NULL);
    os_rshmAlloc(
      interface->activity.shm,
      interface->width*interface->height*4,
      _ACTIVITY_TYPE_APP,
      name
    );
  #endif
  
  unsigned char filename[128];
  sprintf((char*)filename,"activity/APP_%s",name);
  
  unsigned char *myName = NULL;
  if (interfaceName!=NULL)
    myName = interfaceName;
  else
    myName = "MENU";
  
  strcpy((char*)interface->activity.name,myName);
  activityRegisterOverSocket(&interface->activity);
  
  return 0;
}

int deinitActivity( struct interface_t *interface )
{
  return 0;
}

/** create the backplane of the interface
 */
struct interface_backplane_t *interfaceBackplaneInit( struct interface_t *interface )
{
  #ifdef _DEBUG
    printf("interfaceBackplaneInit\n");
  #endif
  
  int nx;
  int ny;
  unsigned char line[64];
  struct interface_backplane_t *backplane;
  backplane = (struct interface_backplane_t*)malloc(sizeof(struct interface_backplane_t));
  
  int nw = _ROOT_DEFAULT_WIDTH;
  int nh = _ROOT_DEFAULT_HEIGHT;
  
  int size = nw*nh*4;
  
  backplane->shmid = shmget( 
    IPC_PRIVATE, size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR | SHM_R | SHM_W
    );
  
  backplane->w = nw;
  backplane->h = nh;  
  backplane->pixels = shmat(backplane->shmid,0,0);
  for(ny = 0; ny < nh; ny++)
  {
    for(nx = 0; nx < nw; nx++)
    {
      memset(backplane->pixels+(nw*4*ny)+(nx*4),_BACKSIDE_PIXEL,3);
      memset(backplane->pixels+(nw*4*ny)+(nx*4)+3,0xff,1);
    }
  }
  shmdt(backplane->pixels);
  backplane->pixels = NULL;
  
  return backplane;
}

/** set the size of the backplane
 */
int interfaceSetBackplaneSize( int w, int h )
{
  #ifdef _DEBUG
    printf("interfaceSetBackplaneSize (%dx%d)\n",w,h);
  #endif
  bigInterface->backplane->w = w;
  bigInterface->backplane->h = h;
  return 0;
}

/**
 *  to create an interface:
 *    struct interface_t *interface = interfaceInit(NULL);
 *    - setup the values of the interface -
 *    interfaceStart(interface);
 */

struct interface_t *interfaceInit( struct interface_t *interface )
{
  static int run = 0;
  
  if (run>0)
    return NULL;
  
  run++;
  
  if (interface==NULL)
    interface = (struct interface_t*)malloc(sizeof(struct interface_t));
  
  if (!interface) return NULL;
  
  interface->delegate		= NULL;
  
  interface->serial		= mikaSerialClone();
  interface->backplane		= interfaceBackplaneInit(interface);
  interface->surface            = NULL;
  interface->surfaceData 	= NULL;   // created by createSurface
  interface->_surfaceData        = NULL;  //  - rotation middle step
  interface->x                  = 0;      // set by you
  interface->y                  = 0;      // set by you
  interface->width 		= 0;      // set by you
  interface->height 		= 0;      // set by you
  interface->bpp 		= 0;      // set by you
  
  interface->scanline		= 0;      // calculated later
  interface->createSurface	= stdCreateSurface;
  interface->mapMainWindow      = stdMapMainWindow;
  interface->getHWWindowId      = stdGetHWWindowId;
  interface->guiFlush		= stdGuiFlush;
  interface->nextGuiEvent       = stdNextGuiEvent;
  interface->addGuiEvent        = stdAddGuiEvent;
  interface->triggerExpose	= stdTriggerExpose;
  
  bigInterface                  = interface;

  bigInterface->newEvent.type 	= INTERFACE_EVENT_NONE;
  bigInterface->newEvent.x 	= 0;
  bigInterface->newEvent.y 	= 0;
  bigInterface->newEvent.width 	= 0;
  bigInterface->newEvent.height = 0;

  bigInterface->onButtonDown 	= NULL;
  bigInterface->onButtonUp 	= NULL;

  return interface;
}

/** set the name of this application
 */
int interfaceSetName( unsigned char *name )
{
  if (!name)
    return 1;
  
  interfaceName = (unsigned char*)malloc(strlen((char*)name)+1);
  strcpy((char*)interfaceName,name);  
    
  return 0;
}

void interfaceTerminationHandler( )
{
  #ifdef _DEBUG
    printf("\n\n\tinterfaceTerminationHandler\n\n\n");
  #endif
  binarySemaphorePost(bigInterface->controller.SEMID);
  exit(0);
}

/** deinitialize interface resources
 */
void interfaceDeinit( )
{
  #ifdef _DEBUG
    printf("intrefaceDeinit\n");
  #endif
}

/** record the backplane access - call from APP
 *  	line 1. root surface semaphore
 *  	line 2. backplane shm id
 */
int interfaceRecordBackplaneAccess( unsigned char *name )
{
  if (!name)
  {
    #ifdef _DEBUG
      printf("interfaceRecordBackplaneAccess error, invlid arguments\n");
    #endif
    return 1;
  }
  
  if (bigInterface->activity.sem->id<0)
  {
    #ifdef _DEBUG
      printf("interfaceRecordBackplaneAccess error no activity semaphore\n");
    #endif
    interfaceDeinit();
    exit(2);
  }

  #ifdef _DEBUG
    printf("interfaceRecordBackplaneAccess name(%s)\n", (char*)name);
  #endif

  unsigned char filename[256];
  unsigned char line[256];
  sprintf((char*)filename,"mikaAccess%s.log",(char*)name);
  FILE *fdaccess = fopen((char*)filename ,"w");
  if (fdaccess)
  { 
    sprintf((char*)line,"%d\n",bigInterface->activity.sem->id);
    fwrite((char*)line,strlen((char*)line),1,fdaccess);
    sprintf((char*)line,"%d\n",bigInterface->backplane->shmid);
    fwrite((char*)line,strlen((char*)line),1,fdaccess);
    fclose(fdaccess);
  }
  return 0;
}

int interfaceStart( struct interface_t *interface )
{
  if (!interface) return -1;
  
  #ifdef _DEBUG
    if (DEBUG==TRUE) printf("interfaceStart\n");
  #endif
  
  interface->activity.shm = NULL;
  interface->activity.sem = NULL;
  
  _platformInit(&interface->platform);
  
  if ( initController(interface) != 0 )
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("interfaceStart error at initController");
    #endif
  }
  
  unsigned char *myName = NULL;
  if (interfaceName!=NULL)
    myName = interfaceName;
  else
    myName = "GENERAL";
  
  if ( initActivity(interface,myName) != 0 )
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("interfaceStart error at initActivity");
    #endif
  }
  interface->createSurface(interface);
  
  gl = gui_GlInit(
                  NULL,interface->surfaceData,
                  interface->width,
                  interface->height
                  );
  
  signal(SIGHUP,interfaceTerminationHandler);
  signal(SIGINT,interfaceTerminationHandler);
  signal(SIGTERM,interfaceTerminationHandler);
  
  #ifdef _DEBUG
    if (DEBUG==TRUE) printf("interfaceStart end\n");
  #endif
  
  return 0;
}
