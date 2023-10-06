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
#include "rootWmInterface.h"
#include "rootEvents.h"
#include "rootStress.h"
#include "rootModules.h"
#include "rootPointer.h"
#include "rootActivity.h"
#include "rootDraw.h"
#include "rootSetupC.h"
#include "app.h"
#include "system.h"
#include "activityObject.h"
#include "mikaSerial.h"
#include "mikaSkin.h"
#include "rootControllerWatchdog.h"
#ifdef USECAIRO
  #include <cairo/cairo.h>
  #include <cairo/cairo-xlib.h>
#endif

#ifndef _AI
  #include "ai.h"
#endif
#ifdef _STRESS
  #define _CLOCKTIK		10
  #define _CLOCKTIK_SLOW	40
#else
  #define _CLOCKTIK 		2000	// event loop clocktik
  #define _CLOCKTIK_SLOW 	10000	// event loop clocktik in idle mode
#endif
#define _STRESSTEST
#define _ANGLE_TEST_NO
#define _DUMMY_NO
#define _MIKA_SOCKET
#define _IDLE 			20
#define _TRANSPARENCY
#define _GLOW_NO 			// arround each app's window
#define _SHADOW_NO			// shadow around each app

/**  we need to run 3 threads - one platform thread for 
 *   platform speciffic events, then the thread for receival
 *   of application messages and then the main thread
 *   processing the events from the later two
 *
 *   we need to make this more transparent
 */
static struct eventPlatform 	ePlatform;
static struct eventApi 		eApi;
static struct rootActivity 	rootAct;
struct rootDesktop 		*rdGlobal = NULL;
struct rootScheduller		*scheduller = NULL;
static int 			currentx;
static int 			currenty;
static int 			idle_mode = 0;
static struct mikaSerial	*serial = NULL;
static int			_trans = 1;

struct rootDesktop *mainRd = NULL;

void rootControllerLock( int id )
{
  static long long count = 0;
  #ifdef _DEBUG
    printf("rootControllerLock id(%d) count(%d)\n",id,count);
  #endif
  binarySemaphoreWait(rdGlobal->sem->id);
  count++;
}

void rootControllerUnlock( int id )
{
  static long long count = 0;
  #ifdef _DEBUG
    printf("rootControllerLock id(%d) count(%d) finish\n",id,count);
  #endif
  binarySemaphorePost(rdGlobal->sem->id);
  count++;
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
  
  static long count = 0;
  
  #ifdef _DEBUG
    printf("rdPushObject count %d\n",count);
  #endif
  
  struct rootObject *current = rd->children;
  struct rootObject *last = NULL;
  while(current) // loop
  {
    #ifdef _DEBUG
      printf("rdPushObject loop\n");
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
  
  ro->next = rd->children;
  rd->children = ro;
  
  count++;
  
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
  
  static long count = 0;
  
  #ifdef _DEBUG
    printf("rdAddObject count %d OBJECT\n",count);
  #endif
  
  struct rootObject *current = rd->children;
  struct rootObject *last = NULL;
  
  while(current) // loop
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
    while(current) // loop
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
  
  count++;
  
  return 0;
}

int rdRemoveObject(
  struct rootDesktop *rd,
  struct rootObject *ro
)
{
  static long count = 0;

  if (!rd || !ro)
    return -1;
  
  #ifdef _DEBUG
    printf("rdRemoveObject %p count %d OBJECT\n",ro,count);
  #endif
  
  struct rootObject *last = NULL;
  struct rootObject *current = rd->children;
  while(current) // loop
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
  
  count++;
  
  return 0;
}

/** apply transparency to the surface of the object
 */
int rdObjectTransparency( void *surface, int w, int h, unsigned char trans )
{
  #ifndef _TRANSPARENCY
    return 0;
  #endif
  
  if (_trans==0)
    trans=0xff;
  
  if (!surface)
    return 1;
  
  static int n;
  static int run;
  static unsigned char data[1];
  static unsigned char clear[1];
  data[0] = trans;
  clear[0] = 0x00;
  run = 0;
  for(n = 0; n < w*h*4; n++)
  {
    if (run==3)
    {
      if (memcmp((void*)surface+n,(void*)clear,1)!=0)
        memcpy((void*)surface+n,(void*)data,1);
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
  static int nx, ny;
  static unsigned char pixel[5];
  if (pixel[4]!=0x00)
    pixel[4] = 0x00;

  if (!rd || !ro)
    return -1;

  #ifdef _DEBUG
    printf("rdDRawObjectSurfaceESYNC UEVENT\n");
  #endif
  
  binarySemaphoreWait(ro->act->sem->id);
  ro->act->shm->data = (unsigned char*)shmat(ro->act->shm->id,0,0);
  if (!ro->act->shm->data)
  {
    #ifdef _DEBUG
      printf("rdDrawObjectSurfaceESYNC no surface returned, remove object\n");
    #endif
    binarySemaphorePost(ro->act->sem->id);
    return -1;
  }
  
  #ifdef _SHADOW
    cairo_set_source_rgba(cairo, 0.0, 0.0, 0.0, 0.5);
    cairo_rectangle(cairo, 
                      ro->frame.x-_BORDER, 
                      ro->frame.y-_BORDER, 
                      ro->frame.w+(_BORDER*2), 
                      ro->frame.h+(_BORDER*2)
                      );
    cairo_close_path(cairo);
    cairo_fill(cairo);*/
  #endif
  
  activitySHMGetAttributes(ro->act,&attributes);
  
  if (attributes.visible==0) // desktop icon
  {
    #ifdef _GLOW
      cairo_set_line_width(cairo,3);
      static struct rootFrame frame;
      frame.x = ro->frame.x-_BORDER;
      frame.y = ro->frame.y-_BORDER;
      frame.w = frame.h = 100+(_BORDER*2);
      rootDrawDrawRoundRectGlow(cairo,&frame,20);
      cairo_set_line_width(cairo,1);
    #endif
    cairo_set_source_rgba(cairo,1.0,1.0,1.0,1.0);
    cairo_rectangle (cairo, 
      ro->frame.x,ro->frame.y, 
      100,100);
    cairo_fill (cairo);
    
  }
  else if (attributes.visible==1)
  {
    #ifdef _GLOW
      cairo_set_line_width(cairo,3);
      static struct rootFrame frame;
      frame.x = ro->frame.x;
      frame.y = ro->frame.y;
      frame.w = ro->frame.w;
      frame.h = ro->frame.h;
      rootDrawDrawRoundRectGlow(cairo,&frame,20);
      cairo_set_line_width(cairo,1);
    #endif
  
    surface = activitySHMGetSurface(ro->act);
    if (surface)
    { 
      #ifndef _GENERAL_TRANSPARENCY
        if (ro->next)
      #endif
      // only applying transparency on the top window dramatically speeds up
      // the drawing
      //
      //if (_trans==1)
        if (_TRANSPARENCY_VALUE!=0xff)
        {
          #ifdef _TOPLEVEL_PRIORITY
            if (activityGetToplevel(ro->act,0)==1)
          #endif
              rdObjectTransparency(surface,ro->frame.w,ro->frame.h,_TRANSPARENCY_VALUE);
        }  
      //else
      //  rdObjectTransparency(surface,ro->frame.w,ro->frame.h,0xff);
    
      if (!ro->surface)
        ro->surfaceData = malloc(ro->frame.w*ro->frame.h*4);  
      
      ro->surface = (unsigned char*)cairo_image_surface_create_for_data(
          ro->surfaceData,
          CAIRO_FORMAT_ARGB32,
          ro->frame.w,ro->frame.h,
          ro->frame.w*4
          );
      
      image = ro->surface;
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
          // corners rotation:
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
        #ifdef _DEBUG
          printf("rootController forceangle %d\n",forceangle);
        #endif
        ro->frame.angle = forceangle;
        ro->forceangle = forceangle;
        switch(forceangle)
        {
          case 90:
            sector = _ROOT_SECTOR_LEFT;
            break;
          case 270:
            sector = _ROOT_SECTOR_RIGHT;
            break;
          case 180:
            sector = _ROOT_SECTOR_UP;
            break;
          case 0:
            sector = _ROOT_SECTOR_DOWN;
            break;
        }
      }
      
      #ifdef _TOPLEVEL_PRIORITY
        if (activityGetToplevel(ro->act,0)==1)
      #endif
      {
        switch(sector)
        {
          case _ROOT_SECTOR_LEFT:
            for(ny = 0; ny < ro->frame.h; ny++)
            {
              for(nx = 0; nx < ro->frame.w; nx++)
              {
                memcpy((void*)pixel,surface+(ny*ro->frame.w*4)+(nx*4),4);
                memcpy(ro->surfaceData+(nx*ro->frame.h*4)+((ro->frame.w-ny)*4),(void*)pixel,4);
              }
            }
            break;
          case _ROOT_SECTOR_RIGHT:
            for(ny = 0; ny < ro->frame.h; ny++)
            {
              for(nx = 0; nx < ro->frame.w; nx++)
              {
                memcpy((void*)pixel,surface+(ny*ro->frame.w*4)+(nx*4),4);
                memcpy(ro->surfaceData+((ro->frame.w*4)*(ro->frame.h-nx))+(ny*4),(void*)pixel,4);
              }
            }
            break;
          case _ROOT_SECTOR_UP:
            for(ny = 0; ny < ro->frame.h; ny++)
            {
              for(nx = 0; nx < ro->frame.w; nx++)
              {
                memcpy((void*)pixel,surface+(ny*ro->frame.w*4)+(nx*4),4);
                memcpy(ro->surfaceData+((ro->frame.w*4)*(ro->frame.h-ny))+((ro->frame.w-nx)*4),(void*)pixel,4);
              }
            }
            break;
          case _ROOT_SECTOR_DOWN:
            memcpy(ro->surfaceData,surface,ro->frame.w*ro->frame.h*4);
            break;
          default:
            memcpy(ro->surfaceData,surface,ro->frame.w*ro->frame.h*4);
            break;
        }
      }
      #ifdef _TOPLEVEL_PRIORITY
        else
        { 
          memcpy(ro->surfaceData,surface,ro->frame.w*ro->frame.h*4);
        }
      #endif
      activitySetAngle(ro->act,ro->frame.angle,0);
    
      #ifdef _USE_SKIN // frame
        if (activityGetMovable(ro->act,0)==1)
        {
          static unsigned char r;
          static unsigned char g;
          static unsigned char b;
          static unsigned char a;
          static struct mikaSkinNode *node = NULL;
          node = mikaSkinGlobalGetNode((unsigned char*)"mikaBackground");
          if (node)
          {
            mikaSkinGlobalNodeGetColor(node,&r,&g,&b,&a);
            #ifdef _DEBUG
              printf("mikaSkin paint mikaBackground (%f%f%f)\n",
                r/256.0,g/256.0,b/256.0);
            #endif
            
            if (_TRANSPARENCY_VALUE!=0xff)
            {
              #ifdef _TOPLEVEL_PRIORITY
                if (
                  activityGetToplevel(ro->act,0)==1 &&
                  rd->active!=ro
                  )
              #endif
                cairo_set_source_rgba(cairo,r/256.0,g/256.0,b/256.0,0.7);
              else
                cairo_set_source_rgba(cairo,r/256.0,g/256.0,b/256.0,1.0);
            }
            else
              cairo_set_source_rgba(cairo,r/256.0,g/256.0,b/256.0,1.0);
              
            cairo_rectangle (cairo, 
              ro->frame.x-_ROOT_OBJECT_FRAME, ro->frame.y-_ROOT_OBJECT_FRAME, 
              ro->frame.w+(_ROOT_OBJECT_FRAME*2), ro->frame.h+(_ROOT_OBJECT_FRAME*2));
            cairo_fill (cairo);
          }
          else
          {
            #ifdef _DEBUG
              printf("mikaSkin warning, node not found\n");
            #endif
          }
        }
        cairo_set_line_width(cairo,1);
      #endif
    
      cairo_save(cairo);
      
      // transformation
      
      cairo_translate(cairo,
          (double)(ro->frame.w/2)+ro->frame.x,
          (double)(ro->frame.h/2)+ro->frame.y);
      //cairo_rotate(cairo,ro->frame.angle*3.14/180);
      cairo_rotate(cairo,0*3.14/180);
      cairo_scale(cairo,
        (double)(ro->frame.w/ro->frame.w),
        (double)(ro->frame.h/ro->frame.h));
      cairo_translate(cairo,
        (double)(-0.5*ro->frame.w),
        (double)(-0.5*ro->frame.h));
      
      cairo_set_source_surface(cairo,image,0,0);
      
      cairo_paint(cairo);     
      cairo_surface_destroy(image);
      cairo_surface_flush(image);
      cairo_surface_finish(image);
      cairo_restore(cairo);
      
    }
  }
  shmdt(ro->act->shm->data);
  
  binarySemaphorePost(ro->act->sem->id);
  
  #ifdef _DEBUG
    printf("rdDrawObjectSurfaceESYNC end\n");  
  #endif
    
  return 0;
}

/**
 *  redraw all active object's surfaces to the desktop
 *  it's safer to recreate cairo on every draw although
 *  it's more time consuming
 */
static void *_surfacedata = NULL;
#define _CAIRO_REUSE_NO // is faster, but the background redrawing is not done
#define _BG_IMAGE
int rdDrawObjectsSurfaceSYNC(
  struct rootDesktop *rd
)
{
  #ifdef _DEBUG
    printf("rdDrawObjectsSurfaceSYNC UEVENT\n");
  #endif
  
  static struct rootObject *current;
  static cairo_t *cairo = NULL;
  static cairo_surface_t *destination = NULL;
  static cairo_surface_t *bg = NULL;

  if (!rd)
    return -1;
  
  {
    if (!_surfacedata)
      _surfacedata = malloc(rd->frame.w*rd->frame.h*4);
    memset(_surfacedata,0xff,rd->frame.w*rd->frame.h*4);
    
    destination = cairo_image_surface_create_for_data(
        _surfacedata,
        CAIRO_FORMAT_ARGB32,
        rd->frame.w,
        rd->frame.h,
        4*rd->frame.w
      );
    
    cairo = cairo_create(destination);
  
    #ifdef _BG_IMAGE
      if (!bg)	
      {
        unsigned char filename[256];
        filename[0] = 0x00;
        if ( rootGetAttribute((unsigned char*)"background",(unsigned char*)filename) == 0 )
          bg = cairo_image_surface_create_from_png((char*)filename);
        else  
          bg = cairo_image_surface_create_from_png(_DEFAULT_BG);
      }
      if (bg)
      {
        cairo_set_source_surface(cairo, bg, 0, 0);
        cairo_paint(cairo);
      }
    #endif
  }
  
  // an object with a toplevel flag is forced to the first position
  #ifdef _DEBUG
    printf("rdDrawObjectsSurfaceSYNC toplevel start\n");
  #endif
  #ifdef _TOPLEVEL_PRIORITY
    current = rd->children;
    while(current) // loop
    {
      int toplevel = activityGetToplevel(current->act,1);
      if (toplevel==1)
      {
        rdAddObject(rd,current);
        break;
      }
      current = current->next;
    }
  #endif
  #ifdef _DEBUG
    printf("rdDrawObjectsSurfaceSYNC toplevel finish\n");
  #endif
  
  int n = 0;
  current = rd->children;
  while(current)
  {
    #ifdef _DEBUG
      printf("rdDrawObjectsSurfaceSYNC %p n %d OBJECT\n",current,n);
    #endif
    rdDrawObjectSurfaceESYNC(rd,current,cairo);
    current = current->next;
    n++;
  }
  
  rootPointerDrawESYNC(rd,cairo,rd->pointerx,rd->pointery);
  
  cairo_destroy(cairo);
  //cairo_surface_flush(destination);
  //cairo_surface_finish(destination);
  cairo_surface_destroy(destination);
  
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
  static cairo_t 		*cairo = NULL;
  static cairo_surface_t 	*surface = NULL;
  static cairo_surface_t	*source = NULL;
  
  #ifdef _DEBUG
    static unsigned long timestamp = 0;
    static unsigned long timestamp2 = 0;
    timestamp = schedullerTime();
    printf("windowEventExpose rdDraw scheduller start(%d) EVENTFLOW pending(%d)\n",
      timestamp,XPending(rd->window->dpy));
  #endif

  //XLockDisplay(rd->window->dpy);
  rootControllerLock(1); // 
  rdDrawObjectsSurfaceSYNC(rd);
  //rootControllerUnlock(1);
  #ifdef _DEBUG
    printf("windowEventExpose rdDraw objects finished\n");
  #endif
  /**
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
  */
  #ifdef _DEBUG
    printf("windowEventExpose rdDraw objects finished 01\n");
  #endif
  
  //void *data = shmat(rd->shmSurface->id,0,0);
  //void *data = _surfacedata;
  
  XGrabServer(rd->window->dpy);
  surface = cairo_xlib_surface_create(
                              rd->window->dpy,
                              rd->window->id,
                              DefaultVisual(rd->window->dpy,0),
                              rd->frame.w,
                              rd->frame.h
                                    );
  
  source = cairo_image_surface_create_for_data(
                              _surfacedata,
                              CAIRO_FORMAT_ARGB32,
                              rd->frame.w,
                              rd->frame.h,
                              4*rd->frame.w
      );
  
  cairo = cairo_create(surface);
  cairo_set_source_surface(cairo, source, 0, 0);
  cairo_paint(cairo);
  
  cairo_destroy(cairo);
  //cairo_surface_flush(surface);
  //cairo_surface_finish(surface);
  cairo_surface_destroy(surface);
  //cairo_surface_flush(source);
  //cairo_surface_finish(source);
  cairo_surface_destroy(source);
  XUngrabServer(rd->window->dpy);
  XFlush(rd->window->dpy);
  
  XSync(rd->window->dpy,FALSE);
  
  //static unsigned long pixel = 0;
  //XSetWindowBackground(rd->window->dpy,rd->window->id,pixel);
  //XClearWindow(rd->window->dpy,rd->window->id);
  //pixel++;
  
  //XFlush(rd->window->dpy);
          
  //if (!image)
  {
  /**  image = XCreateImage(
          rd->window->dpy,
          DefaultVisual(rd->window->dpy,rd->window->screen),
          DefaultDepth(rd->window->dpy,rd->window->screen),
          ZPixmap,
          0,
          data,
          rd->frame.w,
          rd->frame.h,
          format->scanline_pad,0);
  
          image->data = data;*/
  }
  
  #ifdef _DEBUG
    printf("windowEventExpose rdDraw objects finished 02\n");
  #endif
  
  //if (image)
  {
    /**        
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
      */
  #ifdef _DEBUG
    printf("windowEventExpose rdDraw objects painted\n");
  #endif
  
  //XFlush(rd->window->dpy);
  
  //  image->data = NULL;
  //  XDestroyImage(image);
  }
  //XSync(rd->window->dpy,FALSE);
           
  /** isn't needed, infact could cause some problems
   *
   */
  #ifdef _DEBUG
    printf("windowEventExpose rdDraw before finish\n");
  #endif
  //XFlush(rd->window->dpy);
  rootControllerUnlock(1);
  //XUnlockDisplay(rd->window->dpy);
  //XSync(rd->window->dpy,FALSE);
  
  #ifdef _DEBUG
    timestamp2 = schedullerTime();
    printf("windowEventExpose rdDraw scheduller finish(%d) dauer(%d) EVENTFLOW\n",
      timestamp2,timestamp2-timestamp);
  #endif
          
  return 0;
}

/** is object1 higher than object2 - needs to be sync
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
  
  #ifdef _DEBUG
    //printf("_object_top\n");
  #endif
  
  for(
    ny = y-(IO_TOUCH_FRAME*IO_TOUCH_STEP); 
    ny < y+(IO_TOUCH_FRAME*IO_TOUCH_STEP); ny+=IO_TOUCH_STEP)
  {
    nx = x-(IO_TOUCH_FRAME*IO_TOUCH_STEP);
    for(nx; nx < x+(IO_TOUCH_FRAME*IO_TOUCH_STEP); nx+=IO_TOUCH_STEP )
    {
      //printf("_object_top point(%dx%d)\n",nx,ny);
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
  
  #ifdef _DEBUG
    printf("_object_top(%p)\n",object);
  #endif
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
    printf("windowEventButtonDown EVENTFLOW\n");
  #endif
  
  rootControllerLock(2);
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
  rootControllerUnlock(2);
    
  return 0;
}

int lowWindowEvent(
  struct rootDesktop *rd
)
{
  static Display *dpy = NULL;
  static XEvent xe;
  static int loops = 0;

  if (!rd)
  {
    #ifdef _DEBUG
      #ifdef _POLL_DEBUG
        printf("lowWindowEvent error !rd\n");
      #endif
    #endif
    return -1;
  }
  
  #ifdef XLIB
    // it's advised to open a new Display connection
    //
    if (!dpy)
      dpy = rd->window->dpy;
    
    XNextEvent(dpy,&xe);
    
    while(1) // loop
    {
      #ifdef _DEBUG
        #ifdef _POLL_DEBUG
          printf("lowWindowEvent cycle\n");
        #endif
      #endif
      
      rootControllerLock(3);
      if (ePlatform.state == _STATE_INACTIVE) // loop dangerous 
      {
        rootControllerUnlock(3);
        #ifdef _DEBUG
          #ifdef _POLL_DEBUG
            printf("lowWindowEvent platform event\n");
          #endif
        #endif
        break;
      }
      else
      {
        #ifdef _DEBUG
          #ifdef _POLL_DEBUG
            printf("lowWindowEvent waiting for _STATE_INACTIVE\n");
          #endif
        #endif
      }
        
      rootControllerUnlock(3);
      usleep(_CLOCKTIK/5);
    }
    
    rootControllerLock(4);
    ePlatform.event = xe;
    ePlatform.state = _STATE_ACTIVE;
    rootControllerUnlock(4);
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
    //lowWindowEvent(rd);
    usleep(1000);
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
    // needs to be the absolute first XLib call made, even before
    // we open the display
    #ifdef _XTHREADS
      XInitThreads();
    #endif
    
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
  
    // hide the cursor
    #ifdef _HIDE_CURSOR
      Display *display = lw->dpy;
      Window window = lw->id;
          
      Cursor invisibleCursor;
      Pixmap bitmapNoData;
      XColor black;
      static char noData[] = { 0,0,0,0,0,0,0,0 };
      black.red = black.green = black.blue = 0;
          
      bitmapNoData = XCreateBitmapFromData(display, window, noData, 8, 8);
      invisibleCursor = XCreatePixmapCursor(display, bitmapNoData, bitmapNoData,
                          &black, &black, 0, 0);
      XDefineCursor(display,window, invisibleCursor);
      XFreeCursor(display, invisibleCursor);
    #endif
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

/** get the toplevel object if it exists
 */
struct rootObject *_get_toplevel_object( struct rootDesktop *rd )
{
  if (!rd)
  {
    #ifdef _DEBUG
      printf("_get_toplevel_activity ERROR invalid arguments\n");
    #endif
    return NULL;
  }
  
  #ifdef _DEBUG
    printf("_get_toplevel_object\n");
  #endif
  
  struct rootObject *current = rd->children;
  while(current) // ok
  {
    if (activityGetToplevel(current->act,0)==1)
      return current;
    current = current->next;
  }
  
  return NULL;
}

void _trigger_expose()
{
  //while(1)
  {
    /**
    int pending = 0;
    while(1)
    {
      XGrabServer(rdGlobal->window->dpy);
      pending = XPending(rdGlobal->window->dpy);
      #ifdef _DEBUG
        printf("lowWindowEvent pending(%d)\n",pending);
      #endif
      XUngrabServer(rdGlobal->window->dpy);
      if (pending<20)
        break;
      usleep(100000);
    }
    */
    static long long count = 0;
    #ifdef _DEBUG
      printf("lowWindowEvent _trigger_expose START %d\n",count);
    #endif
    rootControllerLock(91);
    
    /**if (ePlatform.state == _STATE_INACTIVE)
    {
      ePlatform.event.type = Expose;
      ePlatform.state = _STATE_ACTIVE;
      rootControllerUnlock(5);
      break;
    }*/
    
    XEvent ev;
    ev.type = Expose;
    ev.xexpose.window = rdGlobal->window->id;
    ev.xexpose.display = rdGlobal->window->dpy;  
    #ifdef _DEBUG
      printf("_trigger_expose 01 window(%d)\n",rdGlobal->window->id);
    #endif
    //XGrabServer(rdGlobal->window->dpy);
    XSendEvent(rdGlobal->window->dpy,rdGlobal->window->id,True,ExposureMask,&ev);
    ev.type = ClientMessage;
    //int n = 0;
    //for(n = 0; n < 10; n++)
    //  XSendEvent(rdGlobal->window->dpy,rdGlobal->window->id,True,ClientMessageMask,&ev);
    XFlush(rdGlobal->window->dpy);
    //XUngrabServer(rdGlobal->window->dpy);
    
    rootControllerUnlock(92);
    #ifdef _DEBUG
      printf("lowWindowEvent _trigger_expose FINISH\n");
    #endif
    //usleep(_CLOCKTIK);
    count++;
  }
}

/** possibly reverse buttons after deremining the window
 */
#define _REVERSE_BUTTONS
unsigned long _ts_bdown = 0;
int _bdown_x = 0;
int _bdown_y = 0;
int rdEventHandler(
  struct rootDesktop *rd
)
{
  if (!rd)
    return -1;
  
  XEvent epcopy;
  
  static long long count = 0;
  int 	x;
  int 	y;
  int 	rx;
  int 	ry;
  int 	oldx = 0;
  int 	oldy = 0;
  int 	sector;
  int 	motionx;
  int 	motiony;
  struct rootObject *active = NULL;
  struct rootObject *toplevel = NULL;
  
  for(;;)
  {
    count++;
    #ifdef _DEBUG
      #ifdef _POLL_DEBUG
        printf("lowWindowEvent window(%d) count(%d)\n",rdGlobal->window->id,count);
      #endif
    #endif
  /**
    rootControllerLock(5);
    if (ePlatform.state == _STATE_ACTIVE)
    {
      ePlatform.state = _STATE_INACTIVE;
      epcopy = ePlatform.event;
      rootControllerUnlock(5);
      goto platformhit;
    }
    rootControllerUnlock(6);
    usleep(_CLOCKTIK);
    continue;
  platformhit:*/
  
    XNextEvent(rd->window->dpy,&epcopy);
  
    switch(epcopy.type)
    {
      case ButtonPress:
        XRaiseWindow(rd->window->dpy,rd->window->id);
        _ts_bdown = schedullerTime();
        _bdown_x = epcopy.xbutton.x;
        _bdown_y = epcopy.xbutton.y;
        idle_mode = 1;
        #ifdef _DEBUG
          printf("lowWindowEvent ButtonPress EVENTFLOW\n");
        #endif
        rootActivityOnEvent(&rootAct);
        
        rootControllerLock(7);
        #ifdef _AI
          active = _object_top(rd,&epcopy.xbutton.x,&epcopy.xbutton.y);
        #else
          active = rdPositionToObject(rd,epcopy.xbutton.x,epcopy.xbutton.y);
        #endif
        
        if (!active)
        {
          rootControllerUnlock(8);
          break;
        }
        
        rootControllerUnlock(9);
        
        #ifdef _MOTION_ALL
          toplevel = _get_toplevel_object(rd);
          if (toplevel)
          {
            if (activityGetVisible(toplevel->act,1))
              active = toplevel;
          }
        #endif
        
        #ifdef _DEBUG
          printf("lowWindowEvent current(%dx%d)\n",currentx,currenty);
        #endif
        currentx = epcopy.xbutton.x;
        currenty = epcopy.xbutton.y;
        
        rd->pointerx = currentx;
        rd->pointery = currenty;
        
        oldx = currentx;
        oldy = currenty;
        
        if (active)
        {
          #ifdef _DEBUG
            printf("lowWindowEvent push active(%p)\n",active);
          #endif
          rdPushObject(rd,active);
        
          rx = epcopy.xbutton.x-active->frame.x;
          ry = epcopy.xbutton.y-active->frame.y;
            
          struct _frame frame;// = *active->frame;
          memcpy(&frame,&active->frame,sizeof(struct _frame));
          if (active->forceangle!=361)
          {
            switch(active->forceangle)
            {
              case 90:
                sector = _ROOT_SECTOR_LEFT;
                break;
              case 270:
                sector = _ROOT_SECTOR_RIGHT;
                break;
              case 180:
                sector = _ROOT_SECTOR_UP;
                break;
              case 0:
                sector = _ROOT_SECTOR_DOWN;
                break;
            }
          }
          else
            sector = rootSectorByPosition(
                frame.x,frame.y,
                frame.w,frame.h,
                rd->frame.w,rd->frame.h);
          
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
          
          windowEventButtonDown(rd,active,epcopy.xbutton.x,epcopy.xbutton.y);
          rootSchedullerButtondown(scheduller,active,rx,ry);
        }
        rootSchedullerExpose(scheduller);
      break;
      case ButtonRelease:
        _trans = 1;
        #ifdef _DEBUG
          printf("lowWindoEvent ButtonRelease\n");
        #endif
        rootActivityOnEvent(&rootAct);
        
        rd->pointerx = epcopy.xbutton.x;
        rd->pointery = epcopy.xbutton.y;
        
        rd->active = NULL;
        
          rootControllerLock(10);
          #ifdef _AI
            active = _object_top(rd,&epcopy.xbutton.x,&epcopy.xbutton.y);
          #else
            active = rdPositionToObject(rd,epcopy.xbutton.x,epcopy.xbutton.y);
          #endif
          rootControllerUnlock(11);
          
          if (active)
          {
            rx = epcopy.xbutton.x-active->frame.x;
            ry = epcopy.xbutton.y-active->frame.y;
            
              struct _frame frame;// = *active->frame;
              memcpy(&frame,&active->frame,sizeof(struct _frame));
              if (active->forceangle!=361)
              {
                switch(active->forceangle)
                {
                  case 90:
                    sector = _ROOT_SECTOR_LEFT;
                    break;
                  case 270:
                    sector = _ROOT_SECTOR_RIGHT;
                    break;
                  case 180:
                    sector = _ROOT_SECTOR_UP;
                    break;
                  case 0:
                    sector = _ROOT_SECTOR_DOWN;
                    break;
                }
              }
              else
                sector = rootSectorByPosition(
                  frame.x,frame.y,
                  frame.w,frame.h,
                  rd->frame.w,rd->frame.h);
            
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
              oldx-20<epcopy.xbutton.x &&
              oldx+20>epcopy.xbutton.x &&
              oldy-20<epcopy.xbutton.y &&
              oldy+20>epcopy.xbutton.y ) ||
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
          rootSchedullerExpose(scheduller);
      break;
      case MotionNotify:
        _trans = 0;
        
        if (!rd->active)
          break;
        
        #ifdef _DEBUG
          printf("_motion_notify bdown(%d) current(%d)\n",
            _ts_bdown,schedullerTime());
        #endif
        if (
          schedullerTime()-350 > _ts_bdown ||
          (
            _bdown_x-20<epcopy.xmotion.x &&
            _bdown_x+20>epcopy.xmotion.x &&
            _bdown_y-20<epcopy.xmotion.y &&
            _bdown_y+20>epcopy.xmotion.y
          )
        )
        {
          _ts_bdown = schedullerTime();
          break;
        }  
        idle_mode = 1;
        #ifdef _DEBUG
          printf("lowWindowEvent Motion (%d,%d)\n",
            epcopy.xmotion.x,epcopy.xmotion.y);
        #endif
        rootActivityOnEvent(&rootAct);
        
        rd->pointerx = epcopy.xmotion.x;
        rd->pointery = epcopy.xmotion.y;
        
        rootControllerLock(12);
        
        if (rd->active)
        {
          active = rd->active;
          if (1) // could cause synchronisation problems
          {
            static struct _frame frame;// = *active->frame;
            memcpy(&frame,&active->frame,sizeof(struct _frame));
            if (active->forceangle!=361)
              {
                switch(active->forceangle)
                {
                  case 90:
                    sector = _ROOT_SECTOR_LEFT;
                    break;
                  case 270:
                    sector = _ROOT_SECTOR_RIGHT;
                    break;
                  case 180:
                    sector = _ROOT_SECTOR_UP;
                    break;
                  case 0:
                    sector = _ROOT_SECTOR_DOWN;
                    break;
                }
              }
              else
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
          
          if (activityGetMovable(rd->active->act,1)==1)
          {
            // move the actual active object
            #ifdef _MOTION_ACCEL
              // resolve inheritance
              
              if (currentx<epcopy.xmotion.x-currentx)
                motionx = epcopy.xmotion.x*0.6;
              else
                motionx = epcopy.xmotion.x*1.5;
                
              if (currenty<epcopy.xmotion.y-currenty)
                motiony = epcopy.xmotion.y*0.6;
              else
                motiony = epcopy.xmotion.y*1.5;  
              
              rd->active->frame.x += (epcopy.xmotion.x-currentx)*1.5;
              rd->active->frame.y += (epcopy.xmotion.y-currenty)*1.5;
            #else
              rd->active->frame.x += epcopy.xmotion.x-currentx;
              rd->active->frame.y += epcopy.xmotion.y-currenty;
            #endif
            
            if ( rd->active->frame.x < 0 ) rd->active->frame.x = 0;
            if ( rd->active->frame.y < 0 ) rd->active->frame.y = 0;
            
            if ( rd->active->frame.x+rd->active->frame.w > rd->frame.w )
              rd->active->frame.x = rd->frame.w-rd->active->frame.w;
            
            if ( rd->active->frame.y+rd->active->frame.h > rd->frame.h )
              rd->active->frame.y = rd->frame.h-rd->active->frame.h;
          }
          #ifdef _ANGLE_TEST
            rd->active->frame.angle += 1;
          #endif
          rootControllerUnlock(13);
          rootSchedullerExpose(scheduller);
          
          if (
              (
              oldx+20<epcopy.xbutton.x ||
              oldx-20>epcopy.xbutton.x ||
              oldy+20<epcopy.xbutton.y ||
              oldy-20>epcopy.xbutton.y) &&
              rd->active
            )
            rootSchedullerButtondownReset(scheduller);
            
        }
        else
        {
          rootControllerUnlock(14);
        }
        
        #ifdef _DEBUG
          printf("lowWindowEvent current(%dx%d) before\n",currentx,currenty);
        #endif
        
        currentx = epcopy.xmotion.x;
        currenty = epcopy.xmotion.y;
        
        #ifdef _DEBUG
          printf("lowWindowEvent Motion finish processing\n");
        #endif
      break;
      case Expose:
        //#ifdef _DEBUG
        //  printf("lowWindowEvent Expose\n");
        //#endif
        /** we could get into synchronization problems on some Xorg servers
         *  if this event would be handled by the scheduller in a separate
         *  thread
         *
         */
        //#ifdef _EXPOSE_RESPONSE
        //  rootSchedullerExpose(scheduller);
        //#endif
        windowEventExpose(rdGlobal);
      break;
    }
    #ifdef _DEBUG
      printf("lowWindowEvent finish\n");
    #endif
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

int errorHandler( Display *dpy, XErrorEvent *event )
{
  #ifdef _DEBUG
    printf("errorHandler\n");
  #endif
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
  
  rd->pointerx = 100;
  rd->pointery = 100;
  
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
      rootControllerLock(20);
      rootControllerUnlock(21);
    }
    #ifdef _DEBUG
      printf("rdInit register rootsemaphore finish\n");
    #endif
  }
  
  // deprecated, remove
  rd->atomActPoll = ratomAlloc(rd,NULL);
  rd->atomActPoll->data = rd;
  rd->atomActPoll->call = rdPollObjectsThread;
  #ifndef _MIKA_SOCKET
    ratomInit(rd,rd->atomActPoll);
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
  
  pthread_t wt;
  pthread_create(&wt,NULL,rootControllerWatchdog,NULL);
  
  XSetErrorHandler(errorHandler);
  
  struct rootModules *myModules = rootModulesNew();
  rootModulesRun(myModules);
  
  #ifdef _MIKA_SOCKET
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
  ro->surface = NULL;
  ro->forceangle = 361;
  
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
  while(current) // ok
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
  while(current) // ok
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
  #ifdef _DEBUG
    printf("r dCleanupObjects\n");
  #endif
   
  struct rootObject *current;
  struct rootObject *next;
  
  current = rd->children;
  while(current) // ok
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
int rdPollObjects( // only run this for a limited amount of time on speciffic systems
  struct rootDesktop *rd
)
{ // loop dangerous
  static unsigned char path[128];

  if (!rd)
  {
    #ifdef _DEBUG
      printf("rdPollObjects error !rd\n");
    #endif
    return -1;
  }
  
  #ifdef _DEBUG
    printf("rdPollObjects\n");
  #endif
  
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
      ro->surface = NULL;
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

/** should be avoided, we poll at the moment we get a socket connection
 */
#define _POLL_OBJECTS_INTERVAL 500000
void *rdPollObjectsThread(
  void *data
)
{
  #ifndef _POLL_ACTIVITY_DIRECTORY
    return NULL;
  #endif
  
  struct rootDesktop *rd = (struct rootDesktop*)data;
  int pollCount = 0;
  for(;;)
  {
    #ifdef _DEBUG
      printf("rdPollObjectsThread cycle\n");
    #endif
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
  
  #ifdef _DEBUG
    printf("rdPositionToObject\n");
  #endif
  
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
 *  handling incomming connections on mika.sock
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
        printf("_socket_handler connection rootController\n");
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
                ro->surface = NULL;
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

/** watchdog to check if we didn't run into synchronization problems
 */
#define _ROOT_CONTROLLER_WATCHDOG_TIK 500000
void *rootControllerWatchdog(void*data)
{
  long long count = 0;
  while(1)
  {
    #ifdef _DEBUG
      printf("rootControllerWatchdog count(%d)\n",count);
    #endif
    binarySemaphoreWait(rdGlobal->sem->id);
    binarySemaphorePost(rdGlobal->sem->id);
    count++;
    usleep(_ROOT_CONTROLLER_WATCHDOG_TIK);
  }
}

#ifdef _STATIC
  
  int main( )
  {
    struct _frame frame;
    struct _frame smallframe;
    frame.x = 0;
    frame.y = 0;
    
    unsigned char resolution[16];
    resolution[0] = 0x00;
    if ( rootGetAttribute((unsigned char*)"resolution",(unsigned char*)resolution) == 0 )
    {
      sscanf((char*)resolution,"%dx%d",&frame.w,&frame.h);
      #ifdef _DEBUG
        printf("resolution %s %dx%d\n",(char*)resolution,frame.w,frame.h);
      #endif
    }
    else
    {
      frame.w = _ROOT_DEFAULT_WIDTH;
      frame.h = _ROOT_DEFAULT_HEIGHT;
    }
    
    smallframe.x = 0;
    smallframe.y = 0;
    smallframe.w = 320;
    smallframe.h = 320;
    
    serial = mikaSerialInit(NULL);
    appRegisterAll();
    
    osInit();
    activityObjectsInit();
    
    mikaSkinGlobalInit();
    
    /** scheduller
     */
    #ifdef _DEBUG
      unsigned long ts = schedullerTime();
      usleep(18000);
      ts = schedullerTime();
      printf("schedullerTime 18000/1000 ms(%d)\n",ts);
    #endif
    
    #ifdef _MIKA_SOCKET
      trop_IPC *ipc = trop_IPCInit(NULL);
      if (ipc)
      {
        trop_IPCUdsInit(ipc,"./mika.sock");
        trop_IPCUdsCustomQueue(ipc,_socket_handler);
      }
      else
      {
        printf("could not create ./mika.sock\n");
        exit(2);
      }
    #endif
    
    #ifdef _ROOT_STRESS
      struct rootStress *stress = rootStressNew();
      stress->loopTime = 10000;
      if (0) rootStressStart(stress);
    #endif
    
    if (!actObjects)
      actObjects = activityObjectsClone();
    if (actObjects)
    {
      if ( activityObjectRegister(actObjects,(unsigned char*)"rootExpose") != 0 )
      {
        #ifdef _DEBUG
          printf("rootController ERROR couldn't register activityObject rootExpose\n");
        #endif
      }
      else
      {
        struct activityObject oExpose;
        activityObjectClone(actObjects,(unsigned char*)"rootExpose",&oExpose);
        oExpose.fastData[0] = 0;
        activityObjectSet(actObjects,(unsigned char*)"rootExpose",&oExpose);
      }
    }
    else
    {
      #ifdef _DEBUG
        printf("rootController ERROR couldn't clone actObjects\n");
      #endif
    }
    
    scheduller = rootSchedullerNew();
    rootSchedullerRun(scheduller);
    struct rootDesktop *rd = rdInit(NULL,frame);
    
    return 0;
  }
#endif
