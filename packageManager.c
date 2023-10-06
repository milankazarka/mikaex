/** this application simply connects to an FTP server, downloads a
 *  package with the system files and extracts them - this is a very
 *  simple tool to update a system
 */
#include "packageManager.h"

#ifndef _DEBUG
  #define _DEBUG
#endif

#define _SERVER	"ftp://mika.com"
#define _PACKAGE "system.zip"
#define _SOURCE

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

/** initialize the package manager
 */
struct packageManager *packageManagerInit( struct packageManager *manager )
{
  if (!manager)
  {
    manager = (struct packageManager*)malloc(sizeof(struct packageManager));
  }
  manager->server[0] = 0x00;
  manager->ftpFile.filename = (const char*)malloc(256);
  manager->ftpFile.stream = NULL;
  return manager;
}

/** download the main system package and extract it in the current directory
 */
int packageManagerUpdateSystem( struct packageManager *manager )
{
  #ifdef _DEBUG
    printf("packageManagerUpdateSystem\n");
  #endif
  
  if (!manager)
  {
    #ifdef _DEBUG
      printf("packageManagerUpdateSystem ERROR invalid arguments\n");
    #endif
    return 1;
  }
  
  if (manager->server[0]==0x00)
  {
    #ifdef _DEBUG
      printf("packageManagerUpdateSystem ERROR server not set\n");
    #endif
    return 1;
  }
  
  unsigned char url[512];
  
  curl_global_init(CURL_GLOBAL_DEFAULT);
  manager->curl = curl_easy_init();
  
  strcpy((char*)manager->ftpFile.filename,_PACKAGE);
  sprintf((char*)url,"%s/%s",(char*)manager->server,_PACKAGE);
  
  curl_easy_setopt(manager->curl, CURLOPT_URL, (char*)url);
  curl_easy_setopt(manager->curl, CURLOPT_WRITEFUNCTION, my_fwrite);
  curl_easy_setopt(manager->curl, CURLOPT_WRITEDATA, &manager->ftpFile);
  curl_easy_setopt(manager->curl, CURLOPT_VERBOSE, 1);
  curl_easy_setopt(manager->curl, CURLOPT_TRANSFERTEXT, 0);
  
  int res = curl_easy_perform(manager->curl);
  
  if (CURLE_OK != res)
  {
    #ifdef _DEBUG
      printf("packageManagerUpdateSystem ERROR couldn't download package\n");
    #endif
    return 1;
  }
  else
  {
    #ifdef _DEBUG
      printf("packageManagerUpdateSystem system package download successful\n");
    #endif
    sync();
    fclose(manager->ftpFile.stream);
    manager->ftpFile.stream = NULL;
    unsigned char command[256];
    sprintf((char*)command,"unzip -o %s",_PACKAGE);
    system((char*)command);
    #ifdef _SOURCE
      sprintf((char*)command,"./build.sh");
      system((char*)command);
    #endif
  }
  curl_easy_cleanup(manager->curl);
  curl_global_cleanup();
  
  return 0;
}

int main( )
{
  struct packageManager *manager = NULL;
  manager = packageManagerInit(manager);
  if (!manager)
  {
    #ifdef _DEBUG
      printf("packageManager ERROR coundn't initialize\n");
    #endif
    return 1;
  }
  strcpy((char*)manager->server,_SERVER);
  packageManagerUpdateSystem(manager);
  
  return 0;
}
