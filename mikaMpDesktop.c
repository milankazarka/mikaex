/** this is the Mika Mp Desktop
 *
 */
#include "mikaMpDesktop.h"
#include "config.h"
#include "mikaCe.h"
#include "mikaMessage.h"
#define _STATIC
#define _DEPTH 32
#define _MIKA_HW_FULLSCREEN_NO
#define _MIKA_MP_TEST
#ifdef _MIKA_MP_TEST
  #define _MIKA_MP_TEST_TIK 1000000
#endif
#define _MIKA_MP_WINDOW_CLEAR 0xda
#define _MIKA_MP_MULTITOUCH_NO	// from SDL 1.3
#define _MIKA_MP_OPENGL_NO
#define _MIKA_MP_SCHED

#ifdef _MIKA_MP_OPENGL
  #include <GL/gl.h>
  #include <GL/glx.h>
  #include <GL/glu.h>
#endif

#ifdef _STATIC
  void *init(void*data);
#endif

static mikaMpDesktop *mpDesktop = NULL;

mikaMpAction::mikaMpAction( )
{
  last = NULL;
  next = NULL;
}

mikaMpAction::~mikaMpAction( )
{
}

mikaMpDesktopAtom::mikaMpDesktopAtom( struct mikaFrame *_frame )
{
  frame = *_frame;
  type = _MIKA_MP_DESKTOP_ATOM_TYPE_DEFAULT;
}

mikaMpDesktopAtom::~mikaMpDesktopAtom( )
{
}

void mikaMpDesktopAtom::eventExpose( )
{
}

mikaMpWindow::mikaMpWindow( struct mikaFrame *_frame )
{
  frame = *_frame;
  last = NULL;
  next = NULL;
  pbo = new mikaPBO(&frame);
  pbo->clearChar = _MIKA_MP_WINDOW_CLEAR;
  state = _MIKA_MP_WINDOW_STATE_NORMAL;
}

mikaMpWindow::~mikaMpWindow( )
{
  if (last)
    last->next = next;
  delete pbo;
}

void mikaMpWindow::eventExpose( )
{
  switch(state)
  {
    case _MIKA_MP_WINDOW_STATE_NORMAL:
      pbo->clear();
      cairo_save(mpDesktop->cairo);
      cairo_translate(mpDesktop->cairo,
        (double)frame.x, 
        (double)frame.y
      );
      cairo_set_source_surface(mpDesktop->cairo,pbo->cairo_surface,0,0);
      cairo_paint(mpDesktop->cairo);
      #ifndef _MIKA_MP_OPENGL
        cairo_surface_flush(pbo->cairo_surface);
      #endif
      cairo_restore(mpDesktop->cairo);
      break;
    case _MIKA_MP_WINDOW_STATE_ICONIFIED:
      break;
  }
}

/** draw a test object on the desktop's surface
 *  - picked up from the official Cairo website
 */
void mikaMpDesktop::drawTest( )
{
  double x         = 25.6,        /* parameters like cairo_rectangle */
         y         = 25.6,
     width         = 204.8,
     height        = 204.8,
     aspect        = 1.0,     /* aspect ratio */
     corner_radius = height / 10.0;   /* and corner curvature radius */
                                     
  double radius = corner_radius / aspect;
  double degrees = M_PI / 180.0;
                                     
  cairo_new_sub_path (cairo);
  cairo_arc (cairo, x + width - radius, y + radius, radius, -90 * degrees, 0 * degrees);
  cairo_arc (cairo, x + width - radius, y + height - radius, radius, 0 * degrees, 90 * degrees);
  cairo_arc (cairo, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
  cairo_arc (cairo, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
  cairo_close_path (cairo);
                                     
  cairo_set_source_rgb (cairo, 0.5, 0.5, 1);
  cairo_fill_preserve (cairo);
  cairo_set_source_rgba (cairo, 0.5, 0, 0, 0.5);
  cairo_set_line_width (cairo, 10.0);
  cairo_stroke (cairo);
}

/** redraw the surface and it's objects
 */
int mikaMpDesktop::eventExpose( )
{
  #ifdef _DEBUG
    printf("mikaMpDesktop::eventExpose\n");
  #endif
  
  #ifndef _MIKA_MP_OPENGL
    if(SDL_MUSTLOCK(surface)) 
      if(SDL_LockSurface(surface) < 0) return 1;
  #endif 
    
  clear();
  
  #ifdef _MIKA_MP_TEST
    drawTest();
  #endif
  
  mikaMpWindow *current = windows;
  while(current)
  {
    current->eventExpose();
    current = current->next;
  }
    
  mikaMpDesktopAtom *currentAtom = atoms;
  while(currentAtom)
  {
    currentAtom->eventExpose();
    currentAtom = currentAtom->next;
  }
  
  #ifdef _MIKA_MP_OPENGL
    drawGL();
  #endif
  
  #ifndef _MIKA_MP_OPENGL
    if(SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
    SDL_Flip(surface);
  #endif
  
  return 0;
}

/** determine which window resides at a position
 */
mikaMpWindow *mikaMpDesktop::windowAtPosition( int x, int y )
{
  mikaMpWindow *window = NULL;
  mikaMpWindow *current = windows;
  while(current)
  {
    if (
      current->frame.x < x &&
      current->frame.y < y &&
      current->frame.x+current->frame.w > x &&
      current->frame.y+current->frame.h > y
      )
    {
      window = current;
    }
    current = current->next;
  }
  if (window)
  {
    #ifdef _DEBUG
      printf("mikaMpDesktop::windowAtPosition window(%p)\n",
        window);
    #endif
  }
  return window;
}

void mikaMpDesktop::onButtonDown( int x, int y )
{
  mikaMpWindow *activeWindow = windowAtPosition(x,y);
  if (activeWindow)
  {
    mikaMpAction *action = new mikaMpAction();
    action->type = _MIKA_MP_ACTION_BUTTONUP;
    action->x = x;
    action->y = y;
    if (actions)
      actions->last = action;
    action->next = actions;
    actions = action;
    action->window = activeWindow;
    pushWindow(activeWindow);
  }
}

void mikaMpDesktop::onButtonUp( int x, int y )
{
  mikaMpWindow *activeWindow = windowAtPosition(x,y);
  if (activeWindow)
  {
    mikaMpAction *current = actions;
    while(current)
    {
      if (current->type==_MIKA_MP_ACTION_BUTTONUP)
      {
        if (current->last)
          current->last->next = current->next;
        else
          actions = current->next;
        delete current;
        break;
      }
      current = current->next;
    }
  }
}

void mikaMpDesktop::onMotion( int x, int y )
{
  int movex;
  int movey;
  mikaMpAction *current = actions;
  while(current)
  {
    if (current->type==_MIKA_MP_ACTION_BUTTONUP)
    {
      movex = x-current->x;
      movey = y-current->y;
      current->x = x;
      current->y = y;
      
      mikaMpWindow *currentWindow = windows;
      while(currentWindow)
      {
        if (currentWindow==current->window)
        {
          currentWindow->frame.x += movex;
          currentWindow->frame.y += movey;
          triggerExpose();
          break;
        }
        currentWindow = currentWindow->next;
      }
      
      break;
    }
    current = current->next;
  }
}

/** SDL event loop
 */
int mikaMpDesktop::eventLoop( )
{
  bool done;
  SDL_Event quit_ev;
  SDL_KeyboardEvent *key_ev;
  #ifdef _MIKA_MP_MULTITOUCH
    SDL_TouchFingerEvent *touch_ev;
  #endif
  quit_ev.type = SDL_QUIT;
  SDL_Event event;
  // schedulled event
  struct mikaMpEvent_t *mpEvent = NULL;
       
  while((!done) && (SDL_WaitEvent(&event))) 
  {
    switch(event.type) 
    {
      case SDL_MOUSEMOTION:
        onMotion(event.button.x,event.button.y);
        break;
      case SDL_MOUSEBUTTONDOWN:
        #ifdef _DEBUG
          printf("mikaMpDesktop::eventLoop SDL_MOUSEBUTTONDOWN (%d/%d)\n",
            event.button.x,event.button.y);
        #endif
        onButtonDown(event.button.x,event.button.y);
        break;
      case SDL_MOUSEBUTTONUP:
        #ifdef _DEBUG
          printf("mikaMpDesktop::eventLoop SDL_MOUSEBUTTONUP (%d/%d)\n",
            event.button.x,event.button.y);
        #endif
        onButtonUp(event.button.x,event.button.y);
        break;
      #ifdef _MIKA_MP_MULTITOUCH
        // multi-touch events
        case SDL_FINGERDOWN:
          touch_ev = ( SDL_TouchFingerEvent * ) &event;
          break;
        case SDL_FINGERUP:
          touch_ev = ( SDL_TouchFingerEvent * ) &event;
          break;
        case SDL_FINGERMOTION:
          touch_ev = ( SDL_TouchFingerEvent * ) &event;
          break;
      #endif
      case SDL_USEREVENT:
        #ifdef _DEBUG
          printf("mikaMpDesktop::eventLoop SDL_USEREVENT\n");
        #endif
        break;
      case SDL_KEYDOWN:
      case SDL_KEYUP:
        key_ev = ( SDL_KeyboardEvent * ) &event;
        if ( key_ev->keysym.sym == SDLK_RETURN )
        {
          SDL_PushEvent( &quit_ev );
        }
        break;                                                                                                                    
      case SDL_VIDEOEXPOSE:
        #ifdef _DEBUG
          printf("mikaMpDesktop::eventLoop SDL_VIDEOEXPOSE\n");
        #endif
        
        #ifdef _MIKA_MP_SCHED
          mpEvent = mikaMpEventNew();
          mpEvent->Id = _MIKA_MP_ACTION_EXPOSE;
          mikaMpEventAddCustom(mpEvent,this,_MIKA_MP_ACTION_DATA_DESKTOP);
          globalSched->onEvent(mpEvent);
        #else
          eventExpose();
        #endif
        break;
      default:
        #ifdef _DEBUG
          printf("mikaMpDesktop::eventLoop default event\n");
        #endif
        break;
      case SDL_QUIT:
        done = true;
        break;                               
    }
  }
                               
  
  return 0;
}

void *mikaMpDesktopEventsThread( void *data )
{
  mikaMpDesktop *desktop = (mikaMpDesktop*)data;
  
  #ifdef _MIKA_MP_TEST
    while(1)
    {
      #ifdef _DEBUG
        printf("mikaMpDesktopEventsThread cycle\n");
      #endif
      SDL_Event event;
      event.type = SDL_VIDEOEXPOSE;
      SDL_PushEvent(&event);
      usleep(_MIKA_MP_TEST_TIK);
    }
  #endif
}

/** initialize OpenGL, setup
 */
int mikaMpDesktop::initGL( )
{
  #ifdef _MIKA_MP_OPENGL
      #ifdef _DEBUG
        printf ("OpenGL version: %s\n", glGetString (GL_VERSION));
        printf ("OpenGL vendor: %s\n", glGetString (GL_VENDOR));
        printf ("OpenGL renderer: %s\n", glGetString (GL_RENDERER));
      #endif
      
      SDL_GL_SetAttribute (SDL_GL_RED_SIZE, 8);
      SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE, 8);
      SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE, 8);
      SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, 32);
      SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,  1); 
      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,  2); 
      
      glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
      glDisable (GL_DEPTH_TEST);
      glEnable (GL_BLEND);
      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glEnable (GL_TEXTURE_RECTANGLE_ARB);
  #endif
  return 0;
}

int mikaMpDesktop::drawGL( )
{
  #ifdef _MIKA_MP_OPENGL
    static unsigned int _texture_id = -1;
    static unsigned int *texture_id = &_texture_id;
    static unsigned int displayListId;
    
    if (_texture_id==-1)
    {
      glViewport (0, 0, frame.w, frame.h);
      glMatrixMode (GL_PROJECTION);
      glLoadIdentity ();
      glOrtho (0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
      
      glClear (GL_COLOR_BUFFER_BIT);
       	
      glGenTextures (1, texture_id);
      glBindTexture (GL_TEXTURE_RECTANGLE_ARB, *texture_id);
      glTexImage2D (GL_TEXTURE_RECTANGLE_ARB,
       			      0,
       			      GL_RGBA,
       			      frame.w,
       			      frame.h,
       			      0,
       			      GL_BGRA,
       			      GL_UNSIGNED_BYTE,
       			      NULL);
      glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
      
      displayListId = glGenLists(1); //Make room for the display list
      glNewList(displayListId, GL_COMPILE);
      // list of OpenGL commands to execute
      glEndList();
    }
    
    glCallList(displayListId);
    
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
    glClear (GL_COLOR_BUFFER_BIT);
     		
    glPushMatrix ();
     	
    glBindTexture (GL_TEXTURE_RECTANGLE_ARB, *texture_id);
    glTexImage2D (GL_TEXTURE_RECTANGLE_ARB,
     			      0,
     			      GL_RGBA,
     			      frame.w,
     			      frame.h,
     			      0,
     			      GL_BGRA,
     			      GL_UNSIGNED_BYTE,
     			      pbo->pixels);
     	
    glColor3f (0.25f, 0.5f, 1.0f);
    glBegin (GL_QUADS);
    
    glTexCoord2f (0.0f, 0.0f); 			glVertex2f (0.0f, 1.0f);
    glTexCoord2f ((GLfloat)frame.w, 0.0f); 	glVertex2f (1.0f, 1.0f);
    glTexCoord2f ((GLfloat)frame.w, (GLfloat)frame.h); glVertex2f (1.0f, 0.0f);
    glTexCoord2f (0.0f, (GLfloat)frame.h); 	glVertex2f (0.0f, 0.0f);
    
    glEnd ();
    
    //glPopMatrix ();
     	
    SDL_GL_SwapBuffers();
  #endif
  return 0;
}

/** 
 *  (mikaMpSched)
 *  scheduller triggered event handler - triggers the Expose
 *  event. In general it is a good idea to have the rule for the
 *  Expose event have the flag 'merge' set to '1', so that
 *  there is always only one Expose event in the Queue, which makes
 *  moving objects / repainting smooth
 */
void *schedOnExpose( void *sched, struct mikaMpEvent_t *event )
{  
  if (!sched || !event)
    return NULL;
  
  mikaMpSched *dSched = (mikaMpSched*)sched;
  struct mikaMpEventData_t *currentData = event->custom;
  mikaMpDesktop *desktop = NULL;
  while(currentData)
  {
    if (currentData->id==_MIKA_MP_ACTION_DATA_DESKTOP)
      desktop = (mikaMpDesktop*)currentData->data;
    currentData = currentData->next;
  }
  
  if (desktop)
  {
    #ifdef _DEBUG
      printf("schedOnExpose\n");
    #endif
    desktop->eventExpose();
  }
  
  dSched->removeEvent(event);
  
  return NULL;
}

mikaMpDesktop::mikaMpDesktop( struct mikaFrame *_frame )
{
  mpDesktop	= this;
  frame 	= *_frame;
  windows 	= NULL;
  atoms		= NULL;
  
  // initialize the global event scheduller
  // divert events through the scheduller to apply rules to events
  mikaMpSchedGlobalInit();
  globalSched->eventLoop();
  // create rules for handling events
  struct mikaMpSchedRuleDelegate_t *ruleDelegate = (struct mikaMpSchedRuleDelegate_t*)malloc(
    sizeof(struct mikaMpSchedRuleDelegate_t));
  ruleDelegate->onEvent = schedOnExpose;
  mikaMpSchedRule *ruleExpose = new mikaMpSchedRule(ruleDelegate);
  ruleExpose->eventId = _MIKA_MP_ACTION_EXPOSE;
  ruleExpose->merge = 0;  // no Expose events in Queue, only Expose
                          // after we finished last Expose
  globalSched->addRule(ruleExpose);
  
  // initialize the Server - we listen for applications to register
  // themselves & their window content (mikaMpWindowClient/mikaApps)
  //
  if ( mikaMpDesktopServerGlobalInit()!=0 )
  {
    #ifdef _DEBUG
      printf("mikaMpDesktop::mikaMpDesktop warning, unable to setup server\n");
    #endif
  }
  
  if (SDL_Init(SDL_INIT_VIDEO) < 0 )
  {
    #ifdef _DEBUG
      printf("mikaMpWindow::mikaMpWindow error couldn't initialize SDL\n");
    #endif
    return;
  }
  if (!(surface = SDL_SetVideoMode(
    frame.w, frame.h, _DEPTH,
    #ifdef _MIKA_HW_FULLSCREEN 
      SDL_FULLSCREEN|SDL_HWSURFACE)))
    #else
      #ifdef _MIKA_MP_OPENGL
        SDL_GL_DOUBLEBUFFER|SDL_OPENGL)))
      #else
        SDL_ANYFORMAT)))
      #endif
    #endif
  {
    #ifdef _DEBUG
      printf("mikaMpWindow::mikaMpWindow error couldn't create window\n");
    #endif
    SDL_Quit();
    exit(2);
  }
  
  #ifdef _MIKA_MP_OPENGL
    initGL();
  #endif
  
  pbo = new mikaPBO(&frame);
  pbo->clearChar = 0xc0;
  if(SDL_MUSTLOCK(surface)) 
    if(SDL_LockSurface(surface) < 0) return;
  #ifdef _MIKA_MP_OPENGL
  #else
    pbo->pixels = surface->pixels;
  #endif
  pbo->cairo_surface = cairo_image_surface_create_for_data(
    (unsigned char*)pbo->pixels, CAIRO_FORMAT_ARGB32,
    frame.w,frame.h,
    4*frame.w
    );
  pbo->clear();
  cairo = cairo_create(pbo->cairo_surface);
  if(SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
  
  pthread_t initThread;
  pthread_create(&initThread,NULL,init,this);
  
  pthread_mutex_init(&mutex,NULL);
  pthread_create(&events_thread,NULL,mikaMpDesktopEventsThread,this);
  eventLoop();
}

mikaMpDesktop::~mikaMpDesktop( )
{
}

void mikaMpDesktop::lock( )
{
  pthread_mutex_lock(&mutex);
}

void mikaMpDesktop::unlock( )
{
  pthread_mutex_unlock(&mutex);
}

int mikaMpDesktop::addWindow( mikaMpWindow *window )
{
  if (!window)
  {
    #ifdef _DEBUG
      printf("mikaMpWindow::addWindow error invalid arguments\n");
    #endif
    return 1;
  }
  
  lock();
  if (windows)
    windows->last = window;
  window->next = windows;
  windows = window;
  unlock();
  
  return 0;
}

int mikaMpDesktop::rmWindow( mikaMpWindow *window )
{
  lock();
  delete window;
  unlock();
  return 0;
}

/** push a window to be at the top position of the stack
 */
int mikaMpDesktop::pushWindow( mikaMpWindow *window )
{
  if (!window)
  {
    #ifdef _DEBUG
      printf("mikaMpWindow::pushWindow error invalid arguments\n");
    #endif
    return 1;
  }
  
  mikaMpWindow *current = windows;
  while(current)
  {
    current->pbo->clearChar = _MIKA_MP_WINDOW_CLEAR;
    current = current->next;
  }
  
  current = windows;
  while(current)
  {
    if (current==window)
    {
      if (current->last)
      {
        current->last->next = current->next;
        if (current->next)
          current->next->last = current->last;
      }
      else
      {
        windows = current->next;
        if (current->next)
          current->next->last = NULL;
      }
      break;
    }
    current = current->next;
  }
  
  current = windows;
  if (current)
  {
    while(current)
    {
      if (current->next==NULL)
      {
        current->next = window;
        window->last = current;
        break;
      }
      current = current->next;
    }
  }
  else
  {
    windows = window;
    window->last = NULL;
  }
  
  window->next = NULL;
  window->pbo->clearChar = 0xff;
  triggerExpose();
  
  return 0;
}

int mikaMpDesktop::mvWindow( mikaMpWindow *window, struct mikaFrame *_frame )
{
  if (!window || !_frame)
  {
    #ifdef _DEBUG
      printf("mikaMpDesktop::mvWindow error invalid arguments\n");
    #endif
    return 1;
  }
  return 0;
}

void mikaMpDesktop::clear( )
{
  pbo->clear();
}

void mikaMpDesktop::triggerExpose( )
{
  SDL_Event event;
  event.type = SDL_VIDEOEXPOSE;
  SDL_PushEvent(&event);
}

#ifdef _STATIC

  #define _TEST_WIDTH 800
  #define _TEST_HEIGHT 600

  void *init(void*data)
  {
    mikaMpDesktop *desktop = (mikaMpDesktop*)data;
    struct mikaFrame frame;
    frame.x = 100;
    frame.y = 100;
    frame.w = 320;
    frame.h = 240;
    mikaMpWindow *window = new mikaMpWindow(&frame);
    desktop->addWindow(window);
    frame.x = 140;
    frame.y = 140;
    frame.w = 320;
    frame.h = 240;
    mikaMpWindow *window2 = new mikaMpWindow(&frame);
    desktop->addWindow(window2);
    frame.x = 180;
    frame.y = 180;
    frame.w = 120;
    frame.h = 120;
    mikaMpWindow *window3 = new mikaMpWindow(&frame);
    desktop->addWindow(window3);
  }

  int main( )
  {
    struct mikaFrame frame;
    frame.x = 0;
    frame.y = 0;
    frame.w = _TEST_WIDTH;
    frame.h = _TEST_HEIGHT;
    
    mikaMessage *message = new mikaMessage();
    message->loadFileToBuffer((unsigned char*)"setup.xml");
    message->loadString(NULL);
    message->generateString(NULL);
    
    mikaMpDesktop *desktop = new mikaMpDesktop(&frame);
    SDL_Quit();
    return 0;
  }
#endif
