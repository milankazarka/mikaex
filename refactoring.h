/** Milan Kazarka 2011
 */
#ifndef _REFACTORING_H_
#define _REFACTORING_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <dirent.h>

#include "config.h"

#define _RENAME_SIZE 256
#define _RENAME_LINE 4096

typedef struct renameMember
{
  unsigned char from[_RENAME_SIZE];
  unsigned char to[_RENAME_SIZE];
  struct renameMember *next;
} renameMember;

class refactoringRename {
  public:
    struct renameMember *members;

    refactoringRename( );
    ~refactoringRename( );
    
    int addMember( unsigned char *from, unsigned char *to );
    int parse( unsigned char *from, unsigned char *to );
    int parseFile( unsigned char *path, unsigned char *pathTo );
};

#endif
