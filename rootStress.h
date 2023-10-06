/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _ROOT_STRESS_H_
#define _ROOT_STRESS_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

typedef struct rootStress
{
  int loopTime;
  pthread_t thread;
} rootStress;

struct rootStress *rootStressNew( );
int rootStressRelease( struct rootStress *myStress );
int rootStressStart( struct rootStress *myStress );

#endif
