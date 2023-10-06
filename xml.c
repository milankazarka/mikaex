/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "xml.h"
#include "config.h"
#define STATIC_NO

#ifndef FALSE
 #define FALSE 0
#endif
#ifndef TRUE
 #define TRUE 1
#endif

int xml_Init( struct xml_Inst *x )
{
  if (x->filename[0]!=0x00)
  {
   if (!(x->fd = fopen((char*)x->filename,"r")))
   {
     printf(":Could not open file '%s'\n",(char*)x->filename);
     return -1;
   }
  }
  x->obj = NULL;
  struct xml_Object *parent 	= NULL;
  struct xml_Object *current 	= NULL;
  struct xml_Bit *bit		= NULL;
  struct xml_Hierarchy *hy	= NULL;
  struct xml_Hierarchy *chy	= NULL;
  char *data = NULL;		// not freed
  if (x->filename[0]==0x00) data = x->data;
  char line[256];
  int size;
  int i, ii, start, end;
  current = NULL;
  while(1)
  {
    if (x->filename[0]!=0x00)
    {
     fgets((char*)line,256,x->fd);
     if (feof(x->fd)) break;
    }
    else // directly parsed data
    {
     if (data==NULL) break;
     i = 0;
     while(memcmp(data+i,"\n",1)!=0)
     {
      i++;
     }
     i++;
     strncpy((char*)line,data,i);
     line[i-1] = 0x00;
     if (data[i]==0x00) data = NULL;
     else data += i;
    }
    size = strlen((char*)line);
    #ifdef _DEBUG
     printf(":size '%d'\n",size);
    #endif
    if (i==0)
    {
     if (size>0)
     {
      if (line[0]=='#')
      {
       continue;
      }
     }
    }
    start 	= -1;
    end		= -1;
    for (i = 0; i < size; i++)
    {
      if (line[i] == '<') // start of object
      {
        if (line[i+1] == '/')
        {
          parent = NULL;
          current = NULL;
          continue;
        }      
      
        if (parent == NULL)
          parent = current;
      
        if (x->obj == NULL)
        {
          x->obj = (struct xml_Object*)malloc(sizeof(struct xml_Object));
          current = x->obj;
          current->next = NULL;
        }
        else
        {
          current = (struct xml_Object*)malloc(sizeof(struct xml_Object));
          current->next = x->obj;
          x->obj = current;
        }
        current->name[0] = 0x00;
        current->bit = NULL;
        current->parent = parent;
        current->bfinished = FALSE;
      }
      if (current != NULL)
      {
        if (line[i] != '<' && line[i] != ' ' 
          && line[i] != '\t' && start == -1)
        {
          start = i;
        }
        if (start != -1 && end == -1 &&
          (line[i] == ' ' || line[i] == '\t' || line[i] == '>'))
        {
          end = i;
          strncpy(current->name,line+start,end-start); // object name
          current->name[end-start] = 0x00;
        }
        if (start != -1 && end != -1 && current->name[0] != 0x00 &&
          line[i] != ' ' && line[i] != '\t' && line[i] != '>') // object attr 'bits'
        {
          bit = NULL;
          for (ii = i; ii < size; ii++)
          {
            if (line[ii] == ' ' || line[ii] == '\t' || line[ii] == '>')
            {
              if (current->bit == NULL)
              {
                printf(":first bit\n");
                current->bit = (struct xml_Bit*)malloc(sizeof(struct xml_Bit));
                bit = current->bit;
                bit->next = NULL;
                bit->name[0] = 0x00;
                bit->value[0] = 0x00;
              }
              else
              {
                bit = (struct xml_Bit*)malloc(sizeof(struct xml_Bit));
                bit->next = current->bit;
                bit->name[0] = 0x00;
                bit->value[0] = 0x00;
                current->bit = bit;
              }
              break;
            }
          }
          if (bit != NULL && bit->name[0] == 0x00 && bit->value[0] == 0x00)
          {
            start = i;
            for (1; i < ii; i++)
            {
              if (line[i] == '=')
              {
                strncpy(bit->name,line+start,i-start);
                bit->name[i-start] = 0x00;
                #ifdef _DEBUG
                 printf("\t:bit name'%s' (%d-%d)\n",bit->name,start,i);
                #endif
              }
              else if (line[i] == '\"' && bit->value[0] == 0x00 &&
                start != i+1)
              {
                start = i+1;
                bit->value[0] = 0x0a;
              }
              else if (line[i] == '\"' && bit->value[0] == 0x0a)
              {
                strncpy(bit->value,line+start,i-start);
                bit->value[i-start] = 0x00;
                #ifdef _DEBUG
                 printf("\t:bit value '%s'\n",bit->value);
                #endif
              }
            }
            i = ii;
          }
        }
        if (start != -1 && end != -1)
        {
         #ifdef _DEBUG
          printf(":object at %d-%d '%s'\n",start,end,current->name);
         #endif
        }
      }
    }
  }
  #ifdef _DEBUG
   printf(":reconstruct\n");
   current = x->obj;
   while(current != NULL)
   {
     printf(":obj %s\n",current->name);
     if (current->parent != NULL)	
     {
       printf("\t:parent %s\n",current->parent->name);
     }
     bit = current->bit;
     while(bit != NULL)
     {
       printf("\t:bit '%s' '%s'\n",bit->name,bit->value);
       bit = bit->next;
     }
     current = current->next;
   }
  #endif
  return 0;
}

/*
 free the content of the xml_Inst 
*/
int xml_Free( struct xml_Inst *x )
{
 static struct xml_Object *cobj		= NULL;
 static struct xml_Object *_cobj	= NULL;
 static struct xml_Bit *cbit		= NULL;
 static struct xml_Bit *_cbit		= NULL;
 if (!x) return -1;
 cobj = x->obj;
 while(cobj != NULL)
 {
  _cobj = cobj->next;
  cbit = cobj->bit;
  while(cbit != NULL)
  {
   _cbit = cbit->next;
   free(cbit);
   cbit = _cbit;
  }
  cobj->bit = NULL;
  cobj->parent = NULL;
  free(cobj);
  cobj = _cobj;
 }
 x->obj = NULL;
 x->data = NULL; // not freed
 return 0;
}

struct xml_Object *xml_GetObject( struct xml_Inst *x, char *name )
{
  struct xml_Object *current = x->obj;
  while(current != NULL)
  {
    if (strcmp(current->name,name) == 0)
    {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

struct xml_Bit *xml_GetObjectBit( struct xml_Object *o, char *name )
{
  struct xml_Bit *current = o->bit;
  while(current != NULL)
  {
    if (strcmp(current->name,name) == 0)
    {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

#ifdef STATIC
 int main( )
 {
   printf(":xml\n");
   struct xml_Inst x;
   strcpy((char*)x.filename,"config.xml");
   xml_Init(&x);
   struct xml_Object *o = NULL;
   o = xml_GetObject(&x,"some");
   if (o != NULL)
   {
     struct xml_Bit *b = xml_GetObjectBit(o,"id");
     if (b != NULL)
     {
       printf("\t\t:bitc '%s'\n",b->value);
     }
   }
   return 0;
 }
#endif
