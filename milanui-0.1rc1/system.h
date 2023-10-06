/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#ifdef __cplusplus
  extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "config.h"
#include "csv.h"

#ifdef UNIX
  #include <sys/un.h>
#endif

#ifdef USESSL
  #include <openssl/bio.h>
  #include <openssl/ssl.h>
  #include <openssl/err.h>
#endif

#ifdef USETCL
  #include <tcl.h>
  #include <tk.h>
#endif

#ifdef USECAIRO
  //#include "cairo/include/cairo/cairo.h"
  #include <cairo/cairo.h>
#endif

#ifdef USEFT
  #define _FT_TYPE_CUSTOM_NO
  #define _FT_TYPE_GL
  
  #ifdef _FT_TYPE_CUSTOM
    #include "freetype/fttrop.h"
  #endif
#endif

#ifdef USERSS
  #include "rss/mrss.h"
#endif

#define TROP_WINDOW	4096
#define TROP_WINCNT	100
#define TROP_WINDATA	TROP_WINDOW*TROP_WINCNT

#define TROP_QCNT	20

#define TROP_NLANG_NONE	0
#define TROP_NLANG_SSL	1

#define TROP_CONNPOOL	20
#define TROP_CONNPORT	10501
#define TROP_CONNPREBUF	4096
#define TROP_CONNBUF	4096*30
#define TROP_CRYPT	FALSE

typedef struct trop_List
{
  void			*ptr;
  struct trop_List	*next;
} trop_List;

typedef struct trop_Relay
{
  int			id;			// relay id
  void			*data;			// relay data
  void			*(*call)(void*data);	// relay call
} trop_Relay;

typedef struct trop_Call
{
  char			msg[64];
  void			*(*call)(void*data);
  struct trop_Call 	*next;
} trop_Call;

typedef struct trop_Object
{
  struct trop_Call	*calls;
} trop_Object;

typedef struct trop_Mutex
{
  #ifdef UNIX
      pthread_mutex_t	id;
  #endif
  int			retval;
} trop_Mutex;

typedef struct trop_Atom
{
  #ifdef UNIX
    pthread_t	id;
  #endif
  int		retval;
  void		*data;
  void		*(*call)(void*data);
} trop_Atom;

typedef struct trop_STask
{
  int			ms;
  void			*(*call)(void*data);
  struct trop_STask	*next;
} trop_STask;

typedef struct trop_SAtom // long living atoms, can have many MP tasks
{
  struct trop_Atom 	atom;
  struct trop_STask	*task;
} trop_SAtom;

typedef struct trop_Msg
{
  int		id;
  time_t	time;
  void		*data;
} trop_Msg;

typedef struct trop_Queue
{
  struct trop_Msg *msg[TROP_QCNT];
} trop_Queue;

typedef struct trop_NMsg // network message
{
  char		msg[128]; // possible string message
  trop_List	*lstID; // id's of the message
  struct trop_NMsg
                *next;
} trop_NMsg;

/**
 *  trop action is a connection between ID's, a text description and some
 *  graphics - it describes a presentation of an event 
 */
typedef struct trop_Action
{
  char		msg[128];
  char		desc[128];
  char		about[512];
  char		image[128];
  trop_List	*lstID;
} trop_Action;

typedef struct trop_NLang // network language - crypting/conversion
{
  int		type;
  char		*in;
} trop_NLang;

extern char *_trop_conn_in; // only usable when singlethreaded connections
extern char *_trop_conn_out;

typedef struct trop_Conn
{
  BOOL		bsecure;
  #ifdef USESSL
    BIO		*bio;
    SSL_CTX	*ctx;
    SSL		*ssl;
  #endif
  trop_NLang	*nlang;
  struct trop_NMsg
                *msg; 	// static buffer of network messages
  int		fd_in;
  int		fd_out;
  char		remote[64];
  int		port;
  char		*in;	// -> _trop_conn_in
  char		*out;	// -> _trop_conn_out
} trop_Conn;

enum {
  TROP_IPC_NONE,
  TROP_IPC_UDS,
  TROP_IPC_RC_OK,
  TROP_IPC_RC_NOK
};

#ifdef UNIX
  typedef struct _trop_ipc_uds
  {
    char			path[256];
    char			recv[256];
    char			send[256];
    int			sockfd;
    struct trop_Atom	*queue;
    struct sockaddr_un	address;
    size_t		address_length;
  } _trop_ipc_uds;
#endif

typedef struct trop_IPC
{
  int		type;
  void		*sub;
} trop_IPC;

/**
 * running a speciffic tast paralel to the main thread
 */
typedef struct trop_paralel
{
  int			mintime;
  struct trop_paralel	*next;
} trop_paralel;

typedef struct ipc_Win
{
  unsigned char	state;
  int		id;
} ipc_Win;

#define	RSS_LEN_TITLE		512
#define RSS_LEN_CONTENT		4096

typedef struct trop_RssItem // rss tag
{
  char			title[RSS_LEN_TITLE];
  char			desc[RSS_LEN_CONTENT];
} trop_RssItem;

typedef struct trop_Rss
{
  #ifdef _RSSTYPE_MRSS
    struct mrss_t	*mrss;
  #endif
  struct trop_List	*items;
} trop_Rss;

/**
 *  simple key/value type querying
 */
typedef struct trop_DataModelDescriptor
{
  unsigned char 	*key;
  unsigned char 	*value;
  int			nValue; // makes sence if the descriptor assigns an 'in member' position
                                //  member
                                //               (nValue)
                                //   +--> name      0
                                //   +--> surname   1
  int			len_key;
  int			len_value;
  struct trop_DataModelDescriptor
                        *next;
} trop_DataModelDescriptor;

typedef struct trop_DataModel
{
  struct trop_DataModelDescriptor
                        *descriptor;
  struct trop_Csv	*csv;
} trop_DataModel;

int trop_Init( );
int trop_Deinit( );

trop_List *trop_ListAdd( struct trop_List *list, void *add );
trop_List *trop_ListPush( struct trop_List *list, void *add );
int trop_ListDelete( struct trop_List *list );
int trop_ListDeleteRecursive( struct trop_List *list );
int trop_ListDeleteHard( struct trop_List *list ); // also deletes members of list elements
struct trop_List *trop_ListDeleteMember( struct trop_List *list, void *memb );

int trop_ObjectInit( struct trop_Object *o );
int trop_ObjectAddCall( struct trop_Object *o, struct trop_Call *c );

int trop_AtomInit( struct trop_Atom *atom );
int trop_AtomCancel( struct trop_Atom *atom );

int trop_SAtomInit( struct trop_SAtom *satom );
int trop_SAtomAddTask( struct trop_SAtom *satom, struct trop_STask *task, int ms );

#define TRYLOCK_LOCKED		0
#define TRYLOCK_UNLOCKABLE	1
#define TRYLOCK_UNKNOWN		2

int trop_MutexInit( struct trop_Mutex *mutex );
int trop_MutexLock( struct trop_Mutex *mutex );
int trop_MutexUnlock( struct trop_Mutex *mutex );
int trop_MutexTrylock( struct trop_Mutex *mutex );

int trop_MemLock( );
int trop_MemUnlock( );

int trop_MsgSend( struct trop_Queue *q, struct trop_Msg *msg );
int trop_MsgNext( struct trop_Queue *q, struct trop_Msg *msg );
struct trop_Queue *trop_MsgQCreate( );

int fprintfex( FILE *f, const char *str, ... );

struct trop_NMsg *trop_NMsgInit( struct trop_NMsg *msg );
int trop_NMsgAddID( struct trop_NMsg *msg, int id );
int trop_NMsgAddMsg( struct trop_NMsg *msg, char *text );

struct trop_Action *trop_ActionInit( struct trop_Action *action );
int trop_ActionAddID( struct trop_Action *action, int id );
int trop_ActionTrigger( struct trop_Action *action, struct trop_Conn *conn );

struct trop_NLang *new_TropNlang( );
int trop_NLangCrypt( struct trop_NLang *nlang );
int trop_NLangDecrypt( struct trop_NLang *nlang );

int trop_IOSaveDataToFile( char *data, char *filename );

int trop_ConnInit( struct trop_Conn *conn );
int trop_ConnDeinit( struct trop_Conn *conn );
int trop_ConnSend( struct trop_Conn *conn );
int trop_ConnSendHttpRequest( struct trop_Conn *conn, char *file );
int trop_ConnGet( struct trop_Conn *conn );
int trop_ConnGetEx( struct trop_Conn *conn );
int trop_ConnAddMsg( struct trop_Conn *conn, struct trop_NMsg *msg );
int trop_ConnSendMsg( struct trop_Conn *conn, struct trop_NMsg *msg );
int trop_ConnSendMsgs( struct trop_Conn *conn );

int _trop_ipc_udp_parser( struct _trop_ipc_uds *uds );
void *_trop_ipc_uds_queue( void *data );
int trop_IPCUdsHandshake( struct trop_IPC *ipc, char *path, char *msg );
int trop_IPCUdsQueue( struct trop_IPC *ipc );
int trop_IPCUdsCustomQueue( struct trop_IPC *ipc, void *(*call)(void*data) );
int trop_IPCUdsInit( struct trop_IPC *ipc, char *path );
struct trop_IPC *trop_IPCInit( struct trop_IPC *ipc );

struct trop_DataModel *trop_DataModelInit( struct trop_DataModel *model );
struct trop_DataModel *trop_DataModelGetFromCsv( struct trop_DataModel *model, unsigned char *csvFilename );
int trop_DataModelGetDataAtPosition( struct trop_DataModel *model, int posList, int posMember, int maxLen, unsigned char *data, int *lenData );
struct trop_DataModelDescriptor *trop_DataModelDescriptorInit( struct trop_DataModelDescriptor *desc );
struct trop_DataModelDescriptor *trop_DataModelDescriptorAddPair( struct trop_DataModelDescriptor *desc, unsigned char *key, unsigned char *value );
struct trop_DataModelDescriptor *trop_DataModelDescriptorAddPairNum( struct trop_DataModelDescriptor *desc, unsigned char *key, int value );
int trop_DataModelGetListSize( struct trop_DataModel *model );
int trop_DataModelGetListMemberSize( struct trop_DataModel *model, int posList );

int ipc_WinResetAll( );
int ipc_WinGetState( int pos );

/**
 *  mika - very small tree database system:
*/
#define SYMBOLS		256
#define BASESIZE	256

typedef struct sParentValues
{
  char			name[BASESIZE];
} sParentValues;

typedef struct sMember {
  void 			*symbols[SYMBOLS];
  void 			*value;
  struct sMember 	*parent;
  struct sParentValues	*pval;
} sMember;

typedef struct sListData {
  void		*list;
  void		*ptr;
} sListData;

typedef struct sList {
  void		*ptr;
  struct sList 	*next;
  void		*add;
} sList;

void tree_initMember( struct sMember *member );
void tree_setMemberParent( struct sMember *member, struct sMember *parent );
void tree_setMemberPVAL( struct sMember *member, struct sParentValues *pval );
void tree_freeMember( struct sMember *member );
void tree_printAllValues( struct sMember *member );
BOOL tree_addBound( struct sMember *member, char *key, char *value );
BOOL tree_removeBound( struct sMember *member, char *key );
BOOL tree_printBound( struct sMember *member, char *key );
BOOL tree_getBoundsFromFile( struct sMember *member, char *filename );
BOOL tree_getBOundsFromFileEX( struct sMember *member, char *filename );

#ifdef __cplusplus
  }
#endif

#endif
