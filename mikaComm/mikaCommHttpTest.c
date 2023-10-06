#include "mikaCommHttp.h"

int main( )
{
  struct mikaHost host;
  
  struct postAttribute *attributes = NULL;
  strcpy((char*)host.name,"http://draquie-sk.bluegray.eu/sys/getXml.php");
  host.port = 80;
  
  struct postAttribute *attribute = (struct postAttribute*)malloc(sizeof(struct postAttribute));
  attribute->next = attributes;
  attributes = attribute;
  strcpy((char*)attribute->key,"id_appliance");
  strcpy((char*)attribute->value,"671659754298bg65jhf7fr56hg4fd54c");
  
  mikaCommHttp *commHttp = new mikaCommHttp(&host);
  commHttp->sendRequest(attributes);
  
  strcpy((char*)host.name,"http://www.adaluna.com/sys/img.php?id=23.jpg");
  mikaCommHttp *commHttpBinary = new mikaCommHttp(&host);
  commHttpBinary->sendRequest(NULL);
  commHttpBinary->saveResponse((unsigned char*)"testfile.jpg");
  
  return 0;
}
