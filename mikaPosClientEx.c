#include "mikaPosClientEx.h"

#ifndef _STATIC
  #define _STATIC
#endif

mikaPosClientEx::mikaPosClientEx( struct mikaHost *_host )
{
  requestHandler = new mikaNetRequest();
  host = _host;
}

mikaPosClientEx::~mikaPosClientEx( )
{
}

int mikaPosClientEx::testHandshake( )
{
  static int appliance_id = 0;
  
  unsigned char message[1024];
  unsigned char response[4096*10];
  sprintf((char*)message,"<msg><mod name=\"mikaPosEx\"><command type=\"NewOrder\"><appliance id=\"%d\"><orders><order><items><item detail=\"Coca Cola\"></item></items></order></orders></appliance></command></mod></msg>",
    appliance_id);
  //requestHandler->sendRequest(host,(unsigned char*)message);
  requestHandler->handshakeRequest(host,(unsigned char*)message,(unsigned char*)response,4096*10);
  appliance_id++;
  return 0;
}

#ifdef _STATIC
  int main( )
  {
    struct mikaHost host;
    host.port = 46000;
    strcpy((char*)host.name,"localhost");
    
    mikaPosClientEx *client = new mikaPosClientEx(&host);
    if (client)
    {
      int n;
      for(n = 0; n < 100000; n++)
      {
        client->testHandshake();
      }
    }
    
    return 0;
  }
#endif
