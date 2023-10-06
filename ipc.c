/*
  * server which would handle the incoming connections
  
  * it would have an SHM to handle the communication of foreign
  nodes with the local applications
  
  * the SHM should only have a giant lock applied on the 'main
  pool', but the memory itself should not be locked, since each
  application would know it's pool position and would not modify
  the data exceeding it's scope
  
  * po pripade by isla vsetka komunikacia z opacnej strany - 
  aplikacia si vytvori SHM, informuje o tom lokalny server, 
  oblast bude serverom pri dalsej komunikacii vyuzivana na prenos
  sprav
  
  * node komunikacia moze byt 'execution orientated, alebo 
  resource orientated', pricom execution oriented comm. nebude
  hladiet natom na ktorom stroji sa vykona prikaz a resource
  oriented bude zavisiet od toho, ci a ktory node ma pristup
  k akym zdrojom a datam
  
  * bude dodrzovana stromova struktura, pricom kazdy node bude
  mat rodica a kazdy node bude most mat subnody - priama komunikacia
  medzi nodami nebude zpristupnena, vsetko pojde len cez hierarchiu

  * ak mam nejaky broadcast message, tak ho najprv vyslem medzi nody
  a az potom na rodica
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define STATIC
#define DEBUG		1
#define NAME		"IPC"
#define STDBUF		128
#define NFFILE		"ipc.conf"

typedef struct ipc_s_run
{
  char		paddr[STDBUF];
  char		pport[6];
  int		sock;
} s_parent;

typedef struct ipc_child
{
  
} ipc_child;

void ipc_get_parent( struct ipc_s_run *prun );
void ipc_init_network( struct ipc_s_run *prun );
void ipc_init( struct ipc_s_run *prun );
void ipc_deinit( struct ipc_s_run *prun );

void ipc_get_parent( struct ipc_s_run *prun )
{
  FILE *fd = NULL;
  if ( ( fd = fopen(NFFILE,"r") ) == NULL )
  {
    printf("%sE: could not open %s\n",NAME,NFFILE);
    exit(-1);
  }
  prun->paddr[0] = 0x00;
  prun->pport[0] = 0x00;
  char line[STDBUF];
  int i;
  while(1)
  {
    fgets((char*)line,STDBUF,fd);
    if (feof(fd)!=0)
      break;
    if (strlen((char*)line) > 0)
    {
      if (line[strlen((char*)line)-1] == '\n')
        line[strlen((char*)line)-1] = '\0';
    }
    if (DEBUG==1) 
      printf("%s: configuration line '%s'\n",NAME,(char*)line);
    void *ptr	= NULL;
    ptr = strstr((char*)line,"parent");
    if (ptr == (char*)line)
    {
      ptr = ptr+7;
      if (strlen((char*)ptr) > 0)
        strcpy((char*)prun->paddr,(char*)ptr);
      for (i = 0; i < strlen((char*)prun->paddr); i++)
      {
        if (prun->paddr[i] == ':')
          prun->paddr[i] = 0x00;
      }
      ptr = ptr+i;
      if (strlen((char*)ptr) > 0 && strlen((char*)ptr) < 6)
        strcpy((char*)prun->pport,(char*)ptr);
      else
      {
        printf("%sE: configuration line '%s' lacks server port\n",
          NAME,(char*)line);
        exit(-1);
      }
    }
  }
  if (DEBUG==1)
  {
    printf("%s: parent\t'%s'\n",NAME,(char*)prun->paddr);
    printf("%s: port\t'%s'\n",NAME,(char*)prun->pport);
  }
  fclose(fd);
}

void ipc_init_network( struct ipc_s_run *prun )
{
}

void ipc_init( struct ipc_s_run *prun )
{
  ipc_get_parent(prun);
  ipc_init_network(prun);
}

void ipc_deinit( struct ipc_s_run *prun )
{
}

#ifdef STATIC
  int main( )
  {
    struct ipc_s_run prun;
    ipc_init(&prun);
    return 0;
  }
#endif
