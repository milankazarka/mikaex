#include "system.h"

int main()
{
  trop_Init();
  trop_IPC *client = trop_IPCInit(NULL);
  trop_IPCUdsHandshake(client,"./wm2.sock","<from value=\"localhost\">\n");
  return 0;
}
