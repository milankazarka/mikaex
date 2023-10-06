#include "mikaNetCurl.h"

void initCurl( )
{
  static int run = 0;
  if (run>0)
    return;
  curl_global_init(CURL_GLOBAL_DEFAULT);
  run++;
}

void deinitCurl( )
{
  static int run = 0;
  if (run>0)
    return;
  curl_global_cleanup();
  run++;
}
