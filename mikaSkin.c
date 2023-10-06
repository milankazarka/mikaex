#include "mikaSkin.h"
#include "config.h"
#include "mikaXml/tinystr.h"
#include "mikaXml/tinyxml.h"
#include "gui.h"

struct mikaSkin *skinInstance = NULL;

/** read a node from the skin file
 */
extern "C" int mikaSkinGlobalSaveNode( TiXmlElement *root, char *name )
{
  TiXmlElement *element = NULL;
  char *attr = NULL;

  if (!root || !name)
    return 1;
  
  element = root->FirstChildElement(name);
  if (element)
  {
    attr = (char*)element->Attribute("color");
    if (attr)
    {
      struct mikaSkinNode *node = (struct mikaSkinNode*)malloc(sizeof(struct mikaSkinNode));
      node->next = skinInstance->nodes;
      skinInstance->nodes = node;
      strcpy((char*)node->color.hex,attr);
      strcpy((char*)node->name,name);
      
      unsigned char r;
      unsigned char g;
      unsigned char b;
      unsigned char a;
      mikaSkinGlobalNodeGetColor(node,&r,&g,&b,&a);
    }
  }
  
  return 0;
}

extern "C" int mikaSkinGlobalLoadStandardSet( )
{
  TiXmlElement *root = (TiXmlElement*)skinInstance->xmlRoot;
  TiXmlElement *element = NULL;
  
  mikaSkinGlobalSaveNode(root,"mikaBackground");
  mikaSkinGlobalSaveNode(root,"mikaButton");
  mikaSkinGlobalSaveNode(root,"mikaListbox");
  mikaSkinGlobalSaveNode(root,"mikaCanvas");
  mikaSkinGlobalSaveNode(root,"mikaCanvasButtons");
  
  return 0;
}

/** list the nodes of the skin
 */
extern "C" int mikaSkinGlobalList( )
{
  struct mikaSkinNode *current = skinInstance->nodes;
  while(current)
  {
    #ifdef _DEBUG
      printf("mikaSkinGlobalInit node(%s) color(%s)\n",
        (char*)current->name,(char*)current->color.hex);
    #endif
    current = current->next;
  }
  return 0;
}

extern "C" int mikaSkinGlobalInit( )
{
  skinInstance = (struct mikaSkin*)malloc(sizeof(struct mikaSkin));
  skinInstance->nodes = NULL;
  
  TiXmlDocument *doc = new TiXmlDocument((const char*)_SKIN_PATH);
  if (doc)
  {
    bool loading = doc->LoadFile();
    if (!loading)
      return 1;
  }
  else
    return 1;
    
  skinInstance->xmlRoot = (void*)doc->FirstChildElement("mikaSkin");
  if (skinInstance->xmlRoot)
  {
    mikaSkinGlobalLoadStandardSet( );
  }
  else
  {
    #ifdef _DEBUG
      printf("mikaSkinGlobalInit error <mikaSkin> not present\n");
    #endif
    return 1;
  }
  
  mikaSkinGlobalList();
  
  return 0;
}

/** retrieve a node of a name
 */
extern "C" struct mikaSkinNode *mikaSkinGlobalGetNode( unsigned char *name )
{
  struct mikaSkinNode *node = NULL;
  struct mikaSkinNode *current = NULL;
  current = skinInstance->nodes;
  while(current)
  {
    if (strcmp((char*)current->name,(char*)name)==0)
    {
      node = current;
      break;
    }
    current = current->next;
  }
  
  return node;
}

extern "C" int mikaSkinGlobalNodeGetColor( struct mikaSkinNode *node, unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a )
{
  _hex2unsignedChar((unsigned char*)node->color.hex+1,r,g,b,a);
  #ifdef _DEBUG
    printf("mikaSkinGlobalNodeGetColor rgba(%2x%2x%2x)\n",
      *r,*g,*b);
  #endif
  return 1;
}
