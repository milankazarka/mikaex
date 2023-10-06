/** this should manage the initialisation of Curl Global
 */
#include "mikaCurl.h"
#include "common.h"

mikaCurl *mikaCurlInstance = NULL;

void initCurl( )
{
  curl_global_init(CURL_GLOBAL_DEFAULT);
}

void deinitCurl( )
{
  curl_global_cleanup();
}

mikaCurl::mikaCurl( )
{
  #ifdef _DEBUG
    printf("mikaCurl::mikaCurl\n");
  #endif
  if (!mikaCurlInstance)
  {
    mikaCurlInstance = this;
    initCurl();
  }
  else
  {
    #ifdef _DEBUG
      printf("mikaCurl::mikaCurl warning CURL already initialized\n");
    #endif
  }
}

mikaCurl::~mikaCurl( )
{
  if (this==mikaCurlInstance)
  {
    deinitCurl();
    mikaCurlInstance = NULL;
  }
}

void mikaCurlInit( )
{
  if (!mikaCurlInstance)
    new mikaCurl();
}

void mikaCurlDeinit( )
{
  if (mikaCurlInstance)
    delete mikaCurlInstance;
}
