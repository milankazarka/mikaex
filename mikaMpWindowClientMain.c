#include "mikaMpWindowClient.h"
#include "mikaMpDesktopServer.h"
#include "mikaHost.h"

int main( )
{
  struct mikaHost host;
  strcpy((char*)host.name,"localhost");
  host.port = _MIKA_MP_DESKTOP_SERVER_PORT;
  mikaApp *app = new mikaApp((unsigned char*)"./");
  mikaMpWindowClient *client = new mikaMpWindowClient(app,&host);
  
  unsigned char message[4096];
  sprintf((char*)message,"<mikaMpWindowClient uuid=\"%s\">\n</mikaMpWindowClient>\n",
    (char*)app->uid->uuid);
  
  client->sendMessageData((unsigned char*)message);
  return 0;
}
