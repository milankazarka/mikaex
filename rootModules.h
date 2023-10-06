/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _ROOT_MODULES_H_
#define _ROOT_MODULES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/utsname.h>
#include <semaphore.h>
#include <dirent.h>

typedef struct rootModules
{
} rootModules;

struct rootModules *rootModulesNew( );
int rootModulesRun( struct rootModules *myModules );

#endif
