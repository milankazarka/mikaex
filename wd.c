/*
  mika - a very simple way of how to monitor the resources
  of a child process/thread in Linux 
*/
#include "system.h"

#define STATIC

typedef struct sWD
{
  void		*(*call)(void*data);
  int		type; // 0 = process, 1 = thread
  pid_t		id;
} sWD;

#ifdef STATIC
  void *proc00(void *data)
  {
    printf(":child\n");
    while(1)
    {
      void *p = malloc(400);
      usleep(200000);
    }
    sleep(20000);
  }
#endif

int _getValue( char *line, char *name )
{
  int hit,i;
  char value[12];
  if (strncmp(line,name,strlen(name)) == 0)
  {
    hit = 0;
    for (i = 0; i < strlen(line); i++)
    {
      if ((line[i] == ' ' || line[i] == '\t') && hit == 0)
        hit = 1;
      else if ((line[i] != ' ' && line[i] != '\t') && hit == 1)
        hit = i;
      else if ((line[i] == ' ' || line[i] == '\t') && hit > 1)
      {
        strncpy(value,line+hit,i-hit);
        value[i-hit] = '\0';
        return(atoi(value));
      }
    }
  }
  return -1;
}

int sWDInit( struct sWD *wd )
{
  int id;
  pthread_t t;
  if (wd->type == 0)
  {
    id = fork( );
  }
  else
  {
    pthread_create(&t,NULL,wd->call,wd);
    id = 1;
  }
  if (id == 0) // child
  {
    wd->call(wd);
    exit(0);
  }
  else // parent
  {
    if (id == 1 && wd->type == 1)
      id = getpid();
      
    printf(":watching at '%d'\n",id);
    char line[64];
    int value;
    FILE *status = NULL;
    sprintf((char*)line,"/proc/%d/status",id);
    if ((status = fopen((char*)line,"r")) == NULL)
    {
      printf(":cannot open file '%s'\n",line);
      exit(0);
    }
    while(status != NULL)
    {
      while(1)
      {
        fgets((char*)line,64,status);
        if (feof(status))
        {
          break;
        }
        if (strlen((char*)line) == 0)
        {
          status = NULL;
          break;
        }
        //printf(":processing %s",line);
        value = _getValue(line,"VmRSS:");
        if (value != -1) printf(":VmRSS: %d\n",value);
      }
      rewind(status);
      sleep(1);
    }
    fclose(status);
  }
  return 0;
}

#ifdef STATIC
  int main( ) {
    struct sWD wd;
    wd.call = proc00;
    wd.type = 1;
    sWDInit(&wd);
    return 0;
  }
#endif
