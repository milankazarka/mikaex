#include "simpleServer.h"
#define _STATIC
#define _PORT 21000
#define _DEBUG
#define _BIND_TRIES 20
#define _DROPDOWN_FILE "/server/apacheRoot/htdocs/orders.html"

int main( )
{
  int listensocket;
  int newsocket;
  unsigned char buffer[512];
  
  listensocket = socket(AF_INET, SOCK_STREAM, 0);
  if (listensocket<0)
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
  sin.sin_port = htons(_PORT);
  sin.sin_addr.s_addr = INADDR_ANY;
  
  for(n = 0; n < _BIND_TRIES; n++)
  {
    ret = bind(listensocket,(struct sockaddr*)&sin,sizeof(sin));
    if (ret>-1)
    {
      break;
    }
    else
    {
      #ifdef _DEBUG
        printf("simpleServer binding\n");
      #endif
      sleep(1);
    }
  }
  
  if (ret<0)
  {
    #ifdef _DEBUG
      printf("simpleServer couldn't bind socket\n");
    #endif
    return 1;
  }
  else
  {
    #ifdef _DEBUG
      printf("simpleServer connected\n");
    #endif
  }
  
  if (listen(listensocket,10)<0)
  {
    #ifdef _DEBUG
      printf("simpleServer couldn't listen on socket\n");
    #endif
    return 1;
  }

  #ifdef _DEBUG
    printf("simpleServer listening\n");
  #endif

  struct sockaddr_storage cli_addr;
  socklen_t clilen;
  clilen = sizeof(cli_addr);
  
  while(1)
  {
    newsocket = accept(listensocket, (struct sockaddr*)&cli_addr,&clilen);
    
    int red = 0;
    strcpy((char*)buffer,(char*)"");
    unsigned char tmp[512];
  
    while(1)
    {
      red = read(newsocket,(unsigned char*)tmp,511);
      if (red<1)
        break;
      tmp[red] = 0x00;
      #ifdef _DEBUG
        printf("simpleServer tmp(%s)\n",(char*)tmp);
      #endif
      strcat((char*)buffer,(char*)tmp);
      if (strstr((char*)tmp,"\n"))
      {
        break;
      }
    }
    
    if (strlen((char*)buffer)>0)
    {
      #ifdef _DEBUG
        printf("simpleServer bytes_in(%s)\n",(char*)buffer);
      #endif
      
      if (buffer[strlen((char*)buffer)-1]=='\n')
      {
        buffer[strlen((char*)buffer)-1] = 0x00;
      }
      
      struct simpleOrder *order = (struct simpleOrder*)malloc(sizeof(struct simpleOrder));
      strcpy((char*)order->name,(char*)buffer);
      order->next = orders;
      orders = order;
      
      FILE *out = fopen(_DROPDOWN_FILE,"w");
      if (out)
      {
        fwrite("<Orders>\n",strlen("<Orders>\n"),1,out);
        struct simpleOrder *current = orders;
        while(current)
        {
          fwrite("\t<Order state=\"new\" descr=\"",strlen("\t<Order state=\"new\" descr=\""),1,out);
          fwrite((char*)current->name,strlen((char*)current->name),1,out);
          fwrite("\"></Order>\n",strlen("\"></Order>\n"),1,out);
          current = current->next;
        }
        fwrite("</Orders>\n",strlen("</Orders>\n"),1,out);
        fclose(out);
      }
    }
    
    close(newsocket);
  }
  
  return 0;
}
