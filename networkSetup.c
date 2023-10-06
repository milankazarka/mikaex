#include "networkSetup.h"
#include "config.h"

#define _SETUP_DEFAULT_FILENAME "setup.xml"

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

/** initialize the networkSetup object
 */
struct networkSetup *networkSetupInit( struct networkSetup *setup )
{
  #ifdef _DEBUG
    printf("networkSetupInit\n");
  #endif
  
  if (!setup)
    setup = (struct networkSetup*)malloc(sizeof(struct networkSetup));
  
  setup->port = 21;
  setup->server[0] = 0x00;
  setup->path[0] = 0x00;
  setup->filename[0] = 0x00;
  setup->username[0] = 0x00;
  setup->password[0] = 0x00;
  
  setup->ftpFile.filename = (char*)malloc(256);
  setup->ftpFile.stream = NULL;
  
  return setup;
}

/** download the main configuration file from the server
 */
int networkSetupDownloadSetup( struct networkSetup *setup )
{
  #ifdef _DEBUG
    printf("networkSetupDownloadSetup\n");
  #endif
  if (!setup)
  {
    #ifdef _DEBUG
      printf("networkSetupDownloadSetup ERROR invalid arguments\n");
    #endif
    return 1;
  }
  
  if (
    setup->port < 1 ||
    setup->server[0] == 0x00 ||
    setup->username[0] == 0x00 ||
    setup->password[0] == 0x00
  )
  {
    #ifdef _DEBUG
      printf("networkSetupDownloadSetup ERROR missing settings\n");
    #endif
    return 1;
  }
  
  unsigned char url[1024];
  unsigned char *filename = NULL;
  
  if (setup->filename[0]!=0x00) // use default setup file, if not defined
    filename = (unsigned char*)setup->filename;
  else
    filename = _SETUP_DEFAULT_FILENAME;
  
  if (setup->path[0]==0x00)
    sprintf((char*)url,"%s/%s",(char*)setup->server,
                                filename
                                );
  else
    sprintf((char*)url,"%s/%s/%s",(char*)setup->server,
                                (char*)setup->path,
                                filename
                                );
  
  sprintf((char*)setup->ftpFile.filename,filename);
  
  #ifdef _DEBUG
    printf("networkSetupDownloadSetup download(%s)\n",(char*)url);
  #endif
  
  curl_global_init(CURL_GLOBAL_DEFAULT);
  setup->curl = curl_easy_init();
  
  curl_easy_setopt(setup->curl, CURLOPT_URL,(char*)url);
  curl_easy_setopt(setup->curl, CURLOPT_WRITEFUNCTION,my_fwrite);
  curl_easy_setopt(setup->curl, CURLOPT_WRITEDATA, &setup->ftpFile);
  curl_easy_setopt(setup->curl, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(setup->curl, CURLOPT_TRANSFERTEXT,1);
  
  int res = curl_easy_perform(setup->curl);
  
  if (CURLE_OK != res)
  {
    printf("CURL could not fetch %s code %d\n",(char*)url,res);
    return 1;
  }
  else
  {
    sync();
    fclose(setup->ftpFile.stream);
    setup->ftpFile.stream = NULL;
    unsigned char command[256];
    sprintf((char*)command,"unzip -o %s",filename);
    system((char*)command);
  }
  curl_easy_cleanup(setup->curl);
  curl_global_cleanup();
  
  return 0;
}

/** download a file from our configuration server, use the preset path
 */
int networkSetupDownloadFile( struct networkSetup *setup, unsigned char *filename )
{
  #ifdef _DEBUG
    printf("networkSetupDownloadFile\n");
  #endif
  if (!setup)
  {
    #ifdef _DEBUG
      printf("networkSetupDownloadFile ERROR invalid arguments\n");
    #endif
    return 1;
  }
  
  if (
    setup->port < 1 ||
    setup->server[0] == 0x00 ||
    setup->username[0] == 0x00 ||
    setup->password[0] == 0x00
  )
  {
    #ifdef _DEBUG
      printf("networkSetupDownloadFile ERROR missing settings\n");
    #endif
    return 1;
  }
  
  unsigned char url[1024];
  if (setup->path[0]==0x00)
    sprintf((char*)url,"%s/%s",(char*)setup->server,
                                (char*)filename
                                );
  else
    sprintf((char*)url,"%s/%s/%s",(char*)setup->server,
                                (char*)setup->path,
                                (char*)filename
                                );
  
  sprintf((char*)setup->ftpFile.filename,"setup/%s",(char*)filename);
  
  #ifdef _DEBUG
    printf("networkSetupDownloadFile download(%s) to(%s)\n",
      (char*)url,(char*)setup->ftpFile.filename);
  #endif
  
  curl_global_init(CURL_GLOBAL_DEFAULT);
  setup->curl = curl_easy_init();
  
  curl_easy_setopt(setup->curl, CURLOPT_URL,(char*)url);
  curl_easy_setopt(setup->curl, CURLOPT_WRITEFUNCTION,my_fwrite);
  curl_easy_setopt(setup->curl, CURLOPT_WRITEDATA, &setup->ftpFile);
  curl_easy_setopt(setup->curl, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(setup->curl, CURLOPT_TRANSFERTEXT,0);
  
  int res = curl_easy_perform(setup->curl);
  
  if (CURLE_OK != res)
  {
    printf("CURL could not fetch %s code %d\n",(char*)url,res);
    return 1;
  }
  else
  {
    sync();
    fclose(setup->ftpFile.stream);
    setup->ftpFile.stream = NULL;
  }
  curl_easy_cleanup(setup->curl);
  curl_global_cleanup();
  
  return 0;
}
