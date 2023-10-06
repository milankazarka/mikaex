#include "basic.h"
#include "config.h"

struct basic *basicInit( struct basic *b )
{
  printf("basicInit\n");
  
  if (!b)
    b = (struct basic*)malloc(sizeof(struct basic));
  
  b->atoms = NULL;
  
  unsigned char line[256];
  unsigned char key[256];
  unsigned char value[256];
  FILE *fd = NULL;
  fd = fopen("basic.ini","r");
  if (fd)
  {
    printf("basicInit file ok\n");
    while(1)
    {
      fgets((char*)line,(255),fd);
      if (feof(fd))
      {
        fclose(fd);
        break;
      }
      
      if (line[strlen((char*)line)-1]=='\n')
        line[strlen((char*)line)-1] = 0x00;
      printf("basicInit line(%s)\n",(char*)line);
      sscanf((char*)line,"%s = %s",&key,&value);
      printf("basicInit key(%s) value(%s)\n",
        (char*)key,(char*)value);
      
      struct basicAtom *atom = (struct basicAtom*)malloc(sizeof(struct basicAtom));
      atom->next = b->atoms;
      b->atoms = atom;
      strcpy((char*)atom->key,(char*)key);
      strcpy((char*)atom->value,(char*)value);
    }
  }
  
  return b;
}

unsigned char *basicGet( struct basic *b, unsigned char *key )
{
  #ifdef _DEBUG
    printf("basicGet\n");
  #endif
  unsigned char *value = NULL;
  
  if (!b || !key)
  {
    #ifdef _DEBUG
      printf("basicGet ERROR invalid arguments\n");
    #endif
    return NULL;
  }
  
  struct basicAtom *current = b->atoms;
  while(current)
  {
    if (
      strlen((char*)current->key)==
      strlen((char*)key)
    )
    {
      if (
        strcmp((char*)current->key,(char*)key)==0
      )
      {
        return (unsigned char*)current->value;
      }
    }
    current = current->next;
  }
  
  return value;
}
