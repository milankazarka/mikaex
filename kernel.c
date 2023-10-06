/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define _STATIC

#define KERN_MEMSEG  4096
#define KERN_MEMSIZE KERN_MEMSEG*2000
#define KERN_MEMONUM 1024

#define KOBJ_NAMELEN 32

struct kobj_lst *kobjLstAdd( struct kobj_lst *lst, void *ptr );
struct kobj *kobjNew( );
int kobjAddCh( struct kobj *obj, struct kobj *ch );
int kobjSetName( struct kobj *obj, char *name );
int kobjSetBody( struct kobj *obj, void *body );
int kobjStartChTokenizer( struct kobj *obj );
struct kobj *kobjGetChObj( struct kobj *obj, int id );
struct kobj *kobjGetChObjName( struct kobj *obj, char *name );

enum kobj_type {
  KOBJ_TYPE_NONE
};

enum kobj_activity {
  KOBJ_ACTIVE_NO,
  KOBJ_ACTIVE_YES
};

enum {
  TOKEN_ON,
  TOKEN_OFF
};

typedef struct kobj_lst
{
  void 			*ptr;
  struct kobj_lst	*next;
} kobj_lst;

/**
 *  by reverse lookup, the object itself does not
 *  emit a message, but holds it for other objects
 *  associated with the message type to fetch it
 */
enum {
  KRELAY_LOOKUP_NONE,
  KRELAY_LOOKUP_INVERSE,
  KRELAY_LOOKUP_REVERSE
};

/*
 *  you allocate a message descriptor and use it to
 *  identify emmited messages
 */
typedef struct kmsgDescr
{
  char			name[8]; 
} kmsgDescr;

typedef struct kmsg
{
  struct kmsgDescr	*kmsgDescr;
  void			*body;
} kmsg;

typedef struct krelay
{
  int			type; 		// KRELAY_LOOKUP_INVERSE, KRELAY_LOOKUP_REVERSE
  void			*msg;	  	// ptr to emited message
  int			nemit;		// no. emition
  struct kobj		*obj;		// associated object to emit to
} krelay;

typedef struct kobj
{
  int			id;
  int			active;
  short int		token;
  unsigned char 	name[KOBJ_NAMELEN];
  struct kobj_lst 	*ch;
  struct kobj_lst	*relay;
  void			*body;
  void			*(*_delete)(void*data);
  /**
   *  the _onemit is ran when an associated relay emits a message 
   *  and this object is associated to it
   */
  void			*(*_onemit)(void*data);
} kobj;

#define KMM_NONE 0
#define KMM_USED 1

struct k_mem_man
{
  unsigned char		state;
  int			size;
} k_mem_man;

struct kernel
{
  unsigned char		*kmemman;
  void			*mem;
  struct kobj_lst	*objs;
  void			*(*_new)(struct kernel *k, int size);
  void			*(*_free)(void*data);
  void			*(*_on_emit)(struct kernel *k, struct kmsg *msg, struct kobj *obj );
} kernel;

struct krelay *krelayNew( )
{
  struct krelay *kr;
  
  kr = (struct krelay*)malloc(sizeof(struct krelay));
  kr->type = KRELAY_LOOKUP_NONE;
  kr->msg = NULL;
  kr->nemit = 0;
  kr->obj = NULL;
  
  return kr;
}

int krelayEmit( struct kernel *k, struct krelay *kr, struct kmsg *msg )
{
  if (!kr||!msg) return -1;
  
  if (kr->nemit>999999) kr->nemit = 0;
  else kr->nemit++;
  
  kr->msg = msg;
  
  if ( kr->type == KRELAY_LOOKUP_REVERSE )
  {
    k->_on_emit(k,msg,NULL);
  }
  else if ( kr->type == KRELAY_LOOKUP_INVERSE )
  { }
  
  return 0;
}

struct kobj_lst *kobjLstAdd( struct kobj_lst *lst, void *ptr )
{
  struct kobj_lst *current;
  if (!ptr) return lst;
  if (lst==NULL)
  {
    lst = (struct kobj_lst*)malloc(sizeof(struct kobj_lst));
    lst->ptr = ptr;
    lst->next = NULL;
  }
  else
  {
    current = (struct kobj_lst*)malloc(sizeof(struct kobj_lst));
    current->ptr = ptr;
    current->next = lst;
    lst = current;
  }
  return lst;
}

struct kobj *kobjNew( )
{
  struct kobj *obj;
  obj = (struct kobj*)malloc(sizeof(struct kobj));
  obj->id 		= KOBJ_TYPE_NONE;
  obj->active		= KOBJ_ACTIVE_YES;
  obj->name[0] 		= 0x00;
  obj->ch 		= NULL;
  obj->relay		= NULL;
  obj->token 		= TOKEN_ON;
  obj->body		= NULL;
  obj->_delete		= NULL;
  return obj;
}

int kobjAddCh( struct kobj *obj, struct kobj *ch )
{
  if (!obj||!ch) return -1;
  obj->ch = kobjLstAdd(obj->ch,ch);
  return 0;
}

int kobjSetName( struct kobj *obj, char *name )
{
  if (!obj||!name) return -1;
  if (strlen(name)>=KOBJ_NAMELEN)
  {
    strncpy((char*)obj->name,name,KOBJ_NAMELEN-1);
    obj->name[KOBJ_NAMELEN-1] = 0x00;
  }
  else
    strcpy((char*)obj->name,name);
  return 0;
}

int kobjSetBody( struct kobj *obj, void *body )
{
  if (!obj||!body) return -1;
  obj->body = body;
  return 0;
}

int kobjSetId( struct kobj *obj, int id )
{
  if (!obj) return -1;
  obj->id = id;
  return 0;
}

int kobjStartChTokenizer( struct kobj *obj )
{
  struct kobj_lst *current;
  struct kobj *currento;
  
  if (!obj) return -1;
  
  current = obj->ch;
  while(current)
  {
    currento = current->ptr;
    currento->token = TOKEN_ON;
    current = current->next;
  }
  
  return 0;
}

struct kobj *kobjGetChObj( struct kobj *obj, int id )
{
  struct kobj_lst *current;
  struct kobj *currento;
  struct kobj *ch;
  
  if (!obj) return NULL;
  
  ch = NULL;
  current = obj->ch;
  while(current)
  {
    currento = current->ptr;
    if (
      currento->token==TOKEN_ON &&
      currento->id == id
    )
    {
      currento->token = TOKEN_OFF;
      ch = currento;
    }
    current = current->next;
  }
  
  return ch;
}

struct kobj *kobjGetChObjName( struct kobj *obj, char *name )
{
  struct kobj_lst *current;
  struct kobj *currento;
  struct kobj *ch;
  
  if (!obj||!name) return NULL;
  
  ch = NULL;
  current = obj->ch;
  while(current)
  {
    currento = current->ptr;
    if (currento->name[0]==0x00)
    {
      current = current->next;
      continue;
    }
    if (
      currento->token==TOKEN_ON &&
      strcmp((char*)currento->name,name) == 0
    )
    {
      currento->token = TOKEN_OFF;
      ch = currento;
    }
    current = current->next;
  }
  
  return ch;
}

void *_kernel_on_emit( struct kernel *k, struct kmsg *msg, struct kobj *obj )
{
  struct kobj_lst *current;
  struct kobj *currento;
  
  if (!obj)
  {
    current = k->objs;
    while(current)
    {
      currento = current->ptr;
      _kernel_on_emit(k,msg,currento);
      current = current->next;
    }
  }
  else
  {
    // process self
    current = obj->ch;
    while(current)
    {
      currento = current->ptr;
      // process child object
      _kernel_on_emit(k,msg,currento);
      current = current->next;
    }
  }
}

void *_kernel_new( struct kernel *k, int size )
{
  struct k_mem_man *kmm;
  int n;
  void *p;
  
  p = NULL;
  
  kmm = (struct k_mem_man*)k->kmemman;
  for(n = 0; n < KERN_MEMONUM; n++)
  {
    if (kmm->state == KMM_NONE)
    {
      goto FREE_MEMMAN;
    }
  }
  return NULL;
  
FREE_MEMMAN:
  
  return p;
}

struct kernel *kernelNew( )
{
  struct kernel *k;
  
  k = (struct kernel*)malloc(sizeof(struct kernel));
  k->mem 	= NULL;
  k->objs	= NULL;
  k->_new	= NULL;
  k->_free	= NULL;
  
  return k;
}

int kernelInit( struct kernel *k )
{
  int n;
  struct k_mem_man *kmm;

  if (!k) return -1;
  k->kmemman = malloc(sizeof(struct k_mem_man)*KERN_MEMONUM);
  k->mem = malloc(KERN_MEMSIZE);
  
  kmm = (struct k_mem_man*)k->kmemman;
  for(n = 0; n < KERN_MEMONUM; n++)
  {
    kmm->state = KMM_NONE;
    kmm += sizeof(struct k_mem_man);
  }
  
  k->_new = _kernel_new;
  k->_on_emit = _kernel_on_emit;
  
  return 0;
}

#ifdef _STATIC
  int main( )
  {
    struct kernel *k = kernelNew();
    kernelInit(k);
    
    return 0;
  }
#endif
