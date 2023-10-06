#include "mikaServerPosDelegate.h"
#include "mikaServerMod.h"
#include "mikaServer.h"
#include "mikaPosServer.h"
#include "config.h"

#include <pthread.h>

static pthread_mutex_t posMutex;
static mikaPosServer *posServer = NULL;

struct mikaServerPosDelegate *mikaServerPosDelegateInit( struct mikaServerPosDelegate *posDelegate )
{
  if (!posDelegate)
    posDelegate = (struct mikaServerPosDelegate*)malloc(sizeof(struct mikaServerPosDelegate));
  posDelegate->onHandshake = mikaServerPosDelegateOnHandshake;
  
  if (!posServer)
    posServer = new mikaPosServer();
  
  pthread_mutex_init(&posMutex,NULL);
  
  return posDelegate;
}

/** register a new order at the POS server
 */
int mikaServerPosDelegateOnNewOrder( struct mikaServerMod *serverMod, struct mikaServer *server, struct mikaConnection *connection, TiXmlElement *command )
{
  if (!serverMod || !server || !connection || !command)
    return 1;
  
  #ifdef _DEBUG
    printf("mikaServerPosDelegateOnNewOrder\n");
  #endif
  
  posServer->newOrder(server,serverMod,connection,command);
  
  strcpy((char*)connection->msg->outBuffer,"<msg>\n");
  strcat((char*)connection->msg->outBuffer,"</msg>\n");
  
  return 0;
}

int mikaServerPosDelegateOnNewBill( struct mikaServerMod *serverMod, struct mikaServer *server, struct mikaConnection *connection, TiXmlElement *command )
{
  if (!serverMod || !server || !connection || !command)
    return 1;
  return 0;
}

int mikaServerPosDelegateOnModOrder( struct mikaServerMod *serverMod, struct mikaServer *server, struct mikaConnection *connection, TiXmlElement *command )
{
  if (!serverMod || !server || !connection || !command)
    return 1;
  
  posServer->modOrder(server,serverMod,connection,command);
  
  return 0;
}

int mikaServerPosDelegateOnRmOrder( struct mikaServerMod *serverMod, struct mikaServer *server, struct mikaConnection *connection, TiXmlElement *command )
{
  if (!serverMod || !server || !connection || !command)
    return 1;
    
  posServer->rmOrder(server,serverMod,connection,command);
  
  return 0;
}

int mikaServerPosDelegateOnFinBill( struct mikaServerMod *serverMod, struct mikaServer *server, struct mikaConnection *connection, TiXmlElement *command )
{
  if (!serverMod || !server || !connection || !command)
    return 1;
  return 0;
}

/** process the command for the POS server
 */
int mikaServerPosDelegateOnCommand( struct mikaServerMod *serverMod, struct mikaServer *server, struct mikaConnection *connection, TiXmlElement *command )
{
  static long run = 0;
  if (!serverMod || !server || !connection || !command)
  {
    #ifdef _DEBUG
      printf("mikaServerPosDelegateOnCommand error invalid arguments\n");
    #endif
    return 1;
  }
  
  char *type = (char*)command->Attribute("type");
  if (!type)
  {
    #ifdef _DEBUG
      printf("mikaServerPosDelegateOnCommand error command has no type\n");
    #endif
    return 1;
  }
  
  #ifdef _DEBUG
    printf("mikaServerPosDelegateOnCommand run(%d) type(%s)\n",run,type);
  #endif
  
  if (strcmp(type,"NewOrder")==0)
    mikaServerPosDelegateOnNewOrder(serverMod,server,connection,command);
  else if (strcmp(type,"NewBill")==0)
    mikaServerPosDelegateOnNewBill(serverMod,server,connection,command);
  else if (strcmp(type,"ModOrder")==0)
    mikaServerPosDelegateOnModOrder(serverMod,server,connection,command);
  else if (strcmp(type,"RmOrder")==0)
    mikaServerPosDelegateOnRmOrder(serverMod,server,connection,command);
  else if (strcmp(type,"FinBill"))
    mikaServerPosDelegateOnFinBill(serverMod,server,connection,command);
  
  #ifdef _DEBUG
    run++;
  #endif
  
  return 0;
}

void *mikaServerPosDelegateOnHandshake( void *myServerMod, void *myServer, void *myConnection )
{
  if (!myServerMod || !myServer || !myConnection)
  {
    #ifdef _DEBUG
      printf("mikaServerPosDelegateOnHandshake error invalid arguments\n");
    #endif
    return NULL;
  }
  
  #ifdef _DEBUG
    printf("mikaServerPosDelegateOnHandshake\n");
  #endif
  
  struct mikaServer *server = (struct mikaServer*)myServer;
  struct mikaServerMod *serverMod = (struct mikaServerMod*)myServerMod;
  struct mikaConnection *connection = (struct mikaConnection*)myConnection;
  
  connection->msg->outBuffer[0] = 0x00;
  
  TiXmlDocument doc;
  doc.Parse((const char*)connection->msg->inBuffer,0,TIXML_ENCODING_UTF8);
  TiXmlElement *msg = NULL;
  TiXmlElement *command = NULL;
  TiXmlElement *root = doc.FirstChildElement("msg");
  if (root)
    msg = root->FirstChildElement("mod");
  else return NULL;
  if (msg)
    command = msg->FirstChildElement("command");
  else return NULL;
    
  if (command)
  {
    mikaServerPosDelegateOnCommand(serverMod,server,connection,command);
  }
  else
  {
    #ifdef _DEBUG
      printf("mikaServerPosDelegateOnHandshake error <command> not found\n");
    #endif
    return NULL;
  }
  
  if (connection->msg->outBuffer[0]!=0x00)
    mikaConnectionClientPut(connection);
  
  return NULL;
}
