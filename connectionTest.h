#ifndef _CONNECTION_TEST_H_
#define _CONNECTION_TEST_H_

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

typedef struct connectionTest
{
  unsigned char server[128];
  CURL *curl;
  CURLcode res;
  struct FtpFile ftpFile;
} connectionTest;

struct connectionTest *connectionTestInit( struct connectionTest *test );

#endif
