#ifndef _ROOT_WM_INTERFACE_H_
#define _ROOT_WM_INTERFACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "rootFrame.h"

#ifdef __cplusplus
  extern "C"
#endif
int rootWmInterfaceInit( );
#ifdef __cplusplus
  extern "C"
#endif
int rootWmGlobalInit( );
#ifdef __cplusplus
  extern "C"
#endif
int rootWmInterfaceSetFrame( struct rootFrame *frame );
#ifdef __cplusplus
  extern "C"
#endif
int rootWmInterfaceAddWindow( void *lowObject );
#ifdef __cplusplus
  extern "C"
#endif
int rootWmInterfaceMvWindow( void *lowObject, struct rootFrame *frame );

#endif
