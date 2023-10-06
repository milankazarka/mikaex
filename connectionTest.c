/** this application tries to connect to a speciffic host and download
 *  the global configuration file. It runs the tries until it's successfull.
 */
#include "connectionTest.h"

#ifndef _DEBUG
  #define _DEBUG
#endif

#define _SERVER	"ftp://127.0.0.1"
#define _TESTFILE "global.xml"

static size_t my_fwrite( void *buffer, size_t size, size_t nmemb, void *stream )
{
  struct FtpFile *out=(struct FtpFile *)stream;
  if (out && !out->stream)
  {
    out->stream=fopen(out->filename,"wb");
    if (!out->stream)
      return -1;
  }
  return fwrite(buffer, size, nmemb, out->stream);
}

/** initialize the connectionTest
 */
struct connectionTest *connectionTestInit( struct connectionTest *test )
{
  if (!test)
  {
    test = (struct connectionTest*)malloc(sizeof(struct connectionTest));
  }
  test->server[0] = 0x00;
  test->ftpFile.filename = (const char*)malloc(256);
  test->ftpFile.stream = NULL;
  return test;
}

int connectionTestRun( struct connectionTest *test )
{
  #ifdef _DEBUG
    printf("connectionTestRun\n");
  #endif
  
  if (test->server[0]==0x00)
  {
    #ifdef _DEBUG
      printf("connectionTestRun ERROR server not set\n");
    #endif
    return 1;
  }
  
  unsigned char url[512];
  
  curl_global_init(CURL_GLOBAL_DEFAULT);
  test->curl = curl_easy_init();
  
  strcpy((char*)test->ftpFile.filename,_TESTFILE);
  sprintf((char*)url,"%s/%s",(char*)test->server,_TESTFILE);
  
  curl_easy_setopt(test->curl, CURLOPT_URL, (char*)url);
  curl_easy_setopt(test->curl, CURLOPT_WRITEFUNCTION, my_fwrite);
  curl_easy_setopt(test->curl, CURLOPT_WRITEDATA, &test->ftpFile);
  curl_easy_setopt(test->curl, CURLOPT_VERBOSE, 1);
  curl_easy_setopt(test->curl, CURLOPT_TRANSFERTEXT, 0);
  
  while(1)
  {
    int res = curl_easy_perform(test->curl);
  
    if (CURLE_OK != res)
    {
      #ifdef _DEBUG
        printf("connectionTestRun fail, continue\n");
      #endif
    }
    else
    {
      #ifdef _DEBUG
        printf("connectionTestRun succeed\n");
      #endif
      break;
    }
    sleep(1);
  }
  curl_easy_cleanup(test->curl);
  curl_global_cleanup();
  
  return 0;
}

int main( )
{
  struct connectionTest *test = NULL;
  test = connectionTestInit(NULL);
  if (!test)
  {
    #ifdef _DEBUG
      printf("connectionTest ERROR coundn't initialize\n");
    #endif
    return 1;
  }
  strcpy((char*)test->server,_SERVER);
  connectionTestRun(test);
  
  return 0;
}
