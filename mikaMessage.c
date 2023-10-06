/** this class provides a way how to work with XML data - load it
 *  parse it and process it
 */
#include "mikaMessage.h"
#include "config.h"

mikaMessage::mikaMessage( )
{
  messageBuffer[0] = 0x00;
}

mikaMessage::~mikaMessage( )
{
}

/** load a file's content to the 'messageBuffer'
 */
int mikaMessage::loadFileToBuffer( unsigned char *filename )
{
  if (!filename)
  {
    #ifdef _DEBUG
      printf("mikaMessage::loadFileToBuffer error invalid arguments\n");
    #endif
    return 1;
  }
  
  messageBuffer[0] = 0x00;
  strcpy((char*)messageBuffer,"");
  FILE *in = NULL;
  unsigned char line[_MIKA_MESSAGE_VALUELEN];
  if ((in = fopen((char*)filename,"r")))
  {
    while(1)
    {
      fgets((char*)line,(_MIKA_MESSAGE_VALUELEN-1)-1,in);
      if (feof(in)) break;
      
      strcat((char*)messageBuffer,(char*)line);
    }
  }
  
  #ifdef _DEBUG
    printf("mikaMessage::loadFileToBuffer buffer(\n%s\n)\n",
      (char*)messageBuffer);
  #endif
  
  return 0;
}

/** load a TiXmlDocument 'doc' out of a strMessage, or out of the messageBuffer
 */
int mikaMessage::loadString( unsigned char *strMessage )
{
  if (!strMessage)
  {
    #ifdef _DEBUG
      printf("mikaMessage::loadString warning strMessage empty, using messageBuffer\n");
    #endif
    strMessage = (unsigned char*)messageBuffer;
  }
  
  doc.Parse((const char*)strMessage,0,TIXML_ENCODING_UTF8);
  TiXmlElement *root = doc.RootElement();
  if (!root)
  {
    #ifdef _DEBUG
      printf("mikaMessage::loadString error parsing failed\n");
    #endif
    return 1;
  }
    
  return 0;
}

/** parse the 'doc' into a string message
 */
TiXmlElement *mikaMessageParseDOCToString( TiXmlElement *root, unsigned char *strMessage )
{
  static int n;
  static int depth = 0;
  TiXmlElement *child = NULL;
  static unsigned char elementName[_MIKA_MESSAGE_VALUELEN];
  static unsigned char elementAttrName[_MIKA_MESSAGE_VALUELEN];
  static unsigned char elementAttrValue[_MIKA_MESSAGE_VALUELEN];
  
  #ifdef _DEBUG
    if (depth==0)
      printf("mikaMessageParseDOCToString\n");
  
    for(n = 0; n < depth; n++)
      printf("\t");
    printf("<%s ",(char*)root->Value());
  #endif
  
  TiXmlAttribute *attribute = NULL;
  attribute = root->FirstAttribute();
  if (attribute)
  {
    #ifdef _DEBUG
      printf("%s=\"%s\" ",
        (char*)attribute->Name(),(char*)attribute->Value());
    #endif
    while(attribute = attribute->Next())
    {
      #ifdef _DEBUG
        printf("%s=\"%s\" ",
          (char*)attribute->Name(),(char*)attribute->Value());
      #endif
    }
  }
  
  #ifdef _DEBUG
    printf(">\n");
  #endif
  
  depth++;
  child = root->FirstChildElement();
  if (child)
  {
    mikaMessageParseDOCToString(child,strMessage);
    while(child = child->NextSiblingElement())
    {
      mikaMessageParseDOCToString(child,strMessage);
    }
  }
  depth--;
  
  #ifdef _DEBUG
    for(n = 0; n < depth; n++)
      printf("\t");
    printf("</%s>\n",(char*)root->Value());
  #endif
  
  return NULL;
}

int mikaMessage::generateString( unsigned char *strMessage )
{
  if (!strMessage)
  {
    #ifdef _DEBUG
      printf("mikaMessage::generateString warning strMessage null, using 'messageBuffer'\n");
    #endif
    strMessage = (unsigned char*)messageBuffer;
  }
  
  TiXmlElement *root = doc.RootElement();
  if (!root)
  {
    #ifdef _DEBUG
      printf("mikaMessage::generateString error no doc\n");
    #endif
    return 1;
  }
  else
  {
    mikaMessageParseDOCToString( root, strMessage );
  }
  
  return 0;
}

