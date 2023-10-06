#include "wmanager.h"

#define _STATIC

CWManager::CWManager( )
{
}

CWManager::~CWManager( )
{
}

#ifdef _STATIC
  int main( )
  {
    new CWManager();
    return 0;
  }
#endif
