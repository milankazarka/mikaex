/** this utility generates and executes the command
 *  to pack the object files listed in objects.list
 *  into a single archive file
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define _PACKER_LINE 256
#define _MAX_OBJECTS 1000
#ifndef _DEBUG
  #define _DEBUG
#endif

typedef struct excludeObject {
  unsigned char name[256];
  struct excludeObject *next;
} excludeObject;

struct excludeObject *excludeObjects = NULL;

/** check if an object belongs to the excluded objects 
 *  (exclude.list)
 */
int isObjectExcluded( unsigned char *name )
{
  struct excludeObject *current = excludeObjects;
  while(current)
  {
    if (strcmp((char*)current->name,(char*)name)==0)
      return 1;
    current = current->next;
  }
  return 0;
}

int main( )
{
  /** generate a list of excluded objects
   */
  unsigned char line[_PACKER_LINE];
  FILE *fd = NULL;
  fd = fopen("exclude.list","r");
  if (fd)
  {
    while(1)
    {
      fgets((char*)line,_PACKER_LINE-1,fd);
      if (feof(fd)) break;
    
      if (line[strlen((char*)line)-1]=='\n')
        line[strlen((char*)line)-1] = 0x00;
      
      struct excludeObject *object = (struct excludeObject*)malloc(sizeof(struct excludeObject));
      object->next = excludeObjects;
      excludeObjects = object;
      strcpy((char*)object->name,(char*)line);
    }
    fclose(fd);
  }

  unsigned char command[_PACKER_LINE*_MAX_OBJECTS];
  fd = fopen("objects.list","r");
  if (fd)
  {
    strcpy((char*)command,"ar rvs libMika.a ");
    while(1)
    {
      fgets((char*)line,_PACKER_LINE-1,fd);
      if (feof(fd)) break;
    
      if (line[strlen((char*)line)-1]=='\n')
        line[strlen((char*)line)-1] = 0x00;
      
      if (isObjectExcluded((unsigned char*)line)==0)
      {
        #ifdef _DEBUG
          printf("packArchive object(%s)\n",(char*)line);
        #endif
        strcat((char*)command,(char*)line);
        strcat((char*)command," ");
      }
      else
      {
        #ifdef _DEBUG
          printf("packArchive\t\t\texclude(%s)\n",(char*)line);
        #endif
      }
    }
    #ifdef _DEBUG
      printf("packArchive command(%s)\n",(char*)command);
    #endif
    system((char*)command);
    
    fclose(fd);
  }
  return 0;
}
