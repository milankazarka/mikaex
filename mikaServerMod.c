/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "mikaServerMod.h"
#include "mikaServer.h"
#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"
#include "config.h"

#define _SERVER_MOD_STATIC_NO
#define _SERVER_MOD_CHECK_MESSAGES

struct mikaServerMod *mikaServerModInit( )
{
  struct mikaServerMod *myServerMod = (struct mikaServerMod*)malloc(sizeof(struct mikaServerMod));
  
  myServerMod->mode = _SERVER_MOD_GENERIC;
  myServerMod->processData = NULL;
  myServerMod->id[0] = 0x00;
  myServerMod->next = NULL;
  myServerMod->delegate = NULL;
  
  return myServerMod;
}

void *_mika_server_mod_process_generic( unsigned char *in, unsigned char *out )
{
  if (!in || !out)
    return NULL;
  
  #ifdef _DEBUG
    printf("_mika_server_mod_process_generic\n");
  #endif
  
  return NULL;
}

int mikaServerModRunMessage( struct mikaServerMod *myServerMod, unsigned char *msg, unsigned char *response )
{
  if (!myServerMod)
    return 1;
  
  if (!myServerMod->processData)
    return 1;  
  
  myServerMod->processData(msg,response);
    
  return 0;
}

/** check if a message should be parsed by a MOD
 */
int mikaServerModIsMessage( struct mikaServerMod *myServerMod, unsigned char *msg )
{
  #ifndef _SERVER_MOD_CHECK_MESSAGES
    return 1;
  #endif
  
  if (!myServerMod || !msg)
    return 1;
  
  if (myServerMod->id[0] == 0x00)
    return 1;  

  #ifdef _DEBUG
    printf("mikaServerModIsMessage(%s/%s)\n",(char*)myServerMod->id,msg);
  #endif
  
  TiXmlDocument doc;
  doc.Parse(  	
      (const char*)msg,
      0,
      TIXML_ENCODING_UTF8	 
  );
  TiXmlElement *root = doc.FirstChildElement("msg");
  if (root)
  {
    TiXmlElement *id = root->FirstChildElement( "mod" );
    if (id)
    {
      #ifdef _DEBUG
        #ifdef _SERVER_EX
          if (id->Attribute("name"))
            printf("mikaServerModIsMessage mod(%s)\n",id->Attribute("name"));
          else
          {
            #ifdef _DEBUG
              printf("mikaServerModIsMessage error 'name' attribute missing\n");
            #endif
            return 1;
          }
        #else
          printf("mikaServerModIsMessage mod(%s)\n",id->GetText());
        #endif
      #endif
      
      #ifdef _SERVER_EX
        if (id->Attribute("name"))
          if (strcmp(id->Attribute("name"),(char*)myServerMod->id)==0);
        else
        {
          #ifdef _DEBUG
            printf("mikaServerModIsMessage error 'name attribute missing'\n");
          #endif
          return 1;
        }
      #else
        if (strcmp(id->GetText(),(char*)myServerMod->id)==0)
      #endif
      {
        return 0;
      }
    }
  }
    
  return 1;
}

int mikaServerModConnection( struct mikaServerMod *myServerMod, void *myServer, void *myConnection )
{
  if (!myServerMod || !myServer || !myConnection)
  {
    #ifdef _DEBUG
      printf("mikaServerModConnection error invalid arguments\n");
    #endif
    return 1;
  }

  struct mikaServer *server = (struct mikaServer*)myServer;
  struct mikaConnection *connection = (struct mikaConnection*)myConnection;
  
  return 0;
}

#ifdef _SERVER_MOD_STATIC
  int main( )
  {
    struct mikaServerMod *mod = mikaServerModInit();
    sprintf((char*)mod->id,"<msg>");
    mikaServerModIsMessage(mod,(unsigned char*)"<msg><mod>some</mod></msg>");
    return 0;
  }
#endif
