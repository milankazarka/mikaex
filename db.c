/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "system.h"

#define LOCAL_DEBUG 	TRUE

void *_listAdd( void *data )
{
  struct sListData *listData = (struct sListData*)data;
  struct sList *current = listData->list;
  while( current->next != NULL )
  {
    current = current->next;
  }
  if ( current->ptr == NULL )
  {
    current->ptr = listData->ptr;
  }
  else
  {
    current->next = (struct sList *)malloc(sizeof(struct sList));
    current = current->next;
    current->next = NULL;
    current->ptr = listData->ptr;
  }
}

void *_newList( )
{
  struct sList *list = (struct sList *)malloc(sizeof(struct sList));
  list->add = (void*)_listAdd;
  list->next = NULL;
  list->ptr = NULL;
  return (void*)list;
}

void _testList( )
{
  struct sList *list = _newList();
  struct sListData *data = NULL;
}

void tree_initMember( struct sMember *member )
{
  int i;
  for ( i = 0; i < SYMBOLS; i++ )
  {
    member->symbols[i] = NULL;
  }
  member->value = NULL;
  member->parent = NULL;
  member->pval = NULL;
}

void tree_setMemberParent( struct sMember *member, struct sMember *parent )
{
  if (parent)
    member->parent = parent;
}

void tree_setMemberPVAL( struct sMember *member, struct sParentValues *pval )
{
  if (member && pval)
    member->pval = pval;
}

void tree_freeMember( struct sMember *member )
{
  int i;
  for ( i = 0; i < SYMBOLS; i++ )
  {
    if ( member->symbols[i] != NULL )
    {
      tree_freeMember( member->symbols[i] );
      member->symbols[i] = NULL;
    }
  }
  if ( member->value != NULL )
  {
    free( member->value );
  }
  free(member);
}

void tree_printAllValues( struct sMember *member )
{
  int i;
  for ( i = 0; i < SYMBOLS; i++ )
  {
    if ( member->symbols[i] != NULL )
    {
      tree_printAllValues( member->symbols[i] );
    }
  }
  if ( member->value != NULL )
  {
    if ( LOCAL_DEBUG == TRUE ) printf("db: value '%s'\n",member->value);
  }
}

BOOL tree_addBound( struct sMember *member, char *key, char *value )
{
  int i;
  struct sMember *current = member;
  for ( i = 0; i < strlen(key); i++ )
  {
    if ( current->symbols[key[i]] == NULL )
    {
      current->symbols[key[i]] = (struct sMember*)malloc(sizeof(struct sMember));
      tree_initMember( current->symbols[key[i]] );
    }
    current = current->symbols[key[i]];
  }
  if ( current->value != NULL )
  {
    if ( LOCAL_DEBUG == TRUE ) printf("db: key %s already present\n", key);
    return FALSE;
  }
  char *p = current->value;
  p = (char*)malloc(sizeof(char)*(strlen(value)+1));
  strncpy((char*)p,value,strlen(value));
  p[strlen(value)] = '\0';
  return TRUE;
}

BOOL tree_removeBound( struct sMember *member, char *key )
{
  int i;
  struct sMember *current = member;
  for ( i = 0; i < strlen(key); i++ )
  {
    if ( current->symbols[key[i]] != NULL )
    {
      current = current->symbols[key[i]];
    }
    else
    {
      return FALSE;
    }
  }
  if ( current->value == NULL )
  {
    return FALSE;
  }  
  free(current->value);
  current->value = NULL;
  return TRUE;
}

BOOL tree_printBound( struct sMember *member, char *key )
{
  int i;
  struct sMember *current = member;
  for ( i = 0; i < strlen(key); i++ )
  {
    if ( current->symbols[key[i]] != NULL )
    {
      current = current->symbols[key[i]];
    }
    else
    {
      return FALSE;
    }
  }
  if ( current->value != NULL )
  {
    if ( LOCAL_DEBUG == TRUE ) printf("db: %s %s\n",key, current->value);
  }
  else
  {
    if ( LOCAL_DEBUG == TRUE ) printf("db: key %s not present\n",key);
  }
  return TRUE;
}

BOOL tree_getBoundsFromFileEX( struct sMember *member, char *filename )
{
  FILE *fd = NULL;
  char line[BASESIZE];
  char key[BASESIZE];
  char value[BASESIZE];
  int i, ii, posname;
  if ( (fd = fopen(filename, "r")) == NULL )
  {
    if ( LOCAL_DEBUG == TRUE ) printf("db: file %s not found\n", filename);
    return FALSE;
  }
  while( fgets((char *)line, BASESIZE, fd) != NULL )
  {
    value[0] = '\0';
    if ( line[strlen(line)-1] == '\n' )
    {
       line[strlen(line)-1] = '\0';
    }
    if ( LOCAL_DEBUG == TRUE ) printf("db: line '%s'\n", (char *)line);
    for (i = 0; i < strlen((char*)line); i++)
    {
      if (line[i]==':')
      {
        if (strlen((char*)line) != i)
        {
          if (line[i+1] != '\n' && line[i+1] != ' ' && line[i+1] != '\t')
          {
            for (ii = i+1; ii < strlen((char*)line); ii++)
            {
              if (line[ii] == ':') continue;
              if (line[ii] == '\n')
              {
                strcpy((char*)value,(char*)line+i+1);
                //printf();
              }
            }
          }
        }
      }
    }
  }
  return TRUE;
}

BOOL tree_getBoundsFromFile( struct sMember *member, char *filename )
{
  FILE *fd = NULL;
  char line[BASESIZE];
  char key[BASESIZE];
  char value[BASESIZE];
  int i, i2, ii;
  if ( (fd = fopen(filename, "r")) == NULL )
  {
    if ( LOCAL_DEBUG == TRUE ) printf("db: file %s not found\n", filename);
    return FALSE;
  }
  while( fgets((char *)line, BASESIZE, fd) != NULL )
  {
    if ( line[strlen(line)-1] == '\n' )
    {
       line[strlen(line)-1] = '\0';
    }
    if ( LOCAL_DEBUG == TRUE ) printf("db: line '%s'\n", (char *)line);
    for ( i = 0; i < strlen((char *)line); i++ )
    {
      if ( line[i] == ' ' )
      {
        i2 = i;
        continue;
      }
      key[i] = line[i];
    }
    key[i2] = '\0';
    i2++;
    ii = 0;
    for ( i = i2; i < strlen((char *)line); i++ )
    {
      value[ii] = line[i];
      ii++;
    }
    value[ii] = '\0';
    tree_addBound(member, (char *)key, (char *)value);
  }
  fclose(fd);
  return TRUE;
}

#ifdef STATIC
  void test( )
  {
    struct sMember *member = (struct sMember*)malloc(sizeof(struct sMember));
    tree_initMember(member);
    tree_getBoundsFromFile(member, "bounds");
    
    tree_getBoundsFromFile(member, "bounds2");
    
    tree_addBound(member, "home", "123");
    tree_addBound(member, "home", "234");
    tree_addBound(member, "home2", "321");
    tree_removeBound(member, "home2");
    tree_printBound(member, "home");
    tree_printBound(member, "home2");
    tree_printBound(member, "somethingelseandother");
    tree_printAllValues(member);
    tree_freeMember(member);
  }

  int main( )
  {
    int i;
    for (i = 0; i < 1; i++)
    {
      printf("@%d\n",i);
      test( );
      usleep(2000);
    }
    return 0;
  }
#endif
