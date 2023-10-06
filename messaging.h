#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"
#define _MIKA_MESSAGING_MODULE_CLOCKTIK 100000

typedef struct mikaMessagingMessage
{
  unsigned char *data;
  TiXmlDocument doc;
} mikaMessagingMessage;

typedef struct mikaMessagingModule
{
  int active;
  pthread_t thread;
  void *(*preRecv)(struct mikaMessagingModule *module, struct mikaMessagingMessage *message);
  void *(*preSend)(struct mikaMessagingModule *module, struct mikaMessagingMessage *message);
  void *(*send)(struct mikaMessagingModule *module, struct mikaMessagingMessage *message);
  void *(*recv)(struct mikaMessagingModule *module, struct mikaMessagingMessage *message);
  void *(*process)(struct mikaMessagingModule *module, struct mikaMessagingMessage *message);
} mikaMessagingModule;

typedef struct mikaMessagingModuleHolder
{
  struct mikaMessagingModule *module;
  struct mikaMessagingModuleHolder *next;
} mikaMessagingModuleHolder;

typedef struct mikaMessaging
{
  pthread_mutex_t mutex;
  struct mikaMessagingModuleHolder *modulesHolder;
} mikaMessaging;

extern struct mikaMessaging *globalMikaMessaging;

int mikaMessagingGlobalInit( );
struct mikaMessagingModule *mikaMessagingModuleInit( struct mikaMessagingModule *module );
struct mikaMessagingMessage *mikaMessagingMessageInit( struct mikaMessagingMessage *message );
int mikaMessagingAddModule( struct mikaMessagingModule *module );
