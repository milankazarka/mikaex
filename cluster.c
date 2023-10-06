/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

/*
  - managing the CSA kernel
  - relaying, parsing messages to CSAs
*/
#include "system.h"
#include "csa.h"
#include "xml.h"
#include "modules.h"

#define STATIC

#define LOCAL_DEBUG	TRUE

#define CELL_NAME_SIZE	128
#define CHSUM_SIZE	32
#define CLU_CFG		"cluster.xml"

typedef struct trop_Cell
{
  char			name[CELL_NAME_SIZE+1];
  char			chsum[CHSUM_SIZE+1];
  int			state;
  struct trop_Cell	*next;
} trop_Cell;

typedef struct trop_ClusterHost
{
  char			*host;
  int			port;
} trop_ClusterParent;

typedef struct trop_ClusterConn
{
  struct trop_Conn	*proto;
} trop_ClusterConn;

typedef struct trop_Cluster
{
  struct xml_Inst	xmlcfg;
  struct trop_ClusterHost
                        parent;
  struct trop_ClusterHost
                        host;
  struct trop_ClusterConn
                        conn;
  struct trop_Cell	*cell;
} trop_Cluster;

int _trop_xml_gethost_at_name( struct trop_ClusterHost *ch, struct xml_Inst *xml, char *name );
void *_cluster_parser( void *data );
struct trop_Cluster *myCluster( );
int trop_ClusterInit( struct trop_Cluster *clu );
struct trop_Cluster *new_tropCluster( );
int trop_ClusterCellAdd( struct trop_Cluster *clu, struct trop_Cell *cell );
int trop_CellRemoveAtName( struct trop_Cluster *clu, char *name );
struct trop_Cell *new_tropCell( );
int trop_CellSetName( struct trop_Cell *cell, char *name );

struct trop_Cluster *cluster;

void *_cluster_parser(void*data)
{
  char *str = (char*)data;
  if (LOCAL_DEBUG==TRUE) printf(":_cluster_parser '%s'\n",str);
}

struct trop_Cluster *myCluster( ) { return cluster; }

int _trop_xml_gethost_at_name( struct trop_ClusterHost *ch, struct xml_Inst *xml, char *name )
{
  struct xml_Object *host = xml_GetObject(xml,name);
  if (!host)
  {
  }
  else
  {
    struct xml_Bit *xbhost = xml_GetObjectBit(host,"host");
    if (!xbhost)
    {
      printf("err: could not determine host for '%s'\n",name);
      return -1;
    }
    else
    {
      ch->host = xbhost->value;
      if (LOCAL_DEBUG==TRUE) printf(":cfg cluster.host '%s'\n",xbhost->value);
    }
    struct xml_Bit *xbport = xml_GetObjectBit(host,"port");
    if (!xbport)
    {
      printf("err: could not determine host port for '%s'\n",name);
      return -1;
    }
    else
    {
      ch->port = atoi(xbport->value);
      if (LOCAL_DEBUG==TRUE) printf(":cfg cluster.port '%s'\n",xbport->value);
    }
  }
  return 0;
}

int trop_ClusterInit( struct trop_Cluster *clu )
{
  if (clu == NULL) return -1;
  strcpy((char*)clu->xmlcfg.filename,CLU_CFG);
  xml_Init(&clu->xmlcfg);
  
  _trop_xml_gethost_at_name(&clu->host,&clu->xmlcfg,"me");
  _trop_xml_gethost_at_name(&clu->host,&clu->xmlcfg,"parent");
  
  clu->cell = NULL;
  return 0;
}

struct trop_Cluster *new_tropCluster( )
{
  struct trop_Cluster *clu = (struct trop_Cluster*)malloc(sizeof(struct trop_Cluster));
  return clu;
}

int trop_ClusterCellAdd( struct trop_Cluster *clu, struct trop_Cell *cell )
{
  if (clu->cell == NULL)
  {
    cell->next = NULL;
    clu->cell = cell;
  }
  else
  {
    cell->next = clu->cell;
    clu->cell = cell;
  }
  return 0;
}

int trop_CellRemoveAtName( struct trop_Cluster *clu, char *name ) // todo, negative return
{
  struct trop_Cell *current = clu->cell;
  struct trop_Cell *last = NULL;
  while(current)
  {
    if (strlen(name) != strlen((char*)current->name)) continue;
    if (memcmp(name,(char*)current->name,strlen(name)) == 0)
    {
      if (last == NULL) 
      {
        clu->cell = current->next;
        free(current);
        current = clu->cell;
      }
      else
      {
        last->next = current->next;
        free(current);
        current = last;
      }
    }
    last = current;
    current = current->next;
  }
  return 0;
}

struct trop_Cell *new_tropCell( )
{
  struct trop_Cell *cell = (struct trop_Cell*)malloc(sizeof(struct trop_Cell));
  cell->name[0] = 0x00;
  cell->chsum[0] = 0x00;
  return cell;
}

int trop_CellSetName( struct trop_Cell *cell, char *name )
{
  memcpy((void*)cell->name,name,strlen(name));
  cell->name[strlen(name)] = 0x00;
  return 0;
}

#ifdef STATIC
  int main()
  {
    trop_Init();
    cluster = new_tropCluster();
    if ( trop_ClusterInit(myCluster()) == -1) return -1;
    struct trop_Cell *cell00 = new_tropCell();
    trop_ClusterCellAdd(myCluster(),cell00);
    
    // initialise a CSA kernel with 4 cores
    struct trop_Csa *csa01 = new_TropCsa();
    struct trop_Csa *csa02 = new_TropCsa();
    struct trop_Csa *csa03 = new_TropCsa();
    struct trop_Csa *csa04 = new_TropCsa();
    struct trop_CsaKernel *kernel = new_TropCsaKernel();
    trop_CsaKernelInit(kernel);
    trop_CsaInitEx(kernel,csa01);
    trop_CsaInitEx(kernel,csa02);
    trop_CsaInitEx(kernel,csa03);
    trop_CsaInitEx(kernel,csa04);
    trop_CsaKernelStart(kernel);

    struct trop_CsaTask *task = new_tropCsaTask();
    trop_CsaTaskInit(task,_cluster_parser,"some_data");
    trop_CsaKernelTaskAdd(kernel,task);
    
    while(1) 
    {
      usleep(5); 
    }
    
    return 0;
  }
#endif
