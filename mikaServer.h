/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _MIKA_SERVER_H_
#define _MIKA_SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <pthread.h>
#include <db.h> // BerkeleyDB

#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"
#include "mikaDb.h"
#define _SERVER_EX

#define PORT        46000
#define CONNECTIONS 10
#define BUFFER	    4096*20
#define TMPBUFFER   512
#define MSGENDING   "</msg>"
#define RETRYDELAY  100000
#define TIMEOUT     20

/**
 * synchronize access to shared objects
 */
typedef struct mikaMem
{
    pthread_mutex_t id;
} mikaMem;

typedef struct mikaMsg
{
    unsigned char *inBuffer;
    unsigned char *outBuffer;
} mikaMsg;

typedef struct mikaDom
{
    TiXmlDocument *doc;
    struct mikaDomObject *objects;
} mikaDom;

typedef struct mikaConnection
{
    int active;
    int sockfd;
    struct mikaDom *dom;
    struct mikaMsg *msg;
} mikaConnection;

typedef struct mikaServer
{
    struct mikaMem *mem;
    mikaConnection *connection;
    void *(*callConnectionProcessing)(struct mikaServer *server, struct mikaConnection *connection);
    void *mod;
} mikaServer;

typedef struct mikaConnectionThread
{
    pthread_t id;
    struct mikaServer *server;
    struct mikaConnection *connection;
} mikaConnectionThread;

typedef struct mikaDomObject
{
    struct mikaDomObject *next;
    struct mikaDomObject *children;
} mikaDomObject;

extern struct mikaConnection *clientConnections[CONNECTIONS];

struct mikaMsg *mikaMsgAlloc( struct mikaMsg *msg );
struct mikaMsg *mikaMsgInit( struct mikaMsg *msg );
struct mikaMsg *mikaMsgReinit( struct mikaMsg *msg );

void *_call_connection_processing_custom( struct mikaServer *server, struct mikaConnection *connection );
int mikaConnectionClientThreadStart( struct mikaServer *server, struct mikaConnection *connection );
struct mikaConnection *mikaConnectionClientRegister( struct mikaServer *server, int newfd );
int mikaConnectionClientUnregister( struct mikaServer *server, struct mikaConnection *connection );
struct mikaConnection *mikaConnectionAlloc( struct mikaConnection *connection );
struct mikaConnection *mikaConnectionInit( struct mikaConnection *connection );
int mikaConnectionOpen( struct mikaConnection *connection, unsigned char *server, int port );
int mikaConnectionClose( struct mikaConnection *connection );
int mikaConnectionClientGet( struct mikaConnection *connection );
int mikaConnectionClientPut( struct mikaConnection *connection );

struct mikaServer *mikaServerAlloc( struct mikaServer *server );
struct mikaServer *mikaServerInit( struct mikaServer *server );

int mikaServerListen( struct mikaServer *server );
int mikaServerRequestLoop( struct mikaServer *server );

int mikaeServerAddMod( struct mikaServer *server, struct mikaServerMod *mod );

struct mikaMem *mikaMemInit( struct mikaMem *mem );
int mikaMemLock( struct mikaMem *mem );
int mikaMemUnlock( struct mikaMem *mem );

struct mikaDom *mikaDomInit( struct mikaDom *dom );
int mikaDomRelease( struct mikaDom *dom );
int mikaDomProcessData( struct mikaDom *dom, unsigned char *data );
int mikaDomParseData( struct mikaDom *dom, unsigned char *data );

struct mikaDb *mikaDbCreate( struct mikaDb *db, unsigned char *filename );
struct mikaDb *mikaDbInit( struct mikaDb *db, unsigned char *filename );
int mikaDbRelease( struct mikaDb *db );
int mikaDbPut( struct mikaDb *db, unsigned char *key, unsigned char *value );

#endif
