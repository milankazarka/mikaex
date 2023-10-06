#ifndef _PACKAGE_MANAGER_H_
#define _PACKAGE_MANAGER_H_

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

typedef struct packageManager
{
  unsigned char server[128];
  CURL *curl;
  CURLcode res;
  struct FtpFile ftpFile;
} packageManager;

struct packageManager *packageManagerInit( struct packageManager *manager );
int packageManagerUpdateSystem( struct packageManager *manager );

#endif
