#include "posSetup.h"
#include "config.h"

struct posSetup *posSetupInit( struct posSetup *ps )
{
  #ifdef _DEBUG
    printf("posSetupInit\n");
  #endif
  
  if (!ps)
    ps = (struct posSetup*)malloc(sizeof(struct posSetup));
  
  ps->server[0] = 0x00;
  ps->port = 0;
  
  return ps;
}

int posSetupConfigure( struct posSetup *ps, struct setupParser *parser, struct networkSetup *setup )
{
  #ifdef _DEBUG
    printf("posSetupConfigure\n");
  #endif
  
  if (!ps || !parser || !setup)
  {
    #ifdef _DEBUG
      printf("posSetupConfigure ERROR invalid arguments\n");
    #endif
    return 1;
  }
  
  TiXmlElement *root = parser->doc->FirstChildElement("setup");
  if (root)
  {
    TiXmlElement *pos = root->FirstChildElement("pos");
    if (pos)
    {
      char *server = (char*)pos->Attribute("server");
      if (server)
        strcpy((char*)ps->server,server);
      else
      {
        #ifdef _DEBUG
          printf("posSetupConfigure ERROR no server setup found\n");
        #endif
        return 1;
      }
      char *port = (char*)pos->Attribute("port");
      if (port)
        ps->port = atoi(port);
      else
      {
        #ifdef _DEBUG
          printf("posSetupConfigure ERROR no port found\n");
        #endif
        return 1;
      }
      
      if (
        ps->server[0]!=0x00 &&
        ps->port != 0
        )
      {
        printf("posSetupConfigure server(%s) port(%d)\n",
          (char*)ps->server,ps->port);
      }
    }
    else
    {
      #ifdef _DEBUG
        printf("posSetupConfigure ERROR no pos setup found\n");
      #endif
    }
  }
  
  return 0;
}
