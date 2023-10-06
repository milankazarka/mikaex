/** copyright Mika Ltd.
 *  created by Milan Kazarka
 *  M.R.Stefanika 44/29, Ziar nad Hronom 965 01, Slovakia
 *
 *
 *  This is a simulation of the Agnis POS server's communication
 *  with our systems.
 *
 */
#include "mikaCommAgnisServer.h"
#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"
#define _STATIC

mikaCommAgnisServer *agnisServer = NULL;

int mikaCommAgnisServerGlobalInit( )
{
    if (agnisServer)
        return 1;
    agnisServer = new mikaCommAgnisServer();
    return 0;
}

void *onRequest( void *connectionObject )
{
  #ifdef _DEBUG
    printf("onRequest\n");
  #endif
  if (!connectionObject)
    return NULL;
  struct mikaNetConnection *connection = (mikaNetConnection*)connectionObject;
  
  if (agnisServer)
    agnisServer->processRequest(connection);
}

mikaCommAgnisServer::mikaCommAgnisServer( )
{
    netServer = new mikaNetServer(_MIKA_COMM_AGNIS_SERVER_PORT);
    if (!netServer)
    {
        #ifdef _DEBUG
            printf("mikaCommAgnisServer::mikaCommAgnisServer error couldn't create server\n");
        #endif
    }
    else
    {
        connectionDelegate.onRequest = onRequest;
        netServer->customConnectionDelegate = &connectionDelegate;
    }
}

mikaCommAgnisServer::~mikaCommAgnisServer( )
{
    delete netServer;
}

int mikaCommAgnisServer::start( )
{
    netServer->start();
    return 0;
}

int mikaCommAgnisServer::processRequest( mikaNetConnection *connection )
{
    #ifdef _DEBUG
        printf("mikaCommAgnisServer::processRequest\n");
    #endif
    if (!connection)
    {
        #ifdef _DEBUG
            printf("mikaCommAgnisServer::processRequest error invalid argument\n");
        #endif
        return 1;
    }
    
    if ( connection->returnInData( (unsigned char*)buffer ) == 0 )
    {
        TiXmlDocument doc;
        doc.Parse((const char*)buffer,0,TIXML_ENCODING_UTF8);
    
        #ifdef _DEBUG
            printf("mikaCommAgnisServer::processRequest data(%s)\n",(char*)buffer);
        #endif
        
        TiXmlElement *request = doc.FirstChildElement("REQUEST");
        if (request)
        {
            char *type = (char*)request->Attribute("TYPE");
            int intType = atoi(type);
            
            switch(intType)
            {
              // client requested a list of the items in the POS
              case 5:
                strcpy((char*)connection->fastBuffer,
                  "<RESPONSE>\n\
                    <HTLUSEK ID=\"0\">\n\
                    </HTLUSEK>\n\
                    <POLOZKA CISLOPOLOZKY=\"101\"\n\
                        NAZEVPOLOZKY=\"Lasagne Bolognese\"\n\
                        ZKRATKA=\"lasBolognese\"\n\
                        JEDNCENA=\"120\"\n\
                        PROCDPH=\"20\"\n\
                        MAXPOVOLSLEVA=\"23\"\n\
                        TYPPOLOZKY=\"2\"\n\
                        AKTIVNI=\"1\">\n\
                    </POLOZKA>\n\
                  </RESPONSE>\n"
                  );
                  break;
              // client made an order
              case 4:
                strcpy((char*)connection->fastBuffer,
                  "<RESPONSE>\n\
                  </RESPONSE>\n"
                  );
                  break;
            }
        }
    }
    
    return 0;
}

#ifdef _STATIC
  int main()
  {
    mikaCommAgnisServerGlobalInit();
    agnisServer->start();
    return 0;
  }
#endif
