/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef __CSA_H__
#define __CSA_H_

#include "system.h"

#define CSA_KERNEL_MAXTASKS	100

#define CSA_STATE_INIT		0
#define CSA_STATE_KLOCKABLE	1	// lockable by th ekernel
#define CSA_STATE_KLOCKED	2
#define CSA_STATE_TERM		3
#define CSA_STATE_WAIT		4
#define CSA_STATE_KUNLOCKED	5

#define CSA_CLOCK_THREAD	1	// timeout
#define CSA_CLOCK_KERNEL	1

#define CSA_SCHED_KERNEL	0	// csa's need to be enabled by the kernel to run
#define CSA_SCHED_FREE		1	// csa's with simpler kernel control

#define CSA_IDLEN		3

typedef struct trop_CsaMsg
{
  char			id[CSA_IDLEN];
  void			*data;
  int			datalen;
} trop_CsaMsg;

typedef struct trop_CsaKernel
{
  struct trop_Atom	atom;
  struct trop_Csa	*csa;
  struct trop_CsaTask	*task;
} trop_CsaKernel;

typedef struct trop_CsaTask
{
  int 			state;
  void			*(*call)(void*data);
  void			*data;
  struct trop_CsaTask	*next;
} trop_CsaTask;

typedef struct trop_Csa
{
  char			id[CSA_IDLEN];
  int			state;
  int			sched;
  struct trop_CsaKernel	*kernel;
  struct trop_Atom	atom;
  struct trop_Mutex	mutex;
  struct trop_CsaTask	*task;
  struct trop_Csa	*next;
} trop_Csa;

void _klock( );
void _kunlock( );
void _kernel_thread_clock( );
void *_trop_csa_run(void*data);
int trop_CsaInit( struct trop_Csa *csa );
int trop_CsaInitEx( struct trop_CsaKernel *kernel, struct trop_Csa *csa );
int trop_CsaUnlock( struct trop_Csa *csa );
int _trop_CsaUnlock( struct trop_Csa *csa, BOOL lock );
int trop_CsaTryLock( struct trop_Csa *csa );
struct trop_Csa *new_TropCsa( );
int trop_CsaKernelInit( struct trop_CsaKernel *kernel );
struct trop_CsaKernel *new_TropCsaKernel( );
void *_kernel_threads( void *data );
int trop_CsaKernelStart( struct trop_CsaKernel *kernel );
int trop_CsaKernelTaskAdd( struct trop_CsaKernel *kernel, struct trop_CsaTask *task );
struct trop_CsaTask *new_tropCsaTask( );
int trop_CsaTaskInit( struct trop_CsaTask *task, void *(*call)(void*data), void *data );
int trop_CsaSetID( struct trop_Csa *csa, char *id );
struct trop_Csa *trop_CsaKernelIsCSAID( struct trop_CsaKernel *kernel, char *id );

struct trop_Mutex kmutex;

#endif
