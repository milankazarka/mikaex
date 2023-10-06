#include "langSetup.h"
#include "config.h"

unsigned char *langGet( struct langSetup *ls, unsigned char *key )
{
  #ifdef _DEBUG
    printf("langGet\n");
  #endif

  if (!ls || !key)
  {
    #ifdef _DEBUG
      printf("langGet ERROR invalid arguments\n");
    #endif
    return NULL;
  }

  unsigned char *value = NULL;
  
  struct langItem *current = ls->items;
  while(current)
  {
    if (
      strlen((char*)key)==strlen((char*)current->key)
    )
    {
      if (strcmp((char*)key,(char*)current->key)==0)
      {
        value = (unsigned char*)current->value;
        break;
      }
    }
    current = current->next;
  }
  
  if (!value)
    value = key;
  
  return value;
}

int langSetupProcessItem( struct langSetup *ls, TiXmlElement *element )
{
  #ifdef _DEBUG
    printf("langSetupProcessItem\n");
  #endif
  
  if (!ls || !element)
  {
    #ifdef _DEBUG
      printf("langSetupProcessItem ERROR invalid arguments\n");
    #endif
    return 1;
  }
  
  if (
    !element->Attribute("key") ||
    !element->Attribute("value")
    )
  {
    #ifdef _DEBUG
      printf("langSetupProcessItem ERROR invalid XML element\n");
    #endif
    return 1;
  }
  
  struct langItem *item = (struct langItem*)malloc(sizeof(struct langItem));
  strcpy((char*)item->key,element->Attribute("key"));
  strcpy((char*)item->value,element->Attribute("value"));
  item->next = ls->items;
  ls->items = item;
  
  return 0;
}

struct langSetup *langSetupInit( struct langSetup *ls, struct setupParser *parser )
{
  #ifdef _DEBUG
    printf("langSetupInit\n");
  #endif
  if (!parser)
  {
    #ifdef _DEBUG
      printf("langSetupInit ERROR invalid arguments\n");
    #endif
    return NULL;
  }
  
  if (!ls)
    ls = (struct langSetup*)malloc(sizeof(struct langSetup));
  
  ls->items = NULL;
  
  TiXmlElement *root = parser->doc->FirstChildElement("setup");
  if (root)
  {
    TiXmlElement *lang = root->FirstChildElement("lang");
    if (lang)
    {
      TiXmlElement *items = lang->FirstChildElement("items");
      if (items)
      {
        TiXmlElement *item = items->FirstChildElement("item");
        if (item)
        {
          langSetupProcessItem(ls,item);
          while(item = item->NextSiblingElement())
          {
            langSetupProcessItem(ls,item);
          }
        }
      }
    }
  }
  
  return ls;
}
