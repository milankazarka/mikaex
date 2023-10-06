#include "mikaSipCaller.h"

mikaSipCaller *caller = NULL;

int main( int argc, char *argv[] )
{
  if (argc!=2)
  {
    printf("mikaSipCallerApp ?number\n");
    return 1;
  }
  else
  {
    caller = new mikaSipCaller((unsigned char*)argv[1]);
    caller->call();
  }
  return 0;
}
