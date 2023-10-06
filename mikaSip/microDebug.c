/**  
  Copyright (C) 2012 Milan Kazarka
  milan.kazarka.office@gmail.com
      
  You may distribute under the terms of the Artistic License, 
  as specified in the README file. You can find a copy of the
  license at LICENSE.
*/
#include "microDebug.h"
#include "config.h"

microDebug *mdebug = NULL;

/** custom printf wrapper
 */
void microDebug::print( const char* format, ... )
{    
    pthread_mutex_lock(&mutex);
    va_list args;
    va_start( args, format );
    #ifdef _DEBUG
      vprintf( format, args );
    #endif
    va_end( args );
    pthread_mutex_unlock(&mutex);
}

microDebug::microDebug( )
{
  #ifdef _DEBUG
    printf("microDebug::microDebug\n");
  #endif
  
  delegate = NULL;
  pthread_mutex_init(&mutex,NULL);
}

microDebug::~microDebug( )
{
  #ifdef _DEBUG
    printf("microDebug::~microDebug\n");
  #endif
}

void mprintf( const char *format, ... )
{
  if (!mdebug)
    mdebug = new microDebug();
  va_list args;
  va_start( args, format );
  //mdebug->print(format,args); // todo
  #ifdef _DEBUG
    vprintf(format,args);
  #endif
  va_end(args);
}
