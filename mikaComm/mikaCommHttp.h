/** copyright Mika Ltd.
 *  created by Milan Kazarka
 *  M.R.Stefanika 44/29, Ziar nad Hronom 965 01, Slovakia
 *  
 *  this class manages requests and responses to
 *  HTTP servers
 */
#ifndef _MIKA_COMM_HTTP_H_
#define _MIKA_COMM_HTTP_H_

#include "mikaNetRequest.h"
#include "mikaHost.h"

#define _ATTRIBUTE_LENGTH 512

typedef struct mikaCommBuffer {
  unsigned char *data;
  int size;
  struct mikaCommBuffer *next;
  struct mikaCommBuffer *last;
} mikaCommBuffer;

struct mikaCommBuffer *mikaCommBufferGetFirst( struct mikaCommBuffer *buffers );

typedef struct postAttribute {
  unsigned char key[_ATTRIBUTE_LENGTH];
  unsigned char value[_ATTRIBUTE_LENGTH];
  struct postAttribute *next;
} postAttribute;

class mikaCommHttp {
  public:
    struct mikaHost myHost;
    CURL *curl;
    struct mikaCommBuffer *responseBuffers;
  
    mikaCommHttp( struct mikaHost *host );
    ~mikaCommHttp( );
    
    void releaseResponse( );
    void printResponse( );
    int sendRequest( struct postAttribute *attributes );
    int saveResponse( unsigned char *filename );
};

#endif
