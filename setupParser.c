#include "setupParser.h"
#include "config.h"

struct networkSetup *currentSetup = NULL;

/** parse a <filename> object, retrieving it from the server
 */
int _parser_filename_node( struct setupParser *parser, TiXmlElement *filename )
{
  #ifdef _DEBUG
    printf("_parser_filename_node\n");
  #endif
  
  if (!parser || !filename)
  {
    #ifdef _DEBUG
      printf("_parser_filename_node ERROR invalid arguments\n");
    #endif
    return 1;
  }
  
  #ifdef _DEBUG
    printf("_parser_filename_node filename(%s)\n",filename->GetText());
  #endif
  
  if (currentSetup)
    networkSetupDownloadFile(currentSetup,(unsigned char*)filename->GetText());
  
  return 0;
}

struct setupParser *setupParserInit( struct setupParser *parser, struct networkSetup *setup )
{
  #ifdef _DEBUG
    printf("setupParserInit\n");
  #endif
  
  if (!setup)
  {
    #ifdef _DEBUG
      printf("setupParserInit ERROR invalid argument !setup\n");
    #endif
    return NULL;
  }
  
  currentSetup = setup;
  
  if (!parser)
    parser = (struct setupParser*)malloc(sizeof(struct setupParser));
  
  unsigned char content[4096*10];
  unsigned char line[512];
  FILE *fd = NULL;
  content[0] = 0x00;
  char *filename = NULL;
  
  if (setup->filename[0]==0x00)
    filename = (char*)"setup.xml";
  else
    filename = (char*)setup->filename;
  
  if (fd = fopen(filename,"r"))
  {
    strcpy((char*)content,"");
    while(1)
    {
      fgets((char*)line,512,fd);
      if (feof(fd))
        break;
      strcat((char*)content,(char*)line); 
      #ifdef _DEBUG
        printf("setupParserInit line(%s)\n",
          (char*)line);
      #endif
    }
    #ifdef _DEBUG
      printf("setupParserInit data(\n%s\n)\n",
        (char*)content);
    #endif
    fclose(fd);
  }
  else
  {
    #ifdef _DEBUG
      printf("setupParserInit ERROR could not open setup file\n");
    #endif
  }
  
  if (content[0]!=0x00)
  {
    parser->doc = new TiXmlDocument();
    parser->doc->Parse((const char*)content,0,TIXML_ENCODING_UTF8);
    
    // we go through the list of files that need to be fetched and we fetch them
    TiXmlElement *root = parser->doc->FirstChildElement("setup");
    if (root)
    {
      #ifdef _DEBUG
        printf("setupParserInit root element found\n");
      #endif
      
      TiXmlElement *filenames = root->FirstChildElement("filenames");
      if (filenames)
      {
        TiXmlElement *filename = NULL;
        TiXmlElement *nextfilename = NULL;
        filename = filenames->FirstChildElement("filename");
        if (filename)
        {
          _parser_filename_node(parser,filename);
          while (filename = filename->NextSiblingElement())
          {
            _parser_filename_node(parser,filename);
          }
        }
      }
    }
    else
    {
      #ifdef _DEBUG
        printf("setupParserInit ERROR couldn't find root element\n");
      #endif
    }
  }
  else
  {
    #ifdef _DEBUG
      printf("setupParserInit ERROR file content not retrieved\n");
    #endif
  }
  
  return parser;
}

/** get the number of screens in the setup
 */
int setupParserGetScreensCount( struct setupParser *parser, struct networkSetup *setup )
{
  #ifdef _DEBUG
    printf("setupParserGetScreensCount\n");
  #endif

  if (!parser || !setup)
  {
    #ifdef _DEBUG
      printf("setupParserGetScreensCount ERROR invalid arguments\n");
    #endif
    return 0;
  }

  if (!parser->doc)
  {
    #ifdef _DEBUG
      printf("setupParserGetScreensCount ERROR parser->doc NULL\n");
    #endif
    return 0;
  }

  int count = 0;
  
  TiXmlElement *root = parser->doc->FirstChildElement("setup");
  if (root)
  {
    TiXmlElement *screens = root->FirstChildElement("screens");
    if (screens)
    {
      TiXmlElement *screen = screens->FirstChildElement("screen");
      if (screen)
      {
        count++;
        while (screen = screen->NextSiblingElement())
          count++;
      }
    }
  }
  
  #ifdef _DEBUG
    printf("setupParserGetScreensCount count(%d)\n",count);
  #endif
  return count;
}

/** find out if a screen has the name 'name'
 */
int _is_screen_named( TiXmlElement *screen, unsigned char *name )
{
  if (!screen || !name)
  {
    #ifdef _DEBUG
      printf("_is_screen_named ERROR invalid arguments\n");
    #endif
    return 1;
  }
  
  char *attr = (char*)screen->Attribute("name");
  if (attr)
  {
    if (strcmp(attr,(char*)name)==0)
    {
      #ifdef _DEBUG
        printf("_is_screen_named HIT name(%s)\n",(char*)name);
      #endif
      return 1;
    }
  }
  
  return 0;
}

/** retrieve the TiXmlElement of a certain name
 */
TiXmlElement *setupParserGetScreenNamed( struct setupParser *parser, struct networkSetup *setup, unsigned char *name )
{
  #ifdef _DEBUG
    printf("setupParserGetScreenNamed\n");
  #endif

  if (!parser || !setup)
  {
    #ifdef _DEBUG
      printf("setupParserGetScreenNamed ERROR invalid arguments\n");
    #endif
    return NULL;
  }

  TiXmlElement *screen = NULL;
  
  TiXmlElement *root = parser->doc->FirstChildElement("setup");
  if (root)
  {
    TiXmlElement *screens = root->FirstChildElement("screens");
    if (screens)
    {
      TiXmlElement *current = screens->FirstChildElement("screen");
      if (current)
      {
        if (_is_screen_named(current,name)==1)
        {
          screen = current;
          return screen;
        }
        while (current = current->NextSiblingElement())
        {
          if (_is_screen_named(current,name)==1)
          {
            screen = current;
            return screen;
          }
        }
      }
    }
  }
  
  return screen;
}

TiXmlElement *setupParserGetScreenNamedAtRoot( struct setupParser *parser, struct networkSEtup *setup, unsigned char *name, TiXmlElement *root )
{
  #ifdef _DEBUG
    printf("setupParserGetScreenNamedAtRoot");
  #endif
  
  if (!parser || !setup || !name || !root)
  {
    #ifdef _DEBUG
      printf("setupParserGetScreenNamedAtRoot ERROR invalid arguments");
    #endif
    return NULL;
  }
  
  TiXmlElement *screen = NULL;
  
  TiXmlElement *screens = root->FirstChildElement("screens");
  if (screens)
  {
    TiXmlElement *current = screens->FirstChildElement("screen");
    if (current)
    {
      if (_is_screen_named(current,name)==1)
      {
        screen = current;
        return screen;
      }
      while (current = current->NextSiblingElement())
      {
        if (_is_screen_named(current,name)==1)
        {
          screen = current;
          return screen;
        }
      }
    }
  }
  
  #ifdef _DEBUG
    if (!screen)
      printf("setupParserGetScreenNamedAtRoot couldn't retrieve screen\n");
  #endif
  
  return screen;
}

/** retrieve an element of a 'name' of 'group' at 'index'
 */
TiXmlElement *setupParserGetElementAtIndex( struct setupParser *parser, struct networkSetup *setup, unsigned char *group, unsigned char *name, int index )
{
  int count = 0;
  TiXmlElement *item = NULL;
  
  TiXmlElement *root = parser->doc->FirstChildElement("setup");
  if (root)
  {
    TiXmlElement *items = root->FirstChildElement((char*)group);
    if (items)
    {
      TiXmlElement *current = items->FirstChildElement((char*)name);
      if (current)
      {
        if (count==index)
        {
          item = current;
          return item;
        }
        count++;
        while (current = current->NextSiblingElement())
        {
          if (count==index)
          {
            item = current;
            return item;
          }
          count++;
        }
      }
    }
  }
  
  return item;
}

/** get the element at index in the group 'root'
 */
TiXmlElement *setupParserGetElementAtIndexAtRoot( 
  struct setupParser *parser, struct networkSetup *setup, 
  unsigned char *named, int index, TiXmlElement *root 
  )
{
  int count = 0;
  TiXmlElement *item = NULL;
  
  TiXmlElement *current = root->FirstChildElement((char*)named);
  if (current)
  {
    if (count==index)
    {
      item = current;
      return item;
    }
    count++;
    while(current = current->NextSiblingElement())
    {
      if (count==index)
      {
        item = current;
        return item;
      }
      count++;
    }
  }
  
  return item;
}

/** retrieve the number of elements of 'name' in 'group'
 */
int setupParserGetElementCount( struct setupParser *parser, struct networkSetup *setup, unsigned char *group, unsigned char *name )
{
  #ifdef _DEBUG
    printf("setupParserScreenGetElementCount\n");
  #endif
  if (!parser || !setup || !group || !name)
  {
    #ifdef _DEBUG
      printf("setupParserScreenGetElementCount ERROR invalid arguments\n");
    #endif
    return 0;
  }

  int count = 0;
  
  TiXmlElement *root = parser->doc->FirstChildElement("setup");
  if (root)
  {
    TiXmlElement *items = root->FirstChildElement((char*)group);
    if (items)
    {
      TiXmlElement *item = items->FirstChildElement((char*)name);
      if (item)
      {
        count++;
        while (item = item->NextSiblingElement())
          count++;
      }
    }
  }
  
  return count;
}

/** retrieve the number of items in a screen object
 */
int setupParserScreenGetItemCount( struct setupParser *parser, struct networkSetup *setup, TiXmlElement *screen )
{
  #ifdef _DEBUG
    printf("setupParserScreenGetItemCount\n");
  #endif
  if (!parser || !setup || !screen)
  {
    #ifdef _DEBUG
      printf("setupParserScreenGetItemCount ERROR invalid arguments\n");
    #endif
    return 0;
  }

  int count = 0;
  
  TiXmlElement *items = screen->FirstChildElement("items");
  if (items)
  {
    TiXmlElement *item = items->FirstChildElement("item");
    if (item)
    {
      printf("\titem\n");
      count++;
      while (item = item->NextSiblingElement())
      {
        printf("\titem\n");
        count++;
      }
    }
  }
  
  return count;
}

/** get the n'th item of a screen
 */
TiXmlElement *setupParserScreenGetItemAtIndex( struct setupParser *parser, struct networkSetup *setup, TiXmlElement *screen, int index )
{
  #ifdef _DEBUG
    printf("setupParserScreenGetItemAtIndex\n");
  #endif
  if (!parser || !setup || !screen)
  {
    #ifdef _DEBUG
      printf("setupParserScreenGetItemAtIndex ERROR invalid arguments\n");
    #endif
    return NULL;
  }
  
  int count = 0;
  TiXmlElement *item = NULL;
  
  TiXmlElement *items = screen->FirstChildElement("items");
  if (items)
  {
    TiXmlElement *current = items->FirstChildElement("item");
    if (current)
    {
      if (count==index)
      {
        item = current;
        return item;
      }
      count++;
      while (current = current->NextSiblingElement())
      {
        if (count==index)
        {
          item = current;
          return item;
        }
        count++;
      }
    }
  }
  
  return item;
}
