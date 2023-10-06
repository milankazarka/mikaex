#include "messaging.h"
#include "config.h"

struct mikaMessaging *globalMikaMessaging = NULL;

/** process the received data before sending to the module speciffic handler
 */
void *mikaMessagingPreRecv( struct mikaMessagingModule *module, struct mikaMessagingMessage *message )
{
  if (!module || !message)
  {
    #ifdef _DEBUG
      printf("mikaMessagingPreRecv error invalid arguments\n");
    #endif
    return NULL;
  }
  if (!message->data)
  {
    #ifdef _DEBUG
      printf("mikaMessagingPreRecv error no data\n");
    #endif
    return NULL;
  }
  message->doc.Parse((const char*)message->data);
}

int mikaMessagingGlobalInit( )
{
  if (globalMikaMessaging)
  {
    #ifdef _DEBUG
      printf("mikaMessagingGlobalInit error messaging already initialized\n");
    #endif
    return 1;
  }
  globalMikaMessaging = (struct mikaMessaging*)malloc(sizeof(struct mikaMessaging));
  if (!globalMikaMessaging)
  {
    #ifdef _DEBUG
      printf("mikaMessagingGlobalInit error couldn't allocate globalMikaMessaging\n");
    #endif
    return 1;
  }
  globalMikaMessaging->modulesHolder = NULL;
  pthread_mutex_init(&globalMikaMessaging->mutex,NULL);
  return 0;
}

void *mikaMessagingModuleThread( void *data )
{
  if (!data)
    return NULL;
  struct mikaMessagingModule *module = (struct mikaMessagingModule*)data;
  while(1)
  {
    pthread_mutex_lock(&globalMikaMessaging->mutex);
    if (module->active==0)
    {
      free(module);
      pthread_mutex_unlock(&globalMikaMessaging->mutex);
      break;
    }
    pthread_mutex_unlock(&globalMikaMessaging->mutex);
    usleep(_MIKA_MESSAGING_MODULE_CLOCKTIK);
  }
}

/** deactivate a module, after which the module thread releases it's memmory
 *
 */
int mikaMessagingModuleDeactivate( struct mikaMessagingModule *module )
{
  if (!module)
  {
    #ifdef _DEBUG
      printf("mikaMessagingModuleDeactivate error invalid arguments\n");
    #endif
    return 1;
  }
  pthread_mutex_lock(&globalMikaMessaging->mutex);
  struct mikaMessagingModuleHolder *current = globalMikaMessaging->modulesHolder;
  struct mikaMessagingModuleHolder *last = NULL;
  while(current)
  {
    if (current->module==module)
    {
      if (last==NULL)
        globalMikaMessaging->modulesHolder = current->next;
      else
      {
        last->next = current->next;
      }
    }
    current = current->next;
  }
  module->active = 0;
  pthread_mutex_unlock(&globalMikaMessaging->mutex);
  return 0;
}

/** register a messaging module
 */
int mikaMessagingAddModule( struct mikaMessagingModule *module )
{
  if (!module)
  {
    #ifdef _DEBUG
      printf("mikaMessagingAddModule error invalid arguments\n");
    #endif
    return 1;
  }
  
  struct mikaMessagingModuleHolder *holder =
    (struct mikaMessagingModuleHolder*)malloc(sizeof(struct mikaMessagingModuleHolder));
  
  pthread_mutex_lock(&globalMikaMessaging->mutex);
  holder->module = module;
  holder->next = globalMikaMessaging->modulesHolder;
  globalMikaMessaging->modulesHolder = holder;
  pthread_create(&module->thread,NULL,mikaMessagingModuleThread,NULL);
  pthread_mutex_unlock(&globalMikaMessaging->mutex); 
   
  return 0;
}

/** initializing a messaging module
 */
struct mikaMessagingModule *mikaMessagingModuleInit( struct mikaMessagingModule *module )
{
  if (!module)
    module = (struct mikaMessagingModule*)malloc(sizeof(struct mikaMessagingModule));
  module->active = 1;
  module->send = NULL;
  module->recv = NULL;
  module->process = NULL;
  return module;
}

struct mikaMessagingMessage *mikaMessagingMessageInit( struct mikaMessagingMessage *message )
{
  if (!message)
    message = (struct mikaMessagingMessage*)malloc(sizeof(struct mikaMessagingMessage));
  message->data = NULL;
  return message;
}
