/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

/**
 * module processing requests of Draqie POS software
 */
#include "draqiePosMod.h"
#include "mikaServer.h"
#include "mikaServerMod.h"
#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"

/** a prototype of a Server mod which mobile POS devices use to
 *  retrieve data from POS terminals
 */

#define _DEFAULT_CSV "pos/SKOC0000/EUROVEA0"

enum _COMMAND_TYPE
{
  _COMMAND_TYPE_NONE,
  _COMMAND_TYPE_LIST,
  _COMMAND_TYPE_MODIFY,
  _COMMAND_TYPE_ADD,
  _COMMAND_TYPE_REMOVE
};

enum _REQUEST_STATE
{
  _REQUEST_STATE_PENDING,
  _REQUEST_STATE_ACCEPT,
  _REQUEST_STATE_DONE
};

static pthread_mutex_t draqiePosMutex;

int _process_data_draqie_pos_command( TiXmlElement *command, unsigned char *out )
{
  if (!command || !out)
    return _COMMAND_TYPE_NONE;
  
  char *commandValue = (char*)command->GetText();
  if (!commandValue)
    return _COMMAND_TYPE_NONE;
  
  int _command_type = _COMMAND_TYPE_NONE;
  
  #ifdef _DEBUG
    printf("_process_data_draqie_pos_command\n");
  #endif
  if (strcmp(commandValue,"LIST")==0)
  {
    #ifdef _DEBUG
      printf("\tLIST\n");
    #endif
    _command_type = _COMMAND_TYPE_LIST;
  }
  else if (strcmp(commandValue,"MODIFY")==0)
  {
    #ifdef _DEBUG
      printf("\tMODIFY\n");
    #endif
    _command_type = _COMMAND_TYPE_MODIFY;
  }
  else if (strcmp(commandValue,"ADD")==0)
  {
    #ifdef _DEBUG
      printf("\tADD\n");
    #endif
    _command_type = _COMMAND_TYPE_ADD;
  }
  else if (strcmp(commandValue,"REMOVE")==0)
  {
    #ifdef _DEBUG
      printf("\tREMOVE\n");
    #endif
    _command_type = _COMMAND_TYPE_REMOVE;
  }
  
  return _command_type; 
}

/**
 * synchronized processing of data from various Draqie POS devices
 *
 */
void *_process_data_draqie_pos( unsigned char *in, unsigned char *out )
{
  static int run = 0;
  
  if (!in || !out)
    return NULL;
  
  #ifdef _DEBUG
    printf("_process_data_draqie_pos\n");
  #endif
  
  if (run==0)
    pthread_mutex_init(&draqiePosMutex,NULL);
  
  pthread_mutex_lock(&draqiePosMutex);
  int _command_type = _COMMAND_TYPE_NONE;
  unsigned char *csvLocation = NULL;
  struct draqiePosData *data = NULL;
  TiXmlElement *elem;
  TiXmlElement *status;
  TiXmlElement *device;
  TiXmlElement *order;;
  
  TiXmlDocument doc;
  doc.Parse(
    (const char*)in,
    0, TIXML_ENCODING_UTF8
  );
  TiXmlElement *root = doc.FirstChildElement("msg");
  if (root)
  {
    TiXmlElement *id = root->FirstChildElement("command");
    if (id)
    {
      _command_type = _process_data_draqie_pos_command(id,out);
    }
  }
  
  switch(_command_type)
  {
    case _COMMAND_TYPE_LIST:
      if (csvLocation==NULL)
        csvLocation = (unsigned char*)_DEFAULT_CSV;
      data = draqiePosDataInit(); 
      if ( draqiePosDataRead(data,csvLocation) == 0 )
      {
        strcpy((char*)out,"<msg>\n<list>\n");
        struct draqiePosRequest *current = data->requests;
        while(current)
        {
          strcat((char*)out,(char*)current->line);
          strcat((char*)out,"\n");
          current = current->next;
        }
        strcat((char*)out,"</list>\n</msg>\n");
        
        #ifdef _DEBUG
          printf("_process_data_draqie_pos out(%s)\n",
            (char*)out);
        #endif
        
        draqiePosDataWrite(data,csvLocation);
      }
      draqiePosDataRelease(data);
    break;
    case _COMMAND_TYPE_MODIFY:
      elem = root->FirstChildElement("element");
      status = root->FirstChildElement("status");
      if (elem && status)
      {
        if (csvLocation==NULL)
          csvLocation = (unsigned char*)_DEFAULT_CSV;
        data = draqiePosDataInit(); 
        if ( draqiePosDataRead(data,csvLocation) == 0 )
        {
          struct draqiePosRequest *current = data->requests;
          while(current)
          {
            if (current->requestid==atoi(elem->GetText()))
            {
              if (strcmp(status->GetText(),"ACCEPT")==0)
                current->state = _REQUEST_STATE_ACCEPT;
              else if (strcmp(status->GetText(),"PENDING")==0)
                current->state = _REQUEST_STATE_PENDING;
              else if (strcmp(status->GetText(),"DONE")==0)
                current->state = _REQUEST_STATE_DONE;
            }
            current = current->next;
          }
          
          draqiePosDataWrite(data,csvLocation);
        }
        draqiePosDataRelease(data);
      }
    break;
    case _COMMAND_TYPE_ADD:
      device = root->FirstChildElement("device");
      order = root->FirstChildElement("order");
      if (elem && order)
      {
        if (csvLocation==NULL)
          csvLocation = (unsigned char*)_DEFAULT_CSV;
        data = draqiePosDataInit(); 
        if ( draqiePosDataRead(data,csvLocation) == 0 )
        {
          draqiePosDataAdd(data,atoi(device->GetText()),(unsigned char*)order->GetText());
          draqiePosDataWrite(data,csvLocation);
        }
        draqiePosDataRelease(data);
      }
    break;
    case _COMMAND_TYPE_REMOVE:
      elem = root->FirstChildElement("element");
      if (elem)
      {
        if (csvLocation==NULL)
          csvLocation = (unsigned char*)_DEFAULT_CSV;
        data = draqiePosDataInit(); 
        if ( draqiePosDataRead(data,csvLocation) == 0 )
        {
          draqiePosDataRemove(data,atoi((char*)elem->GetText()));
          draqiePosDataWrite(data,csvLocation);
        }
        draqiePosDataRelease(data);
      }
    break;
  }
  pthread_mutex_unlock(&draqiePosMutex);
    
  return NULL;
}

struct draqiePosData *draqiePosDataInit( )
{
  struct draqiePosData *data = (struct draqiePosData*)malloc(sizeof(struct draqiePosData));
  data->requests = NULL;
  return data;
}

int draqiePosDataRelease( struct draqiePosData *data )
{
  if (!data)
    return 1;
  
  struct draqiePosRequest *current = data->requests;
  struct draqiePosRequest *next = NULL;
  while(current)
  {
    next = current->next;
    free(current);
    current = next;
  }  
    
  free(data);
  return 0;
}

int draqiePosDataParseLine( struct draqiePosData *data, unsigned char *line )
{
  if (!data || !line)
    return 1;
  
  #ifdef _DEBUG
    printf("draqiePosDataParseLine line(%s)\n",line);
  #endif  
  
  struct draqiePosRequest *request = NULL;
  request = (struct draqiePosRequest*)malloc(sizeof(struct draqiePosRequest));
  request->line[0] = 0x00;
  request->order[0] = 0x00;
  request->next = NULL;
  
  strcpy((char*)request->line,(char*)line);
  unsigned char tmp[4096];
  
  int mode = 0;
  int pos = 0;
  for(int n = 0; n < strlen((char*)line); n++)
  {
    if (line[n]==',' || n == strlen((char*)line)-1)
    {
      if (n == strlen((char*)line)-1)
      {
        strncpy((char*)tmp,(char*)line+pos,(n-pos)+1);
        tmp[(n-pos)+1] = 0x00;
      }
      else
      {
        strncpy((char*)tmp,(char*)line+pos,n-pos);
        tmp[(n-pos)] = 0x00;
      }
      
      switch(mode)
      {
        case 0:
          request->deviceid = atoi((char*)tmp);
        break;
        case 1:
          request->requestid = atoi((char*)tmp);
        break;
        case 2:
          if (strcmp((char*)tmp,"PENDING")==0)
            request->state = _REQUEST_STATE_PENDING;
          else if (strcmp((char*)tmp,"ACCEPT")==0)
            request->state = _REQUEST_STATE_ACCEPT;
          else if (strcmp((char*)tmp,"DONE")==0)
            request->state = _REQUEST_STATE_DONE;
        break;
        case 3:
          strcpy((char*)request->order,(char*)tmp);
        break;
      }
      
      pos = n+1;
      mode++;
    }
  }
  #ifdef _DEBUG
    printf("draqiePosDataParseLine deviceid(%d) requestid(%d) state(%d) order(%s)\n",
      request->deviceid,request->requestid,request->state,(char*)request->order);
  #endif
  
  struct draqiePosRequest *current = data->requests;
  if (!current)
  {
    data->requests = request;
  }
  else
  {
    while(current)
    {
      if (!current->next)
      {
        current->next = request;
        break;
      }
      current = current->next;
    }
  }
    
  return 0;
}

int draqiePosDataRead( struct draqiePosData *data, unsigned char *filename )
{
  if (!data || !filename)
    return 1;
  
  FILE *fd;
  unsigned char line[4096];
  if ((fd = fopen((char*)filename,"r")))
  {
    while(1)
    {
      fgets((char*)line,4095,fd);
      if (feof(fd)) break;
      
      if (line[strlen((char*)line)-1]=='\n')
        line[strlen((char*)line)-1] = 0x00;
      
      if (strlen((char*)line)==0)
        break;
      
      #ifdef _DEBUG
        printf("draqiePosDataRead line(%s)\n",(char*)line);
      #endif
      draqiePosDataParseLine(data,(unsigned char*)line);
    }
    fclose(fd);
  }  
    
  return 0;
}

int draqiePosDataWrite( struct draqiePosData *data, unsigned char *filename )
{
  if (!data || !filename)
    return 1;
  
  FILE *fd = NULL;
  unsigned char line[4096];
  unsigned char state[32];
  if ((fd = fopen((char*)filename,"w")))
  {
    struct draqiePosRequest *current = data->requests;
    while(current)
    {
      switch(current->state)
      {
        case _REQUEST_STATE_PENDING:
          strcpy((char*)state,"PENDING");
        break;
        case _REQUEST_STATE_ACCEPT:
          strcpy((char*)state,"ACCEPT");
        break;
        case _REQUEST_STATE_DONE:
          strcpy((char*)state,"DONE");
        break;
      }
      sprintf((char*)line,"%d,%d,%s,%s\n",
        current->deviceid,current->requestid,(char*)state,(char*)current->order);
      fwrite((void*)line,strlen((char*)line),1,fd);
      current = current->next;
    }
    
    fclose(fd);
  }  
    
  return 0;
}

/**
 * add a new request to the orders list
 */
int draqiePosDataAdd( struct draqiePosData *data, int deviceId, unsigned char *order )
{
  if (!data || !order)
    return 1;
  
  int highest = 0;
  struct draqiePosRequest *current = data->requests;
  while(current)
  {
    if (current->requestid>highest)
      highest = current->requestid;
    current = current->next;
  }
  highest++;
  
  struct draqiePosRequest *request;
  request = (struct draqiePosRequest*)malloc(sizeof(struct draqiePosRequest));
  request->deviceid = deviceId;
  request->requestid = highest;
  strcpy((char*)request->order,(char*)order);
  
  request->next = data->requests;
  data->requests = request;
  
  return 0;
}

/** remove a request from the list
 */
int draqiePosDataRemove( struct draqiePosData *data, int requestid )
{
  if (!data)
    return 1;
  
  struct draqiePosRequest *last = NULL;
  struct draqiePosRequest *next = NULL;
  struct draqiePosRequest *current = data->requests;  
  while(current)
  {
    if (current->requestid==requestid)
    {
      next = current->next;
      if (last)
        last->next = next;
      else
        data->requests = next;
      free(current);
    }
    else
      last = current;
    current = next;
  }
    
  return 0;
}
