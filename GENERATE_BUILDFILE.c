#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define _CONFIG		"GENERATE_BUILDFILE.ini"

#define _CC_COMPILER 	"sources_cc_compiler.in"
#define _CC_LINKER	"sources_cc_linker.in"
#define _CC_FLAGS	"cc_flags.in"
#define _C_COMPILER	"sources_c_compiler.in"
#define _C_LINKER	"sources_c_linker.in"
#define _C_FLAGS	"c_flags.in"

#define _INPUT 		"sources.in"
#define _FILENAME	256
#define _TMP		4096
#define _LINELEN 	4096
#define _DEBUG

unsigned char cc_compiler_flags[_TMP];
unsigned char cc_linker_flags[_TMP];
unsigned char c_compiler_flags[_TMP];
unsigned char c_linker_flags[_TMP];

typedef struct generateBuildfile {
  unsigned char in_cc_compiler[_FILENAME];
  unsigned char in_cc_linker[_FILENAME];
  unsigned char in_cc_flags[_FILENAME];
  unsigned char in_c_compiler[_FILENAME];
  unsigned char in_c_linker[_FILENAME];
  unsigned char in_c_flags[_FILENAME];
} generateBuildfile;

struct generateBuildfile *generateBuildfileInstance = NULL;

void readFlags( unsigned char *filename, unsigned char *storage )
{
  FILE *in = NULL;
  unsigned char line[_LINELEN];
  if ((in = fopen((char*)filename,"r")))
  {
    while(1)
    {
      fgets((char*)line,_LINELEN-1,in);
      if (feof(in)) break;
    
      if (line[strlen((char*)line)-1]=='\n')
        line[strlen((char*)line)-1] = 0x00;
    }
  }
}

void processItem( unsigned char *line, unsigned char *key, unsigned char *storage )
{
  
}

int readConfig( )
{
  FILE *in = NULL;
  unsigned char line[_LINELEN];
  
  generateBuildfileInstance = (struct generateBuildfile*)malloc(
    sizeof(struct generateBuildfile)
    );
  generateBuildfileInstance->in_cc_compiler[0] = 0x00;
  generateBuildfileInstance->in_cc_linker[0] = 0x00;
  generateBuildfileInstance->in_cc_flags[0] = 0x00;
  generateBuildfileInstance->in_c_compiler[0] = 0x00;
  generateBuildfileInstance->in_c_linker[0] = 0x00;
  generateBuildfileInstance->in_c_flags[0] = 0x00;
  
  if ((in = fopen((char*)_CONFIG,"r")))
  {
    while(1)
    {
      fgets((char*)line,_LINELEN-1,in);
      if (feof(in)) break;
      if (line[strlen((char*)line)-1]=='\n')
        line[strlen((char*)line)-1] = 0x00;
      
      processItem(
        (unsigned char*)line,(unsigned char*)"cc_compiler",
        (unsigned char*)generateBuildFileInstance->in_cc_compiler);
      processItem(
        (unsigned char*)line,(unsigned char*)"cc_linker",
        (unsigned char*)generateBuildFileInstance->in_cc_linker);
      processItem(
        (unsigned char*)line,(unsigned char*)"cc_flags",
        (unsigned char*)generateBuildFileInstance->in_cc_flags);
      processItem(
        (unsigned char*)line,(unsigned char*)"c_compiler",
        (unsigned char*)generateBuildFileInstance->in_c_compiler);
      processItem(
        (unsigned char*)line,(unsigned char*)"c_linker",
        (unsigned char*)generateBuildFileInstance->in_c_linker);
      processItem(
        (unsigned char*)line,(unsigned char*)"c_flags",
        (unsigned char*)generateBuildFileInstance->in_c_flags);
    }
  }
  return 0;
}

int main( int argc, char *argv[] )
{
  FILE *in = NULL;
  unsigned char line[_LINELEN];
  
  readConfig();
  
  if ((in = fopen((char*)_INPUT,"r")))
  {
    while(1)
    {
      fgets((char*)line,_LINELEN-1,in);
      if (feof(in)) break;
      
      if (line[strlen((char*)line)-1]=='\n')
      {
        line[strlen((char*)line)-1] = 0x00;
        #ifdef _DEBUG
          printf("main line(%s)\n",(char*)line);
        #endif
        processItem((unsigned char*)line);
      }
    }
  }
  
  return 0;
}
