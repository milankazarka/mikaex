#include "refactoring.h"
#define _TEST_NO

refactoringRename::refactoringRename( )
{
  members = NULL;
}

refactoringRename::~refactoringRename( )
{
}

int refactoringRename::addMember( unsigned char *from, unsigned char *to )
{
  if (!from || !to)
  {
    #ifdef _DEBUG
      printf("refactoringMember::addMember\n");
    #endif
    return 1;
  }
  
  struct renameMember *member = (struct renameMember*)malloc(sizeof(struct renameMember));
  if (member)
  {
    strcpy((char*)member->from,(char*)from);
    strcpy((char*)member->to,(char*)to);
    member->next = members;
    members = member;
  }
  
  return 0;
}

int refactoringRename::parse( unsigned char *from, unsigned char *to )
{
  #ifdef _DEBUG
    printf("refactoringRename::parse\n");
  #endif
  
  if (!from || !to)
  {
    #ifdef _DEBUG
      printf("refactoringRename::parse error, invalid arguments\n");
    #endif
    return 1;
  }
  
  struct dirent *dp = NULL;
  DIR *dir = opendir((char*)from);
  unsigned char path[256];
  unsigned char pathTo[256];
  
  if (dir)
  {
    while((dp=readdir(dir)) != NULL)
    {
      if (dp->d_name[0]=='.')
        continue;
    
      sprintf((char*)path,"%s/%s",(char*)from,(char*)dp->d_name);
      sprintf((char*)pathTo,"%s/%s",(char*)to,(char*)dp->d_name);
      parseFile((unsigned char*)path,(unsigned char*)pathTo);
    }
    
    closedir(dir);
  }
  return 0;
}

/** parse a file at 'path' and save the output to 'pathTo'
 */
int refactoringRename::parseFile( unsigned char *path, unsigned char *pathTo )
{
  #ifdef _DEBUG
    printf("refactoringRename::parseFile\n");
  #endif
  
  if (!path || !pathTo)
  {
    #ifdef _DEBUG
      printf("refactoringRename::parseFile\n");
    #endif
    return 1;
  }
  
  int n, nn, hit, replaced;
  char *ptr = NULL;
  unsigned char line[_RENAME_LINE];
  unsigned char output[_RENAME_LINE];
  
  /** change the filename
   */
  replaced = 0;
  struct renameMember *current = members;
  while(current)
  {
    nn = 0;
    for(n = 0; n < strlen((char*)pathTo); n++)
    {
      hit = 0;
      ptr = (char*)pathTo+n;
      if (strlen((char*)pathTo)-n>strlen((char*)current->from))
      {
        if (
          strncmp(ptr,(char*)current->from,strlen((char*)current->from))==0
        )
        {
          replaced = 1;
          hit = 1;
          strncpy((char*)output+nn,(char*)current->to,strlen((char*)current->to));
          n+= strlen((char*)current->from);
          n--;
          nn+= strlen((char*)current->to);
        }
      }
      if ( hit == 0 )
      {
        output[nn] = pathTo[n];
        nn++;
      }
    }
    output[nn] = 0x00;
    if (replaced==1)
    {
      #ifdef _DEBUG
        printf("refactoringRename::parseFile outputFilename(%s) prototype(%s) size(%d)\n",
          (char*)output,(char*)pathTo,nn);
      #endif
      strcpy((char*)pathTo,(char*)output);
    }
    
    current = current->next;
  }
  
  /** change the content of the file
   */
  char *section = NULL;
  if (section = strstr((char*)path,"~")) return 0;
  if (
    !(section = strstr((char*)path,".c")) &&
    !(section = strstr((char*)path,".h")) &&
    !(section = strstr((char*)path,".cpp")) &&
    !(section = strstr((char*)path,".hh")) &&
    !(section = strstr((char*)path,".sh")) &&
    !(section = strstr((char*)path,"Makefile")) &&
    !(section = strstr((char*)path,".ini")) &&
    !(section = strstr((char*)path,".conf")) &&
    !(section = strstr((char*)path,".xml"))
  )
  {
    return 0;
  }
  
  FILE *out = fopen((char*)pathTo,"w");
  FILE *in = fopen((char*)path,"r");
  if (in)
  {
    #ifdef _DEBUG
      printf("refactoringRename::parseFile(%s) pathTo(%s)\n",
        (char*)path,(char*)pathTo);
    #endif
    while(fgets((char*)line,_RENAME_LINE,in) != NULL)
    {
      replaced = 0;
      nn = 0;
      for(n = 0; n < strlen((char*)line); n++)
      {
        hit = 0;
        #ifdef _TEST
          if (strlen((char*)line)-n>strlen("mika"))
          {
            if (strstr((char*)line,"mika"))
            {
              printf("%s",(char*)line);
              break;
            }
          }
        #endif
        
        struct renameMember *current = members;
        while(current)
        {
          ptr = (char*)line+n;
          if (strlen((char*)line)-n>strlen((char*)current->from))
          {
            if (
              strncmp(ptr,(char*)current->from,strlen((char*)current->from))==0
              )
            {
              replaced = 1;
              hit = 1;
              strncpy((char*)output+nn,(char*)current->to,strlen((char*)current->to));
              n+= strlen((char*)current->from);
              n--;
              nn+= strlen((char*)current->to);
              break;
            }
          }
          current = current->next;
        }
        if (hit==0)
        {
          output[nn] = line[n];
          nn++;
        }
      }
      output[nn] = 0x00;
      if (replaced==1)
      {
        #ifdef _DEBUG
          printf("replaced: %s",(char*)output);
        #endif
      }
      
      fwrite((char*)output,strlen((char*)output),1,out);
      
    }
    fclose(in);
  }
  if (out)
    fclose(out);
  
  return 0;
}

int main( )
{
  refactoringRename *rename = new refactoringRename();
  rename->addMember((unsigned char*)"mika",(unsigned char*)"mika");
  rename->addMember((unsigned char*)"Mika",(unsigned char*)"Mika");
  rename->addMember((unsigned char*)"MIKA",(unsigned char*)"MIKA");
  rename->parse((unsigned char*)".",(unsigned char*)"./refactoring");

  return 0;
}
