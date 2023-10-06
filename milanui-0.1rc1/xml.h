#ifndef _XML_H_
#define _XML_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

#include <sys/types.h>

#ifndef _FALSE
  #define _FALSE 0
#endif
#ifndef _TRUE
  #define _TRUE 1
#endif

typedef unsigned char _BOOL;

typedef struct xml_Bit
{
  char			name[64];
  char			value[64];
  struct xml_Bit	*next;
} xml_Bit;

typedef struct xml_Object
{
  struct xml_Bit	*bit; // object settings
  char			name[64];
  _BOOL			bfinished;
  struct xml_Object 	*parent;
  struct xml_Object	*next;
} xml_Object;

typedef struct xml_Inst
{
  char			filename[128];
  FILE			*fd;
  char			*data;	// if we parse the data directly, not from file
  struct xml_Object	*obj;
} xml_Inst;

typedef struct xml_Hierarchy
{
  struct xml_Object	*obj;
  struct xml_Hierarchy 	*next;
} xml_Hierarchy;

int xml_Init( struct xml_Inst *x );
int xml_Free( struct xml_Inst *x );
struct xml_Object *xml_GetObject( struct xml_Inst *x, char *name );
struct xml_Bit *xml_GetObjectBit( struct xml_Object *o, char *name );

#endif
