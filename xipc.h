#ifndef _XIPC_H_
#define _XIPC_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <netdb.h>

#include <pthread.h>

#define _BIND_TRIES 10
#define _IPC_BUFFER 1024

typedef struct ipc_module_t
{
  unsigned char keyword[256];
  void *(*oncommand)( void *ipc, unsigned char *command );
  struct ipc_module_t *next;
} ipc_module_t;

typedef struct ipc_t
{
  int listenport;
  int serverport;
  unsigned char servername[128];
  int listensocket;
  int sendsocket;
  unsigned char bufferin[_IPC_BUFFER];
  unsigned char bufferout[_IPC_BUFFER];
  struct ipc_module_t *modules;
} ipc_t;

struct ipc_t *ipcInit( struct ipc_t *ipc );
int ipcModuleRegister( struct ipc_t *ipc, struct ipc_module_t *module );
int ipcRun( struct ipc_t *ipc );
void *ipcListenLoop(void*data);
int ipcHandleConnection( struct ipc_t *ipc, int connection );
int ipcSendMessage( struct ipc_t *ipc, struct ipc_module_t *module, unsigned char *message );

#endif
