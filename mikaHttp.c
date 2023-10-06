#include "mikaHttp.h"

#define _MIKA_HTTP_STATIC_NO
#define _TMP_FILENAME "page.html"
#define _LINE_SIZE 4096
#ifndef _DEBUG
  #define _DEBUG
#endif

struct FtpFile
{
  const char *filename;
  FILE *stream;
};

static size_t my_fwrite( void *buffer, size_t size, size_t nmemb, void *stream )
{
  struct FtpFile *out=(struct FtpFile *)stream;
  if(out && !out->stream) 
  {
    out->stream=fopen(out->filename, "wb");
    if(!out->stream)
      return -1;
  }
  size_t ret = fwrite(buffer, size, nmemb, out->stream);
  fflush(out->stream);
  return ret;
}

struct mikaHttp *mikaHttpInit( struct mikaHttp *myHttp )
{
  if (!myHttp)
    myHttp = (struct mikaHttp*)malloc(sizeof(struct mikaHttp));
  
  curl_global_init(CURL_GLOBAL_DEFAULT);
  
  return myHttp;
}

int mikaHttpGet( struct mikaHttp *myHttp, unsigned char *address )
{
  if (!myHttp)
    return 1;
  
  myHttp->curl = curl_easy_init();
  if (myHttp->curl)
  {
    curl_easy_setopt(myHttp->curl,CURLOPT_URL,(char*)address);
    
    struct FtpFile file;
    file.filename = _TMP_FILENAME;
    file.stream = NULL;
    
    curl_easy_setopt(myHttp->curl, CURLOPT_WRITEFUNCTION, my_fwrite); 
    curl_easy_setopt(myHttp->curl, CURLOPT_WRITEDATA, &file);  
    
      
    myHttp->res = curl_easy_perform(myHttp->curl);
    long ptr;
    curl_easy_getinfo(myHttp->curl,CURLINFO_HTTP_CODE,&ptr);
    curl_easy_cleanup(myHttp->curl);
  }
    
  return 0;
}

/** if data == NULL, then we increase data's size dynamically
 */
unsigned char *mikaHttpData( struct mikaHttp *myHttp, unsigned char *data )
{
  if (!myHttp)
    return NULL;
  
  FILE *fd;
  unsigned char line[_LINE_SIZE];
  unsigned char *tmp = NULL;
  if ((fd = fopen(_TMP_FILENAME,"r")))
  {
    while(feof(fd)==0)
    {
      fgets((char*)line,_LINE_SIZE-1,fd);
    
      if (data==NULL)
      {
        data = (unsigned char*)malloc(strlen((char*)line)+1);
        memcpy((void*)data,(void*)line,strlen((char*)line));
        data[strlen((char*)line)] = 0x00;
      }
      else
      {
        tmp = (unsigned char*)malloc(strlen((char*)line)+1+strlen((char*)data));
        tmp[0] = 0x00;
        strcpy((char*)tmp,(char*)data);
        strcat((char*)tmp,(char*)line);
        free(data);
        data = tmp;
      }
    }
    fclose(fd);
  }
  
  #ifdef _DEBUG
    if (data)
    {
      printf("mikaHttpData data(%s)\n",(char*)data);
    }
  #endif
  
  return data;
}

#ifdef _MIKA_HTTP_STATIC
  int main( )
  {
    struct mikaHttp http;
    mikaHttpGet(&http,(unsigned char*)"192.168.0.1");
    mikaHttpData(&http,NULL);
    return 0;
  }
#endif
