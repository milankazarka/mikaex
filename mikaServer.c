/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "mikaServer.h"
#include "mikaServerMod.h"
#include "mikaPosMod.h"
#include "mikaConfigServerMod.h"
#include "mikaServerPosDelegate.h"
#include "config.h"

#define _STATIC

struct mikaConnection *clientConnections[CONNECTIONS];
unsigned char *msgEnding = (unsigned char*)MSGENDING;

int mikaMemLock( struct mikaMem *mem )
{
    if (!mem)
        return 1;
    
    pthread_mutex_trylock(&mem->id);    
        
    return 0;
}

int mikaMemUnlock( struct mikaMem *mem )
{
    if (!mem)
        return 1;
    
    pthread_mutex_unlock(&mem->id);
    
    return 0;
}

int _mikaMemInitValues( struct mikaMem *mem )
{
    if (!mem)
        return 1;
    
    pthread_mutex_init(&mem->id,NULL);    
        
    return 0;
}

struct mikaMem *mikaMemAlloc( struct mikaMem *mem )
{
    if (!mem)
    {
        mem = (struct mikaMem*)malloc(sizeof(struct mikaMem));
        _mikaMemInitValues(mem);
    }
    return mem;
}

struct mikaMem *mikaMemInit( struct mikaMem *mem )
{
    if (!mem)
    {
        mem = mikaMemAlloc(NULL);
    }
    else
    {
        _mikaMemInitValues(mem);
    }
    return mem;
}

int _mikaMsgClearBuffers( struct mikaMsg *msg )
{
    if (!msg)
        return 1;
    
    if (msg->inBuffer) free(msg->inBuffer);
    if (msg->outBuffer) free(msg->outBuffer);
    
    msg->inBuffer = NULL;
    msg->outBuffer = NULL;
    
    return 0;
}

struct mikaMsg *mikaMsgAlloc( struct mikaMsg *msg )
{
    if (!msg)
    {
        msg = (struct mikaMsg*)malloc(sizeof(struct mikaMsg));
        msg->inBuffer = NULL;
        msg->outBuffer = NULL;
        _mikaMsgClearBuffers(msg);
    }
    
    return msg;
}

struct mikaMsg *mikaMsgInit( struct mikaMsg *msg )
{
    if (!msg)
    {
        msg = mikaMsgAlloc(NULL);
    }
    else
    {
        _mikaMsgClearBuffers(msg);
    }
    
    return msg;
}

struct mikaMsg *mikaMsgReinit( struct mikaMsg *msg )
{
    return mikaMsgInit(msg);
}

int mikaMsgAllocBuffers( struct mikaMsg *msg )
{
    if (!msg)
        return 1;
    
    if (!msg->inBuffer)		msg->inBuffer = (unsigned char*)malloc(BUFFER);
    if (!msg->outBuffer)	msg->outBuffer = (unsigned char*)malloc(BUFFER);
    
    return 0;
}

int mikaMsgDeallocBuffers( struct mikaMsg *msg )
{
    if (!msg)
        return 1;
    
    if (msg->inBuffer)
    {
        free(msg->inBuffer);    
        msg->inBuffer = NULL;
    }
    
    if (msg->outBuffer) 
    {
        free(msg->outBuffer);   
        msg->outBuffer = NULL;
    }
        
    return 0;
}

int mikaConnectionListen( struct mikaConnection *conn )
{   
    if (!conn)
        return 1;
    
    conn->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (conn->sockfd < 0)
      return 1;
    
    struct sockaddr_in sin;
    memset((char*)&sin,0,sizeof(struct sockaddr_in));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORT);
    sin.sin_addr.s_addr = INADDR_ANY;
    
    int ret = -1;
    int start = time(NULL);
    
    while((time(NULL)-start<=TIMEOUT) && (ret<0))
    {
        #ifdef _DEBUG
            printf("mikaConnectionListen bind loop ret(%d)\n",ret);
        #endif
        ret = bind(conn->sockfd, (struct sockaddr *)&sin, sizeof(sin));
        if (ret<0)
            sleep(1);
    }
    
    if (ret<0)
    {
        printf("binding socket failed\n");
        return 1;
    }
    
    ret = listen(conn->sockfd,10);
    
    if (ret<0)
    {
        printf("listening on socket failed\n");
        return 1;
    }
    
    return 0;
}

int mikaServerRequestLoop( struct mikaServer *server )
{
    if (!server)
        return 1;
    
    struct sockaddr_storage cli_addr;
    struct mikaConnection *client = NULL;
    socklen_t clilen;
    clilen = sizeof(cli_addr);
    while(1)
    {
        int newfd = accept(server->connection->sockfd, (struct sockaddr *)&cli_addr, &clilen);
        #ifdef _DEBUG
            printf("mikaServerRequestLoop connection\n");
        #endif
        client = mikaConnectionClientRegister(server,newfd);
        if (client)
        {
            mikaConnectionClientThreadStart(server,client);
        }
    }
    
    return 0;
}

void *_mika_call_connection_processing_general( struct mikaServer *server, struct mikaConnection *connection )
{
    if (!server || !connection)
        return NULL;   
    
    #ifdef _DEBUG
        printf("_mika_call_connection_processing_general\n");
    #endif
    
    return NULL;
}

/**
 * thread to be ran after we accept a connection
 * it calls the processing function and unregisters
 * the connection in the end
 */
#define _THREADED_CONNECTION_NO
void *_mika_connection_thread( void *data )
{
    #ifdef _DEBUG
        printf("_mika_connection_thread\n");
    #endif
    struct mikaConnectionThread *threadData = (struct mikaConnectionThread*)data;
    
    if (threadData->server->callConnectionProcessing)
    {
        threadData->server->callConnectionProcessing(threadData->server,threadData->connection);
    }
    
    close(threadData->connection->sockfd);
    mikaMemLock(threadData->server->mem);
    mikaConnectionClientUnregister(threadData->server,threadData->connection);
    mikaMemUnlock(threadData->server->mem);
    free(threadData);
    
    #ifdef _THREADED_CONNECTION
        pthread_exit(NULL);
    #endif
    
    #ifdef _DEBUG
        printf("_mika_connection_thread finish\n");
    #endif
    
    return NULL;
}

int mikaConnectionClientThreadStart( struct mikaServer *server, struct mikaConnection *connection )
{
    if (!server || !connection)
        return 1;
    
    #ifdef _DEBUG
        printf("mikaConnectionClientThreadStart\n");
    #endif
    
    struct mikaConnectionThread *threadData = (struct mikaConnectionThread*)malloc(sizeof(struct mikaConnectionThread));
    threadData->server = server;
    threadData->connection = connection;
    #ifdef _THREADED_CONNECTION
        pthread_create(&threadData->id,NULL,_mika_connection_thread,threadData);
    #else
        _mika_connection_thread(threadData);
    #endif
    
    #ifdef _DEBUG
        printf("mikaConnectionClientThreadStart finish thread(%d)\n",threadData->id);
    #endif    
        
    return 0;
}

struct mikaConnection *mikaConnectionClientRegister( struct mikaServer *server, int newfd )
{
    static int n;
    static struct mikaConnection *client;

    if (!server)
        return NULL;
    
    #ifdef _DEBUG
        printf("mikaConnectionClientRegister\n");
    #endif
    
    while(1)
    {
        mikaMemLock(server->mem);
        for(n = 0; n < CONNECTIONS; n++)
        {
            client = clientConnections[n];
            if (client->active==0)
            {
                client->active = 1;
                client->sockfd = newfd;
                mikaMsgAllocBuffers(client->msg);
                
                mikaMemUnlock(server->mem);
                
                #ifdef _DEBUG
                    printf("mikaConnectionClientRegister hit free connection\n");
                #endif
                return client;
            }
        }
        mikaMemUnlock(server->mem);
        #ifdef _DEBUG
            printf("mikaConnectionClientRegister no free connections, retrying\n");
        #endif
        usleep(RETRYDELAY);
    }    
        
    return NULL;
}

int mikaConnectionClientUnregister( struct mikaServer *server, struct mikaConnection *connection )
{
    if (!server || !connection)
        return 1;
    
    #ifdef _DEBUG
        printf("mikaConnectionClientUnregister\n");
    #endif
    
    connection->active = 0;
    mikaMsgDeallocBuffers(connection->msg);
    close(connection->sockfd);
    
    return 0;
}

/**
 * receive a request over a connection
 */
int mikaConnectionClientGet( struct mikaConnection *connection )
{
    int red = 0;
    unsigned char tmp[TMPBUFFER];
    
    if (!connection)
        return 1;
    
    #ifdef _DEBUG
        printf("mikaConnectionClientGet\n");
    #endif
    
    strcpy((char*)connection->msg->inBuffer,"");
        
    while(1)
    {
        red = read(connection->sockfd,(unsigned char*)tmp,TMPBUFFER-1);
        if (red<1)
            break;
        
        tmp[red] = 0x00;
                
        #ifdef DEBUG
            printf("mikaConnectionClientGet tmp(%s)\n",(unsigned char*)tmp);
        #endif
        strcat((char*)connection->msg->inBuffer,(char*)tmp);
        if (
            strstr((char*)tmp,MSGENDING) ||
            (
                tmp[0] == '*' &&
                tmp[strlen((char*)tmp)-1] == '*'
            )
            )
        {
            break;
        }   
    }
        
    return 0;
}

int mikaConnectionClientPut( struct mikaConnection *connection )
{
    if (!connection)
        return 1;
    
    #ifdef _DEBUG
        if (connection->msg->outBuffer)
            printf("mikaConnectionClientPut msg(%s)\n",
                (char*)connection->msg->outBuffer);
    #endif
    
    int len = strlen((char*)connection->msg->outBuffer);
    
    if (len>0)
        write(connection->sockfd,connection->msg->outBuffer,len);
    
    return 0;
}

int _mikaConnectionInitValues( struct mikaConnection *connection )
{   
    if (!connection)
        return 1;
    connection->active = 0;
    connection->sockfd = -1;
    connection->msg = mikaMsgInit(NULL);
    connection->dom = NULL;
    return 0;
}

struct mikaConnection *mikaConnectionAlloc( struct mikaConnection *connection )
{
    connection = (struct mikaConnection*)malloc(sizeof(struct mikaConnection));
    _mikaConnectionInitValues(connection);
    return connection;
}

struct mikaConnection *mikaConnectionInit( struct mikaConnection *connection )
{
    if (!connection)
    {
        connection = mikaConnectionAlloc(NULL);
    }
    else
    {
        _mikaConnectionInitValues(connection);
    }
    return connection;
}

/** open a socket connection
 */
int mikaConnectionOpen( struct mikaConnection *connection, unsigned char *server, int port )
{
    if (!connection || !server)
        return 1;
    
    struct hostent *host;
    struct sockaddr_in server_addr;  
    
    #ifdef _DEBUG
        printf("mikaConnectionOpen\n");
    #endif
                             
    host = gethostbyname((char*)server);
                                     
    if ((connection->sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    {
        #ifdef _DEBUG
            printf("mikaConnectionOpen couldn't create socket\n");
        #endif
        return 1;
    }
                                                                             
    server_addr.sin_family = AF_INET;     
    server_addr.sin_port = htons(port);   
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(server_addr.sin_zero),8); 
                                                                                                             
    if (connect(connection->sockfd, (struct sockaddr *)&server_addr,
        sizeof(struct sockaddr)) == -1) 
    {
        #ifdef _DEBUG
            printf("mikaConnectionOpen couldn't open\n");
        #endif
        return 1;
    }                                                                                                                                                                             
        
    return 0;
}

int mikaConnectionClose( struct mikaConnection *connection )
{
    if (!connection)
        return 1;
    close(connection->sockfd);
    return 0;
}

int _mikaServerInitValues( struct mikaServer *server )
{
    if (!server)
        return 1;
    
    server->connection = mikaConnectionInit(NULL);
    server->mem = mikaMemInit(NULL);
    server->callConnectionProcessing = NULL;
    int n;
    for(n = 0; n < CONNECTIONS; n++)
    {
        clientConnections[n] = (struct mikaConnection*)malloc(sizeof(struct mikaConnection*));
        mikaConnectionInit(clientConnections[n]);
    }    
        
    return 0;
}

struct mikaServer *mikaServerAlloc( struct mikaServer *server )
{
    if (!server)
    {
        server = (struct mikaServer*)malloc(sizeof(struct mikaServer));
        _mikaServerInitValues(server);
    }
    return server;
}

struct mikaServer *mikaServerInit( struct mikaServer *server )
{
    if (!server)
    {
        server = mikaServerAlloc(NULL);
    }
    else
    {
        _mikaServerInitValues(server);
    }
        
    return server;
}

int mikaServerListen( struct mikaServer *server )
{
    if (!server)
        return 1;
    
    mikaConnectionListen( server->connection );
    
    return 0;
}

int mikaServerAddMod( struct mikaServer *server, void *mod )
{
    if (!server || !mod)
        return 1;
        
    struct mikaServerMod *serverMod = (struct mikaServerMod*)mod;
    serverMod->next = (struct mikaServerMod*)server->mod;
    server->mod = serverMod;
        
    return 0;
}

/**  run a registered module's processing services if the current connection
 *   is to be run by the speciffic module
 */
void *_call_connection_processing_mods( struct mikaServer *server, struct mikaConnection *connection )
{
    if (!server || !connection)
        return NULL;
    
    #ifdef _DEBUG
        printf("_call_connection_processing_mods\n");
    #endif
    
    mikaConnectionClientGet(connection);
    if (connection->msg->inBuffer)
    {
        struct mikaServerMod *current = (struct mikaServerMod*)server->mod;
        while(current)
        {
            if (mikaServerModIsMessage(current,(unsigned char*)connection->msg->inBuffer)==0)
            {
                #ifdef _DEBUG
                    printf("_call_connection_processing_mods mod(%s)\n",
                        (char*)current->id);
                #endif
                if (current->processData)
                {
                    connection->msg->outBuffer[0] = 0x00;
                    current->processData(
                        (unsigned char*)connection->msg->inBuffer,
                        (unsigned char*)connection->msg->outBuffer
                        );
                    if (connection->msg->outBuffer[0]!=0x00)
                    {
                        // send response
                        mikaConnectionClientPut(connection);
                    }
                }
            }
            current = current->next;
        }
    }
    if (connection->msg->outBuffer)
        mikaConnectionClientPut(connection);
    
    return NULL;
}

/** check to which module the connection belongs and start the 'onHandshake'
 *  delegate function. This gives the most responsibility to the hands of the
 *  module
 */
void *_call_connection_processing_mods_ex( struct mikaServer *server, struct mikaConnection *connection )
{
    if (!server || !connection)
        return NULL;
    
    #ifdef _DEBUG
        printf("_call_connection_processing_mods_ex\n");
    #endif
    
    connection->msg->inBuffer[0] = 0x00;
    mikaConnectionClientGet(connection);
    if (connection->msg->inBuffer)
    {
        struct mikaServerMod *current = (struct mikaServerMod*)server->mod;
        while(current)
        {
            #ifdef _DEBUG
                printf("_call_connection_processing_mods_ex check module\n");
            #endif
            if (mikaServerModIsMessage(current,(unsigned char*)connection->msg->inBuffer)==0)
            {
                #ifdef _DEBUG
                    printf("_call_connection_processing_mods_ex run module\n");
                #endif
                if (current->delegate)
                {
                    if (current->delegate->onHandshake)
                        current->delegate->onHandshake(server,current,connection);
                }
                break;
            }
            current = current->next;
        }
    }
    
    mikaConnectionClientUnregister(server,connection);
    close(connection->sockfd);
    
    return NULL;
}    

#ifdef _STATIC
    int main( )
    {
        struct mikaServer *server = NULL;
        struct mikaServerMod *mod = NULL;
        struct mikaServerMod *modPos = NULL;
        struct mikaServerMod *modPosEx = NULL;
        struct mikaServerMod *configMod = NULL;
        
        struct mikaDb *db = mikaDbInit(NULL,(unsigned char*)"db/test.db");
        mikaDbPut(db,(unsigned char*)"testkey",(unsigned char*)"testvalue23");
        mikaDbPut(db,(unsigned char*)"testkey2",(unsigned char*)"testvalue24");
        
        modPos = mikaServerModInit();
        strcpy((char*)modPos->id,"mikaPos");
        modPos->processData = _process_data_mika_pos;
        
        // every module has a delegate with functions that
        // can are called when we receive a connection for that module
        //
        modPosEx = mikaServerModInit();
        strcpy((char*)modPosEx->id,"mikaPosEx");
        modPosEx->delegate = (struct mikaServerModDelegate*)malloc(
            sizeof(struct mikaServerPosDelegate)
            );
        mikaServerPosDelegateInit((struct mikaServerPosDelegate*)modPosEx->delegate);
        
        configMod = mikaServerModInit();
        strcpy((char*)configMod->id,"mikaConfig");
        configMod->processData = _process_data_config_server;
        
        server = mikaServerInit(NULL);
        mikaServerAddMod(server,(void*)mod);
        mikaServerAddMod(server,(void*)modPos);
        mikaServerAddMod(server,(void*)modPosEx);
        mikaServerAddMod(server,(void*)configMod);
        mikaServerListen(server);
        
        //server->callConnectionProcessing = _call_connection_processing_mods;
        server->callConnectionProcessing = _call_connection_processing_mods_ex;
        #ifdef _DEBUG
            printf("mikaServer main->mikaServerRequestLoop\n");
        #endif
        mikaServerRequestLoop(server);
        
        return 0;
    }
#endif
