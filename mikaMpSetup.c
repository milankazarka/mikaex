#include "mikaMpSetup.h"
#include "config.h"

mikaMpSetup *mpSetupGlobal = NULL;

mikaMpSetup::mikaMpSetup( unsigned char *path )
{
  if (!path)
  {
    #ifdef _DEBUG
      printf("mikaMpSetup::mikaMpSetup error invalid argument\n");
    #endif
  }
  doc.LoadFile((const char*)path);
  TiXmlElement *root = doc.RootElement();
  if (!root)
  {
    #ifdef _DEBUG
      printf("mikaMpSetup::mikaMpSetup error no root object\n");
    #endif
  }
}

mikaMpSetup::~mikaMpSetup( )
{
}

/** initialize the global setup
 */
int mikaMpSetupGlobalInit( )
{
  if (mpSetupGlobal)
  {
    #ifdef _DEBUG
      printf("mikaMpSetupGlobalInit warning setup already initialized\n");
    #endif
    return 1;
  }
  
  mpSetupGlobal = new mikaMpSetup((unsigned char*)_MIKA_MP_SETUP);
  
  return 0;
}

int mikaMpSetupGlobalDeinit( )
{
  if (mpSetupGlobal)
  {
    delete mpSetupGlobal;
    mpSetupGlobal = NULL;
  }
  else
  {
    #ifdef _DEBUG
      printf("mikaMpSetupGlobalDeinit warning setup already deinitialized\n");
    #endif
    return 1;
  }
  return 0;
}
