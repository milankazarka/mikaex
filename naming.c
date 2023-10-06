#include "system.h"
#include "csa.h"
#include "xml.h"

#define STATIC

#define CSANAMEGRPLEN	3
#define CSANAMELEN	12

typedef struct trop_CSALev
{
  char		id[CSANAMEGRPLEN];
} trop_CSALev;

typedef struct trop_CSANameLev
{
  char		id[CSANAMEGRPLEN];
} trop_CSANameLev;

typedef struct trop_CSAName
{
  char		id[CSANAMELEN];
  struct trop_CSANameLev
                lev[CSANAMELEN/CSANAMEGRPLEN];
} trop_CSAName;

struct trop_CSAName *new_tropCSAName( );
struct trop_CSAName *trop_CSANameInit( struct trop_CSAName *name );
int trop_CSANameSetup( struct trop_CSAName *name, char *strname );
BOOL trop_CSANameIsLocal( struct trop_CSAName *name );
BOOL trop_CSANameIsLEV( struct trop_CSAName *name, char *grp );
BOOL trop_CSANameMatchCSA( struct trop_CSAName *name, struct trop_Csa *csa );

struct trop_CSAName *new_tropCSAName( )
{
  struct trop_CSAName *name = (struct trop_CSAName*)malloc(sizeof(struct trop_CSAName));
  if (name) return name;
  else printf(":error\n"); // todo termination in system
}

struct trop_CSAName *trop_CSANameInit( struct trop_CSAName *name )
{
  if (!name)
    name = new_tropCSAName( );
  name->id[0] = 0x00;
  int i;
  for (i = 0; i < CSANAMELEN / CSANAMEGRPLEN; i++)
  {
    name->lev[i].id[0] = 0x00;
  }
  return name;
}

BOOL trop_CSANameIsLocal( struct trop_CSAName *name )
{
  int i, ii;
  BOOL ret = TRUE;
  for (i = 0; i < CSANAMELEN / CSANAMEGRPLEN; i++)
  {
    for (ii = 0; ii < CSANAMEGRPLEN; i++)
      if (name->lev[i].id[ii] != ' ') ret = FALSE;
  }
  return ret;
}

void _print_levs( struct trop_CSAName *name )
{
  printf("levs:\n");
  int i, ii;
  for (i = 0; i < CSANAMELEN / CSANAMEGRPLEN; i++)
  {
    printf(" lev '");
    for (ii = 0; ii < CSANAMEGRPLEN; ii++)
      putchar(name->lev[i].id[ii]);
    printf("'\n");
  }
}

int trop_CSANameSetup( struct trop_CSAName *name, char *strname )
{
  if (!strname) return -1;
  strncpy((char*)name->id,strname,CSANAMELEN);
  int i;
  void *p = NULL;
  for (i = 0; i < CSANAMELEN / CSANAMEGRPLEN; i++)
  {
    p = name->id+(i*CSANAMEGRPLEN);
    strncpy((char*)name->lev[i].id,p,CSANAMEGRPLEN);
  }
  return 0;
}

/*
  we push the grp lev higher - we are getting ready for a relay
*/
int trop_CSANamePushLEV( struct trop_CSAName *name )
{
  int i, ii;
  BOOL push = FALSE;
  for (i = 0; i < CSANAMELEN / CSANAMEGRPLEN; i++)
  {
    for (ii = 0; ii < CSANAMEGRPLEN; ii++)
    {
      if (name->lev[i].id[ii] != '0')
      {
        name->lev[i].id[ii] = '0';
        push = TRUE;
        continue;
      }
      if (name->lev[i].id[ii] != '0') 
        name->lev[i].id[ii] = '0';
    }
    if (push==TRUE) return 0;
  }
  return 0;
}

BOOL trop_CSANameIsLEV( struct trop_CSAName *name, char *grp )
{
  int i;
  for (i = 0; i < CSANAMELEN / CSANAMEGRPLEN; i++)
  {
    if (strncmp((char*)name->lev[i].id,grp,CSANAMEGRPLEN) == 0) 
      return TRUE;
  }
  return FALSE;
}

BOOL trop_CSANameMatchCSA( struct trop_CSAName *name, struct trop_Csa *csa )
{
  if (!name || &csa) return FALSE;
  if (strncmp((char*)csa->id,(char*)name->lev[CSANAMELEN/CSANAMEGRPLEN-1].id,
    CSANAMEGRPLEN) == 0)
    return TRUE;
  return FALSE;
}

int _process_csa_name( struct trop_CSALev *lev, struct trop_CSAName *name )
{
  if (!lev || !name) return -1;
  char sendto[3];
  int i, ii;
  BOOL local = TRUE;
  memset((void*)sendto,'0',CSANAMEGRPLEN);
  
  printf(":_process_csa_name\n");
  _print_levs(name);
  
  for (i = 0; i < CSANAMELEN / CSANAMEGRPLEN; i++)
  {
    for (ii = 0; ii < CSANAMEGRPLEN; ii++)
    {
      if (name->lev[i].id[ii] != '0')
      {
        if (i+1 != CSANAMELEN / CSANAMEGRPLEN) local = FALSE;
        if (memcmp((void*)lev->id,(void*)name->lev[i].id,CSANAMEGRPLEN) == 0)
        {
          memcpy((void*)sendto,(void*)name->lev[i].id,CSANAMEGRPLEN);
          break;
        }
      }
    }
  }
  if (local == FALSE) // relay
  {
    if (memcmp((void*)sendto,(void*)lev->id,CSANAMEGRPLEN) != 0)
    {
      printf(":bad relay through us\n");
      return -1;
    }
    printf(":relay '");
    for (i = 0; i < CSANAMEGRPLEN; i++) putchar(sendto[i]);
    printf("'\n");
    return 0;
  }
  return 0;
}

#ifdef STATIC
  int main( )
  {
    struct trop_CSAName *name = trop_CSANameInit(NULL);
    trop_CSANameSetup(name,"000000AB01A0");
    _print_levs(name);
    //trop_CSANamePushLEV(name);
    _print_levs(name);
    struct trop_CSALev *lev = (struct trop_CSALev*)malloc(sizeof(struct trop_CSALev));
    memcpy((void*)lev->id,"AB0",3);
    _process_csa_name(lev,name);
    return 0;
  }
#endif
