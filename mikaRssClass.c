#include "mikaRssClass.h"
#include "config.h"

mikaRssClass::mikaRssClass( unsigned char *filename )
{
  if (!filename)
  {
    #ifdef _DEBUG
      printf("mikaRssClass::mikaRssClass\n");
    #endif
  }
  else
  {
    int ret = mrss_parse_file((char*)filename,&mrss);
  }
}

mikaRssClass::~mikaRssClass( )
{
}
