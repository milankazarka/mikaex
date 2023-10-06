#ifndef _MIKA_MP_DESKTOP_H_
#define _MIKA_MP_DESKTOP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>

#include <cairo/cairo.h>
#include <SDL.h>
#include "mikaFrame.h"
#include "mikaPBO.h"
#include "mikaMpSched.h"
#include "mikaMpDesktopServer.h"

enum _MIKA_MP_WINDOW_STATE {
  _MIKA_MP_WINDOW_STATE_NORMAL,
  _MIKA_MP_WINDOW_STATE_ICONIFIED,
  _MIKA_MP_WINDOW_STATE_FULLSCREEN
};

class mikaMpWindow {
  public:
    struct mikaFrame frame;
    mikaMpWindow *last;
    mikaMpWindow *next;
    mikaPBO *pbo;
    int state; 
     
    mikaMpWindow( struct mikaFrame *_frame );
    ~mikaMpWindow( );
    void eventExpose( );
};

enum _MIKA_MP_ACTION {
  _MIKA_MP_ACTION_EXPOSE,
  _MIKA_MP_ACTION_BUTTONUP,
  _MIKA_MP_ACTION_BUTTONDOWN,
  _MIKA_MP_ACTION_BUTTONMOTION,
  // multi touch events
  _MIKA_MP_ACTION_FINGERDOWN,
  _MIKA_MP_ACTION_FINGERUP
};

/** used in case we pass the MpDesktop objects somewhere
 *  where they must be identified
 */
enum _MIKA_MP_ACTION_DATA {
  _MIKA_MP_ACTION_DATA_DESKTOP,
  _MIKA_MP_ACTION_DATA_WINDOW,
  _MIKA_MP_ACTION_DATA_EVENT
};

class mikaMpAction {
  public:
    int type;
    int x;
    int y;
    mikaMpAction *last;
    mikaMpAction *next;
    mikaMpWindow *window;
    
    mikaMpAction( );
    ~mikaMpAction( );
};

enum _MIKA_MP_DESKTOP_ATOM_TYPE {
  _MIKA_MP_DESKTOP_ATOM_TYPE_ICON,
  _MIKA_MP_DESKTOP_ATOM_TYPE_OVERLAY,
  _MIKA_MP_DESKTOP_ATOM_TYPE_DYNAMIC
};

#define _MIKA_MP_DESKTOP_ATOM_TYPE_DEFAULT _MIKA_MP_DESKTOP_ATOM_TYPE_OVERLAY

class mikaMpDesktopAtom {
  public:
    struct mikaFrame frame;
    mikaMpDesktopAtom *next;
    int type;
    
    mikaMpDesktopAtom( struct mikaFrame *_frame );
    ~mikaMpDesktopAtom( );
    void eventExpose( );
};

class mikaMpDesktop {
  public:
    struct mikaFrame frame;
    SDL_Surface *surface;
    cairo_t *cairo;
    cairo_surface_t *cairo_surface;
    pthread_t events_thread;
    mikaMpWindow *windows;
    mikaMpDesktopAtom *atoms;
    pthread_mutex_t mutex;
    mikaPBO *pbo;
    mikaMpAction *actions;
    
    mikaMpDesktop( struct mikaFrame *_frame );
    ~mikaMpDesktop( );
    
    void lock( );
    void unlock( );
    int eventLoop( );
    int eventExpose( );
    void drawTest( );
    int addWindow( mikaMpWindow *window );
    int rmWindow( mikaMpWindow *window );
    int mvWindow( mikaMpWindow *window, struct mikaFrame *_frame );
    int pushWindow( mikaMpWindow *window );
    void clear( );
    void onButtonDown( int x, int y);
    void onButtonUp( int x, int y );
    void onMotion( int x, int y );
    mikaMpWindow *windowAtPosition( int x, int y );
    void triggerExpose( );
    int initGL( );
    int drawGL( );
};

#endif
