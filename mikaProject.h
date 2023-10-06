#ifndef _MIKA_PROJECT_H_
#define _MIKA_PEOJECT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"

#define _LENGTH_NAME 512

class mikaProjectAppObject {
  public:
    unsigned char filename[_LENGTH_NAME];
    
    mikaProjectAppName( );
    ~mikaProjectAppName( );
};

class mikaProjectApp {
  public:
    unsigned char name[_LENGTH_NAME];
    
    mikaProjectApp( TiXmlElement *root, TiXmlElement *appRoot );
    ~mikaProjectApp( );
};

class mikaProject {
  public:
    unsigned char name[_LENGTH_NAME];
  
    mikaProject( unsigned char *filename );
    ~mikaProject( );
};

#endif
