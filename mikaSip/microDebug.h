/**  
  Copyright (C) 2012 Milan Kazarka
  milan.kazarka.office@gmail.com
      
  You may distribute under the terms of the Artistic License, 
  as specified in the README file. You can find a copy of the
  license at LICENSE.
*/
#ifndef _MICRO_DEBUG_H_
#define _MICRO_DEBUG_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

typedef struct microDebugDelegate_t {
  void	*(*onPrintf)( const char *format, ... );
  void	*(*onMessage)( unsigned char *message );
} microDebugDelegate;

class microDebug {
  public:
    pthread_mutex_t mutex;
    microDebugDelegate *delegate;
  
    microDebug( );
    ~microDebug( );
    
    void print( const char *format, ... );
};

extern microDebug *mdebug;
void mprintf( const char *format, ... );

#endif
