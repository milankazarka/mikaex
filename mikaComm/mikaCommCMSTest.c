#include "mikaCommCMSTest.h"
#include "mikaCommCMS.h"
#include "common.h"

mikaCommCMSTest::mikaCommCMSTest( mikaCommCMS *_cms )
{
  cms = _cms;
}

mikaCommCMSTest::~mikaCommCMSTest( )
{
}

void mikaCommCMSTest::start( )
{
  #ifdef _DEBUG
    printf("mikaCommCMSTest::start\n");
  #endif
  cms->getConfig();
}

int main( )
{
  struct mikaHost host;
  strcpy((char*)host.name,"localhost");
  host.port = 46001;
  mikaCommCMS *commCMS = new mikaCommCMS(&host);
  mikaCommCMSTest *commCMSTest = new mikaCommCMSTest(commCMS);
  commCMSTest->start();
  return 0;
}
