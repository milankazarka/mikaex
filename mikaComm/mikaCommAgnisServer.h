/** copyright Mika Ltd.
 *  created by Milan Kazarka
 *  M.R.Stefanika 44/29, Ziar nad Hronom 965 01, Slovakia
 */
#ifndef _MIKA_COMM_AGNIS_SERVER_H_
#define _MIKA_COMM_AGNIS_SERVER_H_

#include "common.h"
#include "../mikaNetRequest.h"
#include "../mikaNetServer.h"

#define _MIKA_COMM_AGNIS_SERVER_PORT 50000

class mikaCommAgnisServer {
    public:
        mikaNetServer *netServer;
        mikaNetConnectionDelegate connectionDelegate;
        unsigned char buffer[1024*1024*4];
        
        mikaCommAgnisServer( );
        ~mikaCommAgnisServer( );
        
        int start( );
        int processRequest( mikaNetConnection *connection );
};

extern mikaCommAgnisServer *agnisServer;

int mikaCommAgnisServerGlobalInit( );

#endif
