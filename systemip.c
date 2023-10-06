#include <stdio.h>      
#include <stdlib.h>
#include <string.h>
#include "config.h"

int systemipGet( unsigned char *ip )
{
  #ifdef _DEBUG
    printf("systemipGet\n");
  #endif
  
  if (!ip)
  {
    #ifdef _DEBUG
      printf("systemipGet ERROR invalid arguments\n");
    #endif
    return 1;
  }
  
  FILE * fp = popen("ifconfig", "r");
  if (fp) 
  {
    char *p=NULL, *e; 
    size_t n;
    while ((getline(&p, &n, fp) > 0) && p) 
    {
      if (p = strstr(p, "inet addr:")) 
      {
        p+=10;
        if (e = strchr(p, ' ')) 
        {
          *e='\0';
          #ifdef _DEBUG
            printf("systemipGet (%s)\n", p);
          #endif
          if (strcmp(p,"127.0.0.1")!=0)
          {
            strcpy(ip,p);
            #ifdef _DEBUG
              printf("systemipGet systemip(%s)\n",ip);
            #endif
          }
        }
      }
    }
    pclose(fp);
  }                                                                                                                                                                                                                                                
  
  return 0;
}
