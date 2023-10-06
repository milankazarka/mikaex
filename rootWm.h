#ifndef _ROOT_WM_H_
#define _ROOT_WM_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "rootFrame.h"
#include "rootController.h"

class rootWmWindow {
  public:
    struct rootFrame frame;
    void *lowObject;
  
    rootWmWindow( );
    ~rootWmWindow( );
    int setFrame( struct rootFrame *_frame );
};

typedef struct rootWmWindowHolder {
  rootWmWindow *window;
  rootWmWindowHolder *next;
} rootWmWindoHolder;

class rootWm {
  public:
    struct rootFrame frame;
    struct rootWmWindowHolder *windows;
    
    rootWm( );
    ~rootWm( );
    
    struct rootWmWindowHolder *windowHolderByLowObject( void *lowObject );
    int setFrame( struct rootFrame *_frame );
    int addWindow( rootWmWindow *window );
    int update( );
};

extern rootWm *wmInstance;

#endif
