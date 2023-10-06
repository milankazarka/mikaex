#include "rootSetup.h"
#include "mikaXml/tinyxml.h"
#include "mikaXml/tinystr.h"
#include "config.h"

rootSetup *rootSetupGlobal = NULL;

rootSetup::rootSetup( unsigned char *filename )
{
  myFilename = (unsigned char*)malloc(strlen((char*)filename)+1);
  strcpy((char*)myFilename,(char*)filename);
  rootSetupGlobal = this;
}

rootSetup::~rootSetup( )
{
  
}

int _getAttribute( TiXmlElement *element, unsigned char *name, unsigned char *value )
{
  if (!element || !name || !value)
  {
    return 1;
  }
  char *ename = (char*)element->Attribute("name");
  #ifdef _DEBUG
    printf("_getAttribute name(%s) searched(%s)\n",ename,(char*)name);
  #endif
  if (strcmp(ename,(char*)name)==0)
  {
    char *evalue = (char*)element->Attribute("value");
    #ifdef _DEBUG
      printf("_getAttribute value(%s)\n",evalue);
    #endif
    strcpy((char*)value,evalue);
  }
  else
  {
    return 1;
  }
  return 0;
}

int rootSetup::getAttribute( unsigned char *name, unsigned char *value )
{
  if (!name || !value)
    return 1;
  
  TiXmlDocument *doc;
  doc = new TiXmlDocument((const char*)myFilename);
  doc->LoadFile();
  TiXmlElement *root = doc->FirstChildElement("rootSetup");
  if (root)
  {
    TiXmlElement *eattributes = root->FirstChildElement("attributes");
    if (eattributes)
    {
      TiXmlElement *eattribute = eattributes->FirstChildElement("attribute");
      if (eattribute)
      {
        if ( _getAttribute(eattribute,name,value)==1 )
        {
          while(eattribute = eattribute->NextSiblingElement())
          {
            if ( _getAttribute(eattribute,name,value)==0 )
              break;
          }
        }
      }
    }
  }
  delete doc;  
  
  return 0;
}

rootSetup *getRootSetup( )
{
  if (!rootSetupGlobal)
    rootSetupGlobal = new rootSetup((unsigned char*)"root.xml");
  return rootSetupGlobal;
}

extern "C" {
int rootGetAttribute( unsigned char *name, unsigned char *value )
{
  if (!name || !value)
    return 1;
  
  rootSetup *s = getRootSetup();
  s->getAttribute(name,value);
  
  return 0;
}
}
