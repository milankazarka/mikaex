#ifndef _MIKA_MESSAGE_H_
#define _MIKA_MESSAGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tinyxml.h"
#include "tinystr.h"

#define _MIKA_MESSAGE_VALUELEN 4096
#define _MIKA_MESSAGE_BUFFER 4096*1000

class mikaMessage {
  public:
    unsigned char messageBuffer[_MIKA_MESSAGE_BUFFER];
    TiXmlDocument doc;
  
    mikaMessage( );
    ~mikaMessage( );
    
    int loadFileToBuffer( unsigned char *filename );
    /** generate the TiXmlDocument from the strMessage
     */
    int loadString( unsigned char *strMessage );
    /** generate a string message from 'doc'
     */
    int generateString( unsigned char *strMessage );
};

#endif
