/** copyright Mika Ltd.
 *  created by Milan Kazarka
 *  M.R.Stefanika 44/29, Ziar nad Hronom 965 01, Slovakia
 */
#include "mikaCommHttp.h"
#include "mikaCurl.h"
#include "common.h"

/** get the first data buffer of the buffers list
 */
struct mikaCommBuffer *mikaCommBufferGetFirst( struct mikaCommBuffer *buffers )
{
  #ifdef _DEBUG
    printf("mikaCommBufferGetFirst\n");
  #endif
  struct mikaCommBuffer *first = NULL;
  if (!buffers)
    return NULL;
  struct mikaCommBuffer *current = buffers;
  first = current;
  while(current)
  {
    #ifdef _DEBUG
      printf("mikaCommBufferGetFirst iterate\n");
    #endif
    if (!current->next)
      return current;
    current = current->next;
  }
  return first;
}

/** append the response of a http request to string
 */
size_t write_to_string(void *ptr, size_t size, size_t count, void *stream) 
{
  #ifdef _DEBUG
    printf("write_to_string size(%d)\n",size*count);
  #endif
  
  mikaCommHttp *http = (mikaCommHttp*)stream;
  struct mikaCommBuffer *buffer = (struct mikaCommBuffer*)malloc(sizeof(struct mikaCommBuffer));
  
  if (http->responseBuffers)
    http->responseBuffers->last = buffer;
  buffer->next = http->responseBuffers;
  buffer->last = NULL;
  http->responseBuffers = buffer;
  
  buffer->data = (unsigned char*)malloc(size*count);
  buffer->size = size*count;
  memcpy((char*)buffer->data,ptr,size*count);
  
  return size*count;
}

mikaCommHttp::mikaCommHttp( struct mikaHost *host )
{
  if (!host)
  {
    #ifdef _DEBUG
      printf("mikaCommHttp::mikaCommHttp error host not defined\n");
    #endif
  }
  else
    myHost = *host;
  responseBuffers = NULL;
  mikaCurlInit();  
}

mikaCommHttp::~mikaCommHttp( )
{
  releaseResponse();
  mikaCurlDeinit();
}

/** release the last response buffers
 */
void mikaCommHttp::releaseResponse( )
{
  #ifdef _DEBUG
    printf("mikaCommHttp::releaseResponse\n");
  #endif
  struct mikaCommBuffer *current = responseBuffers;
  struct mikaCommBuffer *next = NULL;
  while(current)
  {
    next = current->next;
    free(current->data);
    free(current);
    current = next;
  }
  responseBuffers = NULL;
}

/** debug the respose
 *
 */
void mikaCommHttp::printResponse( )
{
  #ifdef _DEBUG
    printf("mikaCommHttp::printResponse\n");
  #endif
  struct mikaCommBuffer *first = mikaCommBufferGetFirst(responseBuffers);
  if (!first)
  {
    #ifdef _DEBUG
      printf("mikaCommHttp::printResponse error first buffer not found\n");
    #endif
  }
  unsigned char *tmp = NULL;
  while(first)
  {
    tmp = (unsigned char*)malloc(first->size+1);
    memcpy((char*)tmp,(char*)first->data,first->size);
    tmp[first->size] = 0x00;
    printf("%s",(char*)tmp);
    first = first->last;
  }
}

/** save the data contained in the response buffers to a file
 */
int mikaCommHttp::saveResponse( unsigned char *filename )
{
  if (!filename)
    return 1;
  
  FILE *fd = fopen((char*)filename,"w");
  if (fd)
  {
    struct mikaCommBuffer *first = mikaCommBufferGetFirst(responseBuffers);
    if (!first)
    {
      #ifdef _DEBUG
        printf("mikaCommHttp::printResponse error first buffer not found\n");
      #endif
    }
    else
    {
      unsigned char *tmp = NULL;
      while(first)
      {
        tmp = (unsigned char*)malloc(first->size);
        memcpy((char*)tmp,(char*)first->data,first->size);
        fwrite((void*)tmp,first->size,1,fd);
        first = first->last;
      }
    }
    fclose(fd);
  }
  
  return 0;
}

/** performs a HTTP handshake
 */
int mikaCommHttp::sendRequest( struct postAttribute *attributes )
{
  if (!attributes)
  {
    #ifdef _DEBUG
      printf("mikaCommHttp::sendRequest warning no attributes defined\n");
    #endif
  }
  
  releaseResponse();
  
  int urllen = strlen((char*)myHost.name);
  struct postAttribute *current = attributes;
  while(current)
  {
    urllen += strlen((char*)current->key);
    urllen += strlen((char*)current->value);
    urllen += 4;
    current = current->next;
  }
  urllen++;
  unsigned char *url = (unsigned char*)malloc(
    urllen
    );
  // construct the URL
  strcpy((char*)url,(char*)myHost.name);
  current = attributes;
  while(current)
  {
    if (current==attributes)
      strcat((char*)url,"?");
    else
      strcat((char*)url,"&");
    
    strcat((char*)url,(char*)current->key);
    strcat((char*)url,"=");
    strcat((char*)url,(char*)current->value);
    
    current = current->next;
  }
  
  #ifdef _DEBUG
    printf("mikaCommHttp::sendRequest request(%s)\n",
      (char*)url);
  #endif
    
  curl = curl_easy_init();
  
  curl_easy_setopt(curl, CURLOPT_PORT, myHost.port);
  curl_easy_setopt(curl, CURLOPT_URL, (char*)url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 4);
               
  int res = curl_easy_perform(curl);
  if (CURLE_OK != res)
  {
    #ifdef _DEBUG
      printf("mikaCommHttp::sendRequest error performing request\n");
    #endif
  }
  
  curl_easy_cleanup(curl);
  // display the response from the server
  //printResponse();
  
  return 0;
}
