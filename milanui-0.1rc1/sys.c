/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "system.h"
#include "xml.h"
#include "global.h"

unsigned long _hex2long( 
  unsigned char *str 
)
{
  static unsigned long 	ret = 0;
  static char 		*p;
  static char	 	tmp[8];
  
  if (!str) return 0;
  
  #ifdef _DEBUG_X
    #ifdef _DEBUG
      if (DEBUG==TRUE) 
        printf("_hex2long (%s)\n",str);
    #endif
  #endif
  
  p = strcpy(tmp,(const char*)str);
  while(*p)
  {
    *p = *p|32;
    if (*p>='a') *p-=39;
    *p -= 48;
    if (((*p)<0) || ((*p)>15)) return 0;
    ret = (ret<<4)|(*p++);
  }
  
  #ifdef _DEBUG_X
    #ifdef _DEBUG
      if (DEBUG==TRUE) 
        printf("_hex2long.end (%s)\n",str);
    #endif
  #endif
  
  return ret;
}

int _hex2unsignedChar(
  unsigned char *str,
  unsigned char *r,
  unsigned char *g,
  unsigned char *b,
  unsigned char *a
)
{
  static int nr, ng, nb, na;
  
  if (!str||!r||!g||!b) return -1;
  
  sscanf(str,"%02x%02x%02x",&nr,&ng,&nb);
  
  *r = (unsigned char)nr;
  *g = (unsigned char)ng;
  *b = (unsigned char)nb;
  
  return 0;
}

unsigned char *_long2hex(
  unsigned long value, 
  unsigned char *hex
)
{
  if (hex==NULL)
    hex = (unsigned char*)malloc(7);
  sprintf((char*)hex,"%6x",value);
  static int n, len;
  len = strlen(hex);
  for (n = len-1; n > -1; n--) // remove
  {
    hex[n+(6-len)] = hex[n];
    //hex[n] = '0';
  }
  return hex;
}

int _hex_move_up(
  unsigned char *hex
)
{
  if (!hex) return -1;
  if (strlen(hex)!=6) return -1;
  static int n;
  for (n = 0; n < 6; n++)
  {
    if (hex[n] == 'f') continue;
    if (hex[n] == '9') hex[n] = 'a';
    else hex[n]++;
  }
  return 0;
}

int _hex_move_up_ex(
  unsigned char *hex
)
{
  static int n;
  BOOL add = FALSE;
  for (n = 5; n > -1; n--)
  {
    if (n == 4 || n == 2 || n == 0)
    {
      if (add == TRUE)
      {
        if (hex[n] == '9') hex[n] = 'a';
        else hex[n]++;
        add = FALSE;
      }
    }
    else
    {
      if (hex[n] == 'f')
      {
        if (hex[n-1] != 'f')
        {
          hex[n] = '0';
          add = TRUE;
        }
      }
      else if (hex[n] == '9')
        hex[n] = 'a';
      else
        hex[n]++;
    }
  }
  return 0;
}

int _hex_move_up_at( 	// move only 'at*pair' position
  unsigned char *hex,	// when moving color r=1,g=2,b=3
  int at
)
{
  if (at<1 || at>3) return -1;
  static int n;
  BOOL add = FALSE;
  for (n = (at*2)-1; n > (at*2)-3; n--)
  {
    if (n == 4 || n == 2 || n == 0)
    {
      if (add == TRUE)
      {
        if (hex[n] == '9') hex[n] = 'a';
        else hex[n]++;
        add = FALSE;
      }
    }
    else
    {
      if (hex[n] == 'f')
      {
        if (hex[n-1] != 'f')
        {
          hex[n] = '0';	
          add = TRUE;
        }
      }
      else if (hex[n] == '9')
        hex[n] = 'a';
      else
        hex[n]++;
    }
  }
  return 0;
}

int _hex_move_down(
  unsigned char *hex
)
{
  static int n;
  BOOL less = FALSE;
  for (n = 5; n > -1; n--)
  {
    if (n == 4 || n == 2 || n == 0)
    {
      if (less == TRUE)
      {
        if (hex[n]=='a') hex[n] = '9';
        else if (hex[n]!='0') hex[n]--;
        less = FALSE;
      }
    }
    else
    {
      if (hex[n]=='a') hex[n] = '9';
      else if (hex[n]=='0')
      {
        if (hex[n-1]!='0')
        {
          hex[n] = 'f';
          less = TRUE;
        }
      }
      else
        hex[n]--;
    }
  }
  return 0;
}

int _hex_move_down_at( 	// move only 'at*pair' position
  unsigned char *hex,	// when moving color r=1,g=2,b=3
  int at
)
{
  if (!hex || at<1 || at>3) return -1;
  static int n;
  BOOL less = FALSE;
  for (n = (at*2)-1; n > (at*2)-3; n--)
  {
    if (n == 4 || n == 2 || n == 0)
    {
      if (less == TRUE)
      {
        if (hex[n]=='a') hex[n] = '9';
        else if (hex[n]!='0') hex[n]--;
        less = FALSE;
      }
    }
    else
    {
      if (hex[n]=='a') hex[n] = '9';
      else if (hex[n]=='0')
      {
        if (hex[n-1]!='0')
        {
          hex[n] = 'f';
          less = TRUE;
        }
      }
      else
        hex[n]--;
    }
  }
  return 0;
}

#ifdef UNIX
  void trop_MutexInitUNIX( struct trop_Mutex *mutex )
  {
    if ( pthread_mutex_init(&mutex->id,NULL) == 0 )
      mutex->retval = 0;
  }
  
  void trop_MutexLockUNIX( struct trop_Mutex *mutex )
  {
    pthread_mutex_lock(&mutex->id);
  }
  
  void trop_MutexUnlockUNIX( struct trop_Mutex *mutex )
  {
    pthread_mutex_unlock(&mutex->id);
  }

  int trop_MutexTrylockUNIX( struct trop_Mutex *mutex )
  {
    if (pthread_mutex_trylock(&mutex->id) == 16)
    {
      return TRYLOCK_UNLOCKABLE;
    }
    else
      return TRYLOCK_LOCKED;
  }
  
  void trop_AtomInitUNIX( struct trop_Atom *atom )
  {
    trop_MutexLock(&_m_lock);
    pthread_create(&atom->id,NULL,atom->call,atom->data);
    pthread_detach(atom->id);
    trop_MutexUnlock(&_m_lock);
  }
  
  void trop_AtomCancelUNIX( struct trop_Atom *atom )
  {
    pthread_cancel(atom->id);
  }
#endif

int trop_MutexInit( struct trop_Mutex *mutex )
{
  #ifdef UNIX
    trop_MutexInitUNIX( mutex );
  #endif
  return mutex->retval;
}

int trop_MutexLock( struct trop_Mutex *mutex )
{
  #ifdef UNIX
    trop_MutexLockUNIX( mutex );
  #endif
  return mutex->retval;
}

int trop_MutexTrylock( struct trop_Mutex *mutex )
{
  #ifdef UNIX
    return trop_MutexTrylockUNIX( mutex );
  #endif
  return TRYLOCK_UNKNOWN;
}

int trop_MutexUnlock( struct trop_Mutex *mutex )
{
  #ifdef UNIX
    trop_MutexUnlockUNIX( mutex );
  #endif
  return mutex->retval;
}

int trop_MemLock( ) { trop_MutexLock(&_m_lock); }
int trop_MemUnlock( ) { trop_MutexUnlock(&_m_lock); }

int fprintfex( FILE *f, const char *str, ... )
{
  trop_MemLock( );
  int ret;
  va_list ap;
  va_start(ap,str);
  ret = vfprintf(f,str,ap);
  va_end(ap);
  trop_MemUnlock( );
}

void _trop_termination( )
{
  exit(0);
}

/** linked list handling functions, declared inline 
 */

inline struct trop_List *trop_ListAdd( struct trop_List *list, void *add )
{
  if (list == NULL)
  {
    list = (struct trop_List*)malloc(sizeof(struct trop_List));
    list->next = NULL;
    list->ptr = add;
  }
  else
  {
    struct trop_List *current = list;
    while( current != NULL )
    {
      if (current->next == NULL)
      {
        current->next = (struct trop_List*)malloc(sizeof(struct trop_List));
        current->next->next = NULL;
        current->next->ptr = add;
        break;
      }
      current = current->next;
    }
  }
  return list;
}

inline struct trop_List *trop_ListPush(struct trop_List *list, void *add )
{
  struct trop_List *item = NULL;
  
  if (list == NULL)
  {
    list = (struct trop_List*)malloc(sizeof(struct trop_List));
    list->next = NULL;
    list->ptr = add;
    item = list;
  }
  else
  {
    struct trop_List *item = (struct trop_List*)malloc(sizeof(struct trop_List));
    item->ptr = add;
    item->next = list;
  }
  return item;
}

inline int trop_ListDelete( struct trop_List *list )
{
  struct trop_List *current = list;
  void *p = NULL;
  while(current != NULL)
  {
    p = current;
    current = current->next;
    if (p != NULL)
      free(p);
  }
  return 0;
}

/**
 * deletes list and also members of each element 
 */
inline int trop_ListDeleteRecursive( struct trop_List *list )
{
  struct trop_List *current; // todo, check how gcc handles static in inline
  struct trop_List *mem;
  
  if (!list) return -1;
  
  current = list;
  while(current!=NULL)
  {
    mem = current;
    current = current->next;
    if (mem->ptr!=NULL) 
    {
      free(mem->ptr);
      mem->ptr = NULL;
    }
    free(mem);
  }
  
  return 0;
}

inline int trop_ListDeleteHard( struct trop_List *list )
{
  struct trop_List *current = list;
  struct trop_List *last = NULL;
  while(current != NULL)
  {
    last = current;
    current = current->next;
    if (last != NULL)
    {
      free(last->ptr);
      free(last);
    }
  }
  return 0;
}

inline struct trop_List *trop_ListDeleteMember( struct trop_List *list, void *memb )
{
  if (!list) return NULL;
  if (!memb) return list;
  struct trop_List *current = list;
  struct trop_List *last = NULL;
  while(current)
  {
    if (current->ptr == memb)
    {
      if (last != NULL)
        last->next = current->next;
      else
        list = current->next;
      current->ptr = NULL;
      free(current);
      break;
    }
    last = current;
    current = current->next;
  }
  return list;
}

int trop_ObjectInit( struct trop_Object *o )
{
  o->calls = NULL;
  return 0;
}

int trop_ObjectAddCall( struct trop_Object *o, struct trop_Call *c )
{
  if (o->calls == NULL)
  {
    c->next = NULL;
    o->calls = c;
  }
  else
  {
    c->next = o->calls;
    o->calls = c;
  }
  return 0;
}

int trop_AtomInit( struct trop_Atom *atom )
{
  #ifdef UNIX
    trop_AtomInitUNIX( atom );
  #endif
  return atom->retval;
}

int trop_AtomCancel( struct trop_Atom *atom )
{
  #ifdef UNIX
    trop_AtomCancelUNIX( atom );
  #endif
  return atom->retval;
}

/**
 * S* interface to run parallel tasks alongside the main thread
 */
#define _SATOM_CLOCKTIK 100000
void *_satom_task_runner( void *data )
{
  struct trop_SAtom *satom = (struct trop_SAtom*)data;
  struct trop_STask *task = NULL;
  while(1)
  {
    trop_MemLock();
    task = satom->task;
    while(task != NULL)
    {
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf(":satom task\n");
      #endif
      task->call(task);
      task = task->next;
      usleep(_SATOM_CLOCKTIK/100);
    }
    usleep(_SATOM_CLOCKTIK);
    trop_MemUnlock();
  }
}

int trop_SAtomInit( struct trop_SAtom *satom )
{
  satom->task = NULL;
  satom->atom.call = _satom_task_runner;
  satom->atom.data = satom;
  trop_AtomInit(&satom->atom);
  return 0;
}

int trop_SAtomAddTask( struct trop_SAtom *satom, struct trop_STask *task, int ms ) // not synced
{
  trop_MemLock();
  if (satom->task == NULL)
  {
    task->next = NULL;
    satom->task = task;
  }
  else
  {
    struct trop_STask *_task = satom->task;
    satom->task = task;
    satom->task->next = _task;
  }
  trop_MemUnlock();
  return 0;
}

struct trop_Queue *trop_MsgQCreate( )
{
  struct trop_Queue *q = (struct trop_Queue*)malloc(sizeof(struct trop_Queue));
  int i;
  for (i = 0; i < TROP_QCNT; i++)
  {
    q->msg[i] = NULL;
  }
  return q;
}

int trop_MsgSend( struct trop_Queue *q, struct trop_Msg *msg )
{
  int i, ret = -1;
  trop_MemLock( );
  for (i = 0; i < TROP_QCNT; i++)
  {
    if (q->msg[i] == NULL)
    {
      q->msg[i] = msg;
      ret = i;
      break;
    }
  }
  trop_MemUnlock( );
  return ret;
}

int trop_MsgNext( struct trop_Queue *q, struct trop_Msg *msg )
{
  int i, ii;
  trop_MemLock( );
  for (i = 0; i < TROP_QCNT; i++)
  {
    if (q->msg[i] != NULL)
    {
      break;
    }
  }
  ii = i;
  if (i != 0)
  {
    for (1; i < TROP_QCNT; i++)
    {
      if (q->msg[i] != NULL)
      {
        q->msg[i-ii] = q->msg[i];
        q->msg[i] = NULL;
      }
    } 
  }
  if (q->msg[0] != NULL)
  {
    msg->id = q->msg[0]->id;
    msg->time = q->msg[0]->time;
    msg->data = q->msg[0]->data;
    q->msg[0] = NULL;
    trop_MemUnlock( );
    return 0;
  }
  trop_MemUnlock( );
  return -1;
}

/**
 *
 */

#ifdef USESSL

int _get_peer_cert( SSL *ssl, X509 **peerCertificate )
{
  int ret;
  if (ssl == NULL) return -1;
  
  ret = SSL_get_verify_result(ssl);
  *peerCertificate = SSL_get_peer_certificate(ssl);
  return ret;
}

int _verify_cert( X509 *peerCertificate, const char *myname )
{
  char commonName[512];
  X509_NAME *name;
  
  if (peerCertificate == NULL || myname == NULL) return -1;

  name = X509_get_subject_name(peerCertificate);
  X509_NAME_get_text_by_NID(name, NID_commonName, commonName, 512);
  
  #ifdef _DEBUG
    if (DEBUG==TRUE) printf(":verify cert hostname %s\n", commonName);
  #endif
  
  if (strcasecmp(commonName,myname) == 0) return 1;
  else return 0;
}

int _password_callback( char *buf, int size, int rwflag, void *userdata )
{
  strcpy(buf, "");
  return 1;
}

#endif

struct trop_NLang *new_TropLang( )
{
  struct trop_NLang *nlang = (struct trop_NLang*)malloc(sizeof(struct trop_NLang));
  return nlang;
}

int trop_NLangCrypt( struct trop_NLang *nlang )
{
  return 0;
}

int trop_NLangDecrypt( struct trop_NLang *nlang )
{
  return 0;
}

/**
 * prepare a connection object, but without connecting 
 */
int trop_ConnInitPrepare( struct trop_Conn *conn )
{
  if (!conn) return -1;
  
  conn->msg 		= NULL;
  conn->bsecure 	= FALSE;
  conn->remote[0] 	= 0x00;
  conn->port 		= -1;
  conn->in		= (char*)_trop_conn_in;
  conn->out		= (char*)_trop_conn_out;
  
  return 0;
}

/**
 * initialize the connection structures
 */
int trop_ConnInit( struct trop_Conn *conn ) // needs sync
{
  if (!conn)
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("trop_ConnInit !conn\n");
    #endif
    return -1;
  }
  
  if (conn->remote[0] == 0x00 || conn->port < 0)
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("trop_ConnInit remote not set\n");
    #endif
    return -1;
  }
  
  conn->msg = NULL;
  #ifdef USESSL
    if (conn->bsecure == TRUE)
    {
      ERR_load_BIO_strings();
      SSL_load_error_strings();
      OpenSSL_add_all_algorithms();
    
      int (*callback)(char*, int, int, void *) = &_password_callback;
    
      conn->ctx = SSL_CTX_new(SSLv23_client_method());
      // trust store - pem with private key and public certificate
      
      //SSL_CTX_set_default_passwd_cb(conn->ctx,callback);
      //if (!SSL_CTX_use_certificate_file(conn->ctx,"mycert2.pem",SSL_FILETYPE_PEM))
      if (!SSL_CTX_load_verify_locations(conn->ctx,"mycert2.pem",NULL))
      {
        printf(":error loading trust store\n");
        SSL_CTX_free(conn->ctx);
        return -1;
      }
      conn->bio = BIO_new_ssl_connect(conn->ctx);
      BIO_get_ssl(conn->bio, &conn->ssl);
      SSL_set_mode(conn->ssl,SSL_MODE_AUTO_RETRY);
    }
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":done_secure\n");
    #endif
    
    char name[64];
    sprintf((char*)name,"%s:%d",conn->remote,conn->port);
    
    if (conn->bsecure == TRUE)
      BIO_set_conn_hostname(conn->bio,(char*)name);
    else
      conn->bio = BIO_new_connect((char*)name);
    
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf(":done_base_bio\n");
    #endif
    
    if (BIO_do_connect(conn->bio) <= 0)
    {
      printf(":Could not connect socket\n");
      BIO_free_all(conn->bio);
      if (conn->bsecure==TRUE)
      {
        SSL_CTX_free(conn->ctx);
      }
      return -1;
    }
    
    if (conn->bsecure == TRUE)
    {
      X509 *peerCert;
    
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf(":getting peer certificate\n");
      #endif
    
      if (_get_peer_cert(conn->ssl, &peerCert) != X509_V_OK)
      {
        printf(":certiffication veriffication error: %i\n",
          SSL_get_verify_result(conn->ssl));
        BIO_free_all(conn->bio);
        SSL_CTX_free(conn->ctx);
        return -1;
      }
    
      if (!_verify_cert(peerCert,"localhost"))
      {
        printf(":hostname and common name do not match\n");
        BIO_free_all(conn->bio);
        SSL_CTX_free(conn->ctx);
        return 0;
      }
    }
    
  #else
    struct sockaddr_in *sin;
    memset(sin,0,sizeof(struct sockaddr_in));
    sin->sin_family = AF_INET;
    sin->sin_port = htons(TROP_CONNPORT);
  
    if ((conn->fd_in = socket(PF_INET,SOCK_DGRAM,0)) == -1)
    {
      fprintf(_stdout,":Could not create socket\n");
      _trop_termination( );
    }
    else
    {
      
    }
    return 0;
  #endif
}

int trop_ConnDeinit( struct trop_Conn *conn )
{
  if (!conn)
  {
    #ifdef _DEBUG
      if (DEBUG==TRUE)
      {
        printf("trop_ConnDeinit !conn\n");
      }
    #endif
    return -1;
  }
  #ifdef USESSL
    BIO_free_all(conn->bio);
  #else
    close(conn->fd_in);
  #endif
  return 0;
}

#define _W_RETRY	3
int trop_ConnSend( struct trop_Conn *conn )
{
  #ifdef USESSL
    if (BIO_write(conn->bio,conn->out,conn->fd_out) <= 0)
    {
      if (!BIO_should_retry(conn->bio))
      {
        conn->fd_out = -1;
        return -1;
      }
      int i;
      for (i = 0; i < _W_RETRY; i++)
      {
        if ( BIO_write(conn->bio,conn->out,conn->fd_out) > 0 )
          return 0;
      }
      conn->fd_out = -1;
      return -1;
    }
  #endif
  return 0;
}

int trop_ConnSendHttpRequest( struct trop_Conn *conn, char *file )
{
  sprintf((char*)conn->out,"GET %s HTTP/1.1\r\nHost: %s\r\n\r\n",
    file,(char*)conn->remote);
  conn->fd_out = strlen((char*)conn->out);
  trop_ConnSend(conn);
  return 0;
}

int trop_ConnGet( struct trop_Conn *conn )
{
  #ifdef USESSL
    int rb = BIO_read(conn->bio,conn->in,512);
    if (rb == 0)
    {
      printf("BIO: :could not read on socket\n");
      conn->fd_in = -1;
      return -1;
    }
    else if (rb < 0)
    {
      if (!BIO_should_retry(conn->bio))
      {
        printf("BIO: :could not read on socket\n");
        conn->fd_in = -1;
        return -1;
      }
    }
    conn->fd_in = rb;
    return rb;
  #endif
  return 0;
}

/**
 * send message through the connection interface
 */
int trop_ConnSendMsg( struct trop_Conn *conn, struct trop_NMsg *msg )
{
  char tmp[128];
  int id;
  conn->out[0] = 0x00;
  struct trop_List *current = msg->lstID;
  while(current != NULL)
  {
    memcpy(&id,current->ptr,sizeof(int));
    sprintf((char*)tmp,"<id>%d</id>\n",id);
    strcat((char*)conn->out,(char*)tmp);
    current = current->next;
  }
  if (msg->msg[0] != 0x00)
  {
    sprintf((char*)tmp,"<msg>%s</msg>\n",(char*)msg->msg);
    strcat((char*)conn->out,(char*)tmp);
  }
  #ifdef _DEBUG
    if (DEBUG==TRUE) printf(":network message:\n%s\n",(char*)conn->out);
  #endif
  conn->fd_out = strlen((char*)conn->out);
  trop_ConnSend(conn);
  return 0;
}

int trop_ConnSendMsgs( struct trop_Conn *conn ) // try sending messages in message buffer
{
  int sret;
  struct trop_NMsg *current = conn->msg;
  struct trop_NMsg *last = NULL;
  while(current != NULL)
  {
    sret = trop_ConnSendMsg( conn, current );
    last = current;
    current = current->next;
    if (sret != 0)
    {
      return -1;
    }
    else
    {
      trop_ListDeleteHard(last->lstID);
      free(last);
      conn->msg = current;
    }
  }
  return 0;
}

#define _CONN_MAXMSG	10
int trop_ConnAddMsg( struct trop_Conn *conn, struct trop_NMsg *msg )
{
  if ( conn->msg == NULL )
  {
    conn->msg = msg;
  }
  else
  {
    int i = 0;
    struct trop_NMsg *current = conn->msg;
    while(current != NULL)
    {
      if (current->next == NULL)
      {
        current->next = msg;
        break;
      }
      i++;
      if (i == _CONN_MAXMSG) // todo delete message on this return 'upstream'
      {
        return -1;
      }
      current = current->next;
    }
  }
  return 0;
}

int trop_ConnGetEx( struct trop_Conn *conn )
{
  int rb = 0;
  
  conn->fd_in = 0;
  #ifdef USESSL
    while(1)
    {
      rb = BIO_read(conn->bio,conn->in+conn->fd_in,TROP_CONNPREBUF);
      if (rb == 0)
      {
        #ifdef _DEBUG
          if (DEBUG==TRUE) printf(":read on socket finished\n");
        #endif
        break;
      }
      else if (rb < 0)
      {
        if (!BIO_should_retry(conn->bio))
        {
          #ifdef _DEBUG
            if (DEBUG==TRUE) printf(":could not read on socket\n");
          #endif
          conn->fd_in = -1;
          return -1;
        }
      }
      if (conn->fd_in+rb+TROP_CONNPREBUF+1>TROP_CONNBUF)
      {
        break;
      }
      conn->fd_in += rb;
    }
    conn->in[conn->fd_in] = 0x00;
    #ifdef _DEBUG
      if (DEBUG==TRUE) printf("trop_ConnGetEx (%s)finish\n",(char*)conn->in);
    #endif
  #endif
  return 0;
}

  int _trop_ipc_udp_parser( struct _trop_ipc_uds *uds )
  {
    #ifdef UNIX
      if (!uds) return -1;
      int rc = TROP_IPC_RC_OK;
      char tmp[32];
      struct xml_Object *o = NULL;
      struct xml_Bit *b = NULL;
      struct xml_Inst xml;
      uds->send[0] = 0x00;
      strcpy((char*)uds->send,"<msg>\n");
    
      xml.filename[0] = 0x00;
      xml.data = (char*)uds->recv;
      xml_Init(&xml);
      o = xml_GetObject(&xml,"from");
      if (o!=NULL)
      {
        b = xml_GetObjectBit(o,"value");
      }
    
      sprintf((char*)tmp,"<rc value=\"%d\">\n",rc);
      strcat((char*)uds->send,(char*)tmp);
    #endif
    return 0;
  }
  
  void *_trop_ipc_uds_queue(void*data)
  {
    #ifdef UNIX
      struct _trop_ipc_uds *uds = data;
      int connfd;
      int nbytes;
      while((connfd = accept(uds->sockfd, (struct sockaddr*)&uds->address,
        &uds->address_length)) > -1)
      {
        #ifdef _DEBUG
	  printf("uds (%s) connection\n",(char*)uds->path);
        #endif
        uds->recv[0] = 0x00;
        nbytes = read(connfd,(char*)uds->recv,255);
        if (nbytes>0)
        {
          uds->recv[nbytes] = 0x00;
          #ifdef _DEBUG
	    printf("uds recieved (%s) bytes(%d)\n",(char*)uds->recv,nbytes);
          #endif
          _trop_ipc_udp_parser( uds );
          write(connfd,(char*)uds->send,strlen((char*)uds->send));
        }
        close(connfd);
      }
      close(uds->sockfd);
      unlink((char*)uds->path);
    #endif
  }

/**
 *  a UDS queue with a custom handler
 */
int trop_IPCUdsCustomQueue( struct trop_IPC *ipc, void *(*call)(void*data) )
{
  if (!ipc) return -1;
  #ifdef UNIX
    static struct _trop_ipc_uds *uds = NULL;
    if (!ipc) return -1;
    if (ipc->type != TROP_IPC_UDS) 
    {
      #ifdef _DEBUG
	printf("wrong IPC type to be handled by trop_IPCUdsQueue\n");
      #endif
      return -1;
    }
    uds = ipc->sub;
    if (uds->queue==NULL)
    {
      uds->queue = (struct trop_Atom*)malloc(sizeof(struct trop_Atom));
      uds->queue->call = call;
      uds->queue->data = uds;
      trop_AtomInit(uds->queue);
    }
  #else
    return -1;
  #endif
  return 0;
}

int trop_IPCUdsQueue( struct trop_IPC *ipc )
{
  #ifdef UNIX
    static struct _trop_ipc_uds *uds = NULL;
    if (!ipc) return -1;
    if (ipc->type != TROP_IPC_UDS) 
    {
      #ifdef _DEBUG
	printf("wrong IPC type to be handled by trop_IPCUdsQueue\n");
      #endif
      return -1;
    }
    uds = ipc->sub;
    if (uds->queue==NULL)
    {
      uds->queue = (struct trop_Atom*)malloc(sizeof(struct trop_Atom));
      uds->queue->call = _trop_ipc_uds_queue;
      uds->queue->data = uds;
      trop_AtomInit(uds->queue);
    }
    return 0;
  #else
    return -1;
  #endif
}

/**
 *  sends a message to a UDS socket and recieves an answer 
 */
int trop_IPCUdsHandshake( struct trop_IPC *ipc, char *path, char *msg )
{
  #ifdef UNIX
    static struct _trop_ipc_uds *uds = NULL;
    if (!ipc || !path) return -1;
    if (ipc->sub==NULL)
    {
      ipc->sub = (struct _trop_ipc_uds*)malloc(sizeof(struct _trop_ipc_uds));
      ipc->type = TROP_IPC_UDS;
      uds = ipc->sub;
      uds->queue = NULL;
      uds->recv[0] = 0x00;
      uds->send[0] = 0x00;
    }
    else uds = ipc->sub;
    if (uds->queue!=NULL)
    {
      printf("ipc uds descriptor already used in queue\n");
      return -1;
    }
    uds->sockfd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (uds->sockfd < 0)
    {
      printf("could not create uds socket\n");
      return -1;
    }
    uds->address.sun_family = AF_UNIX;
    uds->address_length = sizeof(uds->address.sun_family) +
      sprintf(uds->address.sun_path, path);
    if (connect(uds->sockfd, (struct sockaddr*)&uds->address,uds->address_length) != 0)
    {
      printf("connect to ipc uds (%s) failed\n",path);
      return -1;
    }
    strcpy((char*)uds->send,msg);
    uds->recv[0] = 0x00;
    printf("sending to uds (%s) size(%d)\n",
      (char*)uds->send,strlen((char*)uds->send));
    write(uds->sockfd,(char*)uds->send,strlen((char*)uds->send));
    int nbytes = read(uds->sockfd,(char*)uds->recv,255);
    if (nbytes>0)
    {
      uds->recv[nbytes] = 0x00;
      #ifdef _DEBUG
        if (DEBUG==TRUE) printf("recieved from uds \n%s\n",(char*)uds->recv);
      #endif
    }
    close(uds->sockfd);
    return 0;
  #else
    printf("no uds socket support on this system\n");
    return -1;
  #endif
}

/**
 * creates an UDS socket 
 */
int trop_IPCUdsInit( struct trop_IPC *ipc, char *path )
{
  #ifdef UNIX
    static struct _trop_ipc_uds *uds = NULL;
    if (!ipc || !path) return -1;
    ipc->sub = (struct _trop_ipc_uds*)malloc(sizeof(struct _trop_ipc_uds));
    ipc->type = TROP_IPC_UDS;
    uds = ipc->sub;
    uds->queue = NULL;
    uds->recv[0] = 0x00;
    uds->send[0] = 0x00;
    strcpy((char*)uds->path,path);
    uds->sockfd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (uds->sockfd < 0)
    {
      printf("failed to create uds socket (%s)\n",(char*)uds->path);
      return -1;
    }
    unlink((char*)uds->path);
    uds->address.sun_family = AF_UNIX;
    uds->address_length = sizeof(uds->address.sun_family) + 
      sprintf(uds->address.sun_path,(char*)uds->path);
    if (bind(uds->sockfd, (struct sockaddr *)&uds->address, uds->address_length) != 0)
    {
      printf("bind on uds (%s) failed\n",(char*)uds->path);
      return -1;
    }
    if (listen(uds->sockfd, 5) != 0)
    {
      printf("listen on uds (%s) failed\n",(char*)uds->path);
      return -1;
    }
    return 0;
  #else
    printf("no uds socket support on this system\n");
    return -1;
  #endif
}

struct trop_IPC *trop_IPCInit( struct trop_IPC *ipc )
{
  if (ipc==NULL)
  {
    ipc = (struct trop_IPC*)malloc(sizeof(struct trop_IPC));
  }
  ipc->type = TROP_IPC_NONE;
  ipc->sub = NULL;
  return ipc;
}

struct trop_NMsg *trop_NMsgInit( struct trop_NMsg *msg )
{
  if (msg == NULL)
  {
    msg = (struct trop_NMsg *)malloc(sizeof(struct trop_NMsg));
  }
  msg->lstID = NULL;
  msg->msg[0] = 0x00;
  msg->next = NULL;
  return msg;
}

int trop_NMsgAddID( struct trop_NMsg *msg, int id )
{
  int *pid = (int*)malloc(sizeof(int));
  *pid = id;
  msg->lstID = trop_ListAdd( msg->lstID, pid );
  return 0;
}

int trop_NMsgAddMsg( struct trop_NMsg *msg, char *text )
{
  strcpy(msg->msg,text);
  return 0;
}

struct trop_Action *trop_ActionInit( struct trop_Action *action )
{
  if (action == NULL)
  {
    action = (struct trop_Action*)malloc(sizeof(struct trop_Action));
  }
  action->msg[0] = 0x00;
  action->desc[0] = 0x00;
  action->about[0] = 0x00;
  action->image[0] = 0x00;
  action->lstID = NULL;
  return action;
}

int trop_ActionAddID( struct trop_Action *action, int id )
{
  int *pid = (int*)malloc(sizeof(int));
  *pid = id;
  action->lstID = trop_ListAdd( action->lstID, pid );
  return 0;
}

int trop_ActionTrigger( struct trop_Action *action, struct trop_Conn *conn )
{
  struct trop_List *current = action->lstID;
  struct trop_NMsg *msg = trop_NMsgInit(NULL);
  int pid;
  while(current != NULL)
  {
    memcpy(&pid,current->ptr,sizeof(int));
    trop_NMsgAddID(msg,pid);
    current = current->next;
  }
  if (action->msg[0] != 0x00)
    trop_NMsgAddMsg(msg,(char*)action->msg);
  trop_ConnAddMsg(conn,msg);
  trop_ConnSendMsgs(conn);
  return 0;
}
