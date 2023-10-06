#ifndef _MIKA_NET_SERVER_H_
#define _MIKA_NET_SERVER_H_

/** this is a simplified connection handling class
 *  which can be used to construct socket servers
 *  quicly
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include "mikaBuffer.h"
#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"

typedef struct mikaNetConnectionDelegate
{
  void		*(*onRequest)(void *connectionObject);
} mikaNetConnectionDelegate;

/** connection & buffer handling class
 *
 */
class mikaNetConnection {
  public:
    int sysConnection;
    struct mikaBuffer *inBuffers;
    struct mikaBuffer *outBuffers;
    struct mikaBuffer *firstInBuffer;
    struct mikaBuffer *firstOutBuffer;
    struct mikaNetConnectionDelegate *delegate; 
  
    unsigned char fastBuffer[4096*8];
  
    mikaNetConnection( int connection );
    ~mikaNetConnection( );
    /** standard handshake
     */
    int handshake( );
    int myRecv( );
    int reply( );
    /** return the complete size of the data in the buffers
     */
    int inBuffersSize( );
    int outBuffersSize( );
    int outBuffersAddData( unsigned char *data );
    /** fill the 'data' with the merged content
     *  of all buffers 'in/out'
     */
    int returnInData( unsigned char *data );
    int returnOutData( unsigned char *data );
};

/** display the contents of all the buffers
 */
int debugAllBuffers( struct mikaBuffer *firstBuffer );

class mikaNetServer {
  public:
    int myPort;
    int sysSocket;
    /** set this to process the request and generate a reply
     */
    struct mikaNetConnectionDelegate *customConnectionDelegate;
    
    mikaNetServer( int port );
    ~mikaNetServer( );
    int start( );
};

#endif
