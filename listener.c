#include "listener.h"
#include <pthread.h>

#define _DEFAULT_PORT 2501
#define _DEFAULT_TIMEOUT 10

listener::listener( )
{
    delegate.onListener = NULL;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (!sockfd)
    {
        printf("listener::listener error, couldn't create socket\n");
    }
    struct sockaddr_in sin;
    memset((char*)&sin,0,sizeof(struct sockaddr_in));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(_DEFAULT_PORT);
    sin.sin_addr.s_addr = INADDR_ANY;
    
    int ret = -1;
    int start = time(NULL);
    
    while((time(NULL)-start<=_DEFAULT_TIMEOUT) && (ret<0))
    {
        printf("bind loop ret(%d)\n",ret);
        ret = bind(sockfd, (struct sockaddr *)&sin, sizeof(sin));
        if (ret<0)
            sleep(1);
    }
    
    if (ret<0)
    {
        printf("binding socket failed\n");
    }
    
    ret = listen(sockfd,10);
 
}

listener::~listener( )
{
}

void *loopThread(void*data)
{
    if (!data)
        return NULL;
    listener *l = (listener*)data;
    l->loopThreaded();
}
 
#define TMPBUFFER 256

void listener::loopThreaded( )
{
    unsigned char tmp[TMPBUFFER];
    struct sockaddr_storage cli_addr;
    struct mikaConnection *client = NULL;
    socklen_t clilen;
    clilen = sizeof(cli_addr);
    while(1)
    {
        int newfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        printf("listener::loop connection\n");
        int red = read(newfd,(unsigned char*)tmp,TMPBUFFER-1);
        if (red>0)
        {
            printf("received(%s)\n",(char*)tmp);
            if (tmp[strlen((char*)tmp)-1]=='\n')
                tmp[strlen((char*)tmp)-1] = 0x00;
            int nid = atoi((char*)tmp);
            printf("\tnid(%d)\n",nid);
            if (delegate.onListener)
            {
                delegate.onListener(nid);
            }
        }
        close(newfd);
    }
}

void listener::loop( )
{
    pthread_t t;
    pthread_create(&t,NULL,loopThread,this);
}
