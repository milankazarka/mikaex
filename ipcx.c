#include "system.h"
#include "gui.h"
#include "global.h"

int ipc_WinResetAll( ) // needs sync
{
  int i;
  struct ipc_Win *ptr = (void*)glob_data;
  for (i = 0; i < TROP_WINCNT; i++)
  {
    ptr->state = 0;
    ptr += TROP_WINDOW;
  }
}

int ipc_WinGetState( int pos ) // needs sync
{
  struct ipc_Win *ptr = (void*)glob_data;
  ptr += (TROP_WINDOW*pos);
  return ptr->state;
}

int ipc_WinGetID( int pos ) // needs sync
{
  struct ipc_Win *ptr = (void*)glob_data;
  ptr += (TROP_WINDOW*pos);
  return ptr->id;
}

void *ipc_WinGetAtID( int id ) // needs sync
{
  int i;
  struct ipc_Win *ptr = (void*)glob_data;
  for (i = 0; i < TROP_WINCNT; i++)
  {
    if (ptr->id == id)
    {
      return (void*)ptr;
    }
    ptr += TROP_WINDOW;
  }
  return NULL;
}
