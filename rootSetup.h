#ifndef _ROOT_SETUP_H_
#define _ROOT_SETUP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

class rootSetup {
  public:
    unsigned char *myFilename;
    void *doc;
  
    rootSetup( unsigned char *filename );
    ~rootSetup( );
    
    int getAttribute( unsigned char *name, unsigned char *value );
};

extern rootSetup *rootSetupGlobal;
rootSetup *getRootSetup();

#endif
