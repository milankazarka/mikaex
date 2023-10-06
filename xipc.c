#include "xipc.h"
#include "config.h"

#define _STATIC_NO
#ifndef _DEBUG
  #define _DEBUG
#endif

struct ipc_t *ipcInit( struct ipc_t *ipc )
{
  #ifdef _DEBUG
    printf("ipcInit\n");
  #endif
  if (!ipc)
    ipc = (struct ipc_t*)malloc(sizeof(struct ipc_t));	
  ipc->servername[0] = 0x00;
  ipc->bufferin[0] = 0x00;
  ipc->bufferout[0] = 0x00;
  ipc->modules = NULL;
  return ipc;
}

int ipcModuleRegister( struct ipc_t *ipc, struct ipc_module_t *module )
{
  if (!ipc || !module)
  {
    #ifdef _DEBUG
      printf("ipcModuleRegister error invalid arguments\n");
    #endif
    return 1;
  }
  
  module->next = ipc->modules;
  ipc->modules = module;
  
  return 0;
}

/** read message from network socket
 */
int ipcConnectionGet( struct ipc_t *ipc, int connection )
{
  #ifdef _DEBUG
    printf("ipcConnectionGet\n");
  #endif
  
  if (!ipc)
  {
    #ifdef _DEBUG
      printf("ipcConnectionGet error\n");
    #endif
    return 1;
  }
  
  int red = 0;
  strcpy((char*)ipc->bufferin,(char*)"");
  unsigned char tmp[_IPC_BUFFER];
  
  while(1)
  {
    red = read(connection,(unsigned char*)tmp,_IPC_BUFFER-1);
    if (red<1)
      break;
    tmp[red] = 0x00;
    #ifdef _DEBUG
      printf("ipcConnectionGet tmp(%s)\n",(char*)tmp);
    #endif
    strcat((char*)ipc->bufferin,(char*)tmp);
    if (strstr((char*)tmp,"\n"))
    //if (strstr((char*)tmp,"|"))
    {
      break;
    }
  }
  
  #ifdef _DEBUG
    printf("ipcConnectionGet finished\n");
  #endif
  
  return 0;
}

/** put / reply a message
 */
int ipcConnectionPut( struct ipc_t *ipc, int connection )
{
  #ifdef _DEBUG
    printf("ipcConnectionPut\n");
  #endif

  if (!ipc)
  {
    #ifdef _DEBUG
      printf("ipcConnectionPut error\n");
    #endif
    return 1;
  }
  
  int len = strlen((char*)ipc->bufferout);
  if (len>0)
    write(connection,(char*)ipc->bufferout,len);
  ipc->bufferout[0] = 0x00;
  
  return 0;
}

/** handle an incomming connection
 */
int ipcHandleConnection( struct ipc_t *ipc, int connection )
{
  #ifdef _DEBUG
    printf("ipcHandleConnection\n");
  #endif
  
  if (!ipc)
  {
    #ifdef _DEBUG
      printf("ipcHandleConnection error invalid arguments\n");
    #endif
    return 1;
  }
  
  ipc->bufferin[0] = 0x00;
  ipc->bufferout[0] = 0x00;
  
  if (ipcConnectionGet(ipc,connection)!=0)
  {
    #ifdef _DEBUG
      printf("ipcHandleConnection error couldn't get request\n");
    #endif
    return 1;
  }
  
  char *section = NULL;
  unsigned char command[256];
  unsigned char message[256];
  section = strstr((char*)ipc->bufferin,"|");
  if (section)
  {
    strncpy((char*)command,(char*)ipc->bufferin,section-(char*)ipc->bufferin);
    command[section-(char*)ipc->bufferin] = 0x00;
    
    struct ipc_module_t *current = ipc->modules;
    while(current)
    {
      if (strlen((char*)command)==strlen((char*)current->keyword))
      {
        if (strcmp((char*)command,(char*)current->keyword)==0)
        {
          if (current->oncommand)
          {
            #ifdef _DEBUG
              printf("ipcHandleConnection section(%s)\n",
                section);
            #endif
            
            sprintf((char*)message,"%s",section+1);
            if (message[strlen((char*)message)-1]=='\n')
            {
              #ifdef _DEBUG
                printf("ipcHandleConnection trim message\n");
              #endif
              message[strlen((char*)message)-1] = 0x00;
            }
            #ifdef _DEBUG
              printf("ipcHandleConnection message(%s) len(%d)\n",
                (char*)message,strlen((char*)message));
            #endif
            
            current->oncommand(ipc,(unsigned char*)message);
            break;
          }
        }
      }
      current = current->next;
    }
  }
  
  strcpy((char*)ipc->bufferout,"ok\n");
  if (ipcConnectionPut(ipc,connection)!=0)
  {
    #ifdef _DEBUG
      printf("ipcHandleConnection error couldn't reply to request\n");
    #endif
    return 1;
  }
  
  close(connection);
  
  return 0;
}

/** listen loop
 */
void *ipcListenLoop(void*data)
{
  if (!data)
  {
    #ifdef _DEBUG
      printf("ipcListenLoop error invalid arguments\n");
    #endif
    return NULL;
  }
  struct ipc_t *ipc = (struct ipc_t*)data;
  
  struct sockaddr_storage cli_addr;
  socklen_t clilen;
  clilen = sizeof(cli_addr);
  while(1)
  {
    int newfd = accept(ipc->listensocket, (struct sockaddr*)&cli_addr,&clilen);
    #ifdef _DEBUG
      printf("ipcListenLoop connection\n");
    #endif
    ipcHandleConnection(ipc,newfd);
  }
}

/** send a message of a type defined in module
 */
int ipcSendMessage( struct ipc_t *ipc, struct ipc_module_t *module, unsigned char *message )
{
  if (!ipc || !module || !message)
  {
    #ifdef _DEBUG
      printf("ipcSendMessage error invalid arguments\n");
    #endif
    return 1;
  }
  
  #ifdef _DEBUG
    printf("ipcSendMessage server(%s) port(%d) message(%s)\n",
      (char*)ipc->servername,ipc->serverport,(char*)message);
  #endif
  
  struct hostent *host;
  struct sockaddr_in server_addr;
  unsigned char completeMessage[1024];
  
  memset(&server_addr,0,sizeof(server_addr));
  host = gethostbyname((char*)ipc->servername);
  if ((ipc->sendsocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    #ifdef _DEBUG
      printf("ipcSendMessage coulnd't create socket\n");
    #endif
    close(ipc->sendsocket);
    return 1;
  }
  
  bcopy(
    (char*)host->h_addr,
    (char*)&server_addr.sin_addr.s_addr,
    host->h_length
  );
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(ipc->serverport);
  
  if (connect(ipc->sendsocket, (struct sockaddr *)&server_addr,
    sizeof(struct sockaddr)) == -1)
  {
    #ifdef _DEBUG
      printf("ipcSendMessage error couldn't connect to listener\n");
    #endif
    close(ipc->sendsocket);
    return 1;
  }
  
  sprintf((char*)completeMessage,"%s|%s\n",
    (char*)module->keyword,(char*)message);
  strcpy((char*)ipc->bufferout,(char*)completeMessage);  
  ipcConnectionPut(ipc,ipc->sendsocket); // send request
  ipcConnectionGet(ipc,ipc->sendsocket); // receive reply
  
  close(ipc->sendsocket);
  
  return 0;
}

/** create a listening server
 */
int ipcRun( struct ipc_t *ipc )
{
  if (!ipc)
  {
    #ifdef _DEBUG
      printf("ipcRun error invalid arguments\n");
    #endif
    return 1;
  }
  
  ipc->listensocket = socket(AF_INET, SOCK_STREAM, 0);
  if (ipc->listensocket<0)
  {
    #ifdef _DEBUG
      printf("ipcRun couldn't create socket\n");
    #endif
    return 1;
  }
  
  struct sockaddr_in sin;
  int ret, n;
  memset((char*)&sin,0,sizeof(struct sockaddr_in));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(ipc->listenport);
  sin.sin_addr.s_addr = INADDR_ANY;
  
  for(n = 0; n < _BIND_TRIES; n++)
  {
    ret = bind(ipc->listensocket,(struct sockaddr*)&sin,sizeof(sin));
    if (ret>-1)
      break;
    else
    {
      #ifdef _DEBUG
        printf("ipcRun binding\n");
      #endif
      sleep(1);
    }
  }
  
  if (ret<0)
  {
    #ifdef _DEBUG
      printf("ipcRun couldn't bind socket\n");
    #endif
    return 1;
  }
  else
  {
    #ifdef _DEBUG
      printf("ipcRun connected\n");
    #endif
  }
  
  if (listen(ipc->listensocket,10)<0)
  {
    #ifdef _DEBUG
      printf("ipcRun couldn't listen on socket\n");
    #endif
    return 1;
  }

  pthread_t t;
  pthread_create(&t,NULL,ipcListenLoop,ipc);
  
  return 0;
}

#ifdef _STATIC
  int main( )
  {
    struct ipc_t *ipc = NULL;
    ipc = ipcInit(NULL);
    
    ipc->listenport = 46000;
    ipcRun(ipc);
    while(1)
    {
      sleep(1);
    }
    
    return 0;
  }
#endif
