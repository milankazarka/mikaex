#ifndef _NETWORK_SETUP_H_
#define _NETWORK_SETUP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

struct FtpFile
{
  const char *filename;
  FILE *stream;
};

typedef struct networkSetup
{
  int port;
  unsigned char server[256];
  unsigned char path[256];
  unsigned char filename[256]; // the main setup filename
  unsigned char username[32];
  unsigned char password[32];
  CURL *curl;
  CURLcode res;
  struct FtpFile ftpFile;
} networkSetup;

struct networkSetup *networkSetupInit( struct networkSetup *setup );
int networkSetupDownloadSetup( struct networkSetup *setup );
int networkSetupDownloadFile( struct networkSetup *setup, unsigned char *filename );

#endif
