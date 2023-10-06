/** copyright Mika Ltd.
 *  created by Milan Kazarka
 *  M.R.Stefanika 44/29, Ziar nad Hronom 965 01, Slovakia
 */
#include "mikaCommAgnis.h"

mikaCommAgnisSetup::mikaCommAgnisSetup( )
{
  htlusekId = 0;
  lang[0] = 0;
}

mikaCommAgnisSetup::~mikaCommAgnisSetup( )
{
}

mikaCommAgnis::mikaCommAgnis( struct mikaHost *host )
{
  if (!host)
  {
    #ifdef _DEBUG
      printf("mikaCommAgnis::mikaCommAgnis\n");
    #endif
  }
  else
  {
    myHost = *host;
  }
  setup = new mikaCommAgnisSetup();
  netRequest = new mikaNetRequest();
  requestBuffer[0] = 0x00;
  responseBuffer[0] = 0x00;
  items = NULL;
}

mikaCommAgnis::~mikaCommAgnis( )
{
  delete setup;
  delete netRequest;
}

/** add an item into the items list
 */
int mikaCommAgnis::addItem( struct mikaCommAgnisItem *item )
{
  #ifdef _DEBUG
    printf("mikaCommAgnis::addItem\n");
  #endif
  if (!item)
  {
    #ifdef _DEBUG
      printf("mikaCommAgnis::addItem error invalid arguments\n");
    #endif
    return 1;
  }
  
  item->next = items;
  items = item;
  
  return 0;
}

/** ask for the list of all items from the Agnis POS - the next step
 *  should be to save them to our internal list and generate a menu
 *  out of it
 */
int mikaCommAgnis::getItemsList( )
{
  #ifdef _DEBUG
    printf("mikaCommAgnis::getItemsList\n");
  #endif
  
  unsigned char tmp[_MIKA_COMM_AGNIS_LARGE];
  unsigned char lang[_MIKA_COMM_AGNIS_SMALL];
  
  if (setup->lang[0]!=0x00)
    strcpy((char*)lang,(char*)setup->lang);
  else
    strcpy((char*)lang,_MIKA_COMM_AGNIS_DEFAULT_LANG);
  
  strcpy((char*)requestBuffer,"<?xml version=\"1.0\" encoding=\"Windows-1250\" standalone=\"yes\"?>\n");
  sprintf((char*)tmp,
    "<REQUEST TYPE=\"5\" MODUL=\"PUB\" LANG=\"%s\">\n\
         \t<HTLUSEK ID=\"%d\" />\n\
    </REQUEST>\n\
    ",(char*)lang,setup->htlusekId
    );
  strcat((char*)requestBuffer,(char*)tmp);
  responseBuffer[0] = 0x00;
  netRequest->handshakeRequest(
    &myHost,(unsigned char*)requestBuffer,
    (unsigned char*)responseBuffer,_KB*_MIKA_COMM_AGNIS_LARGE);
  if (responseBuffer[0]!=0x00)
  {
    #ifdef _DEBUG
      printf("mikaCommAgnis::getItemsList response(%s)\n",
            (char*)responseBuffer);
    #endif
    parseItemsListData((unsigned char*)responseBuffer);
  }
  
  return 0;
}

/** release the items list
 */
int mikaCommAgnis::releaseItemsList( )
{
  #ifdef _DEBUG
    printf("mikaCommAgnis::releaseItemsList\n");
  #endif
  mikaCommAgnisItem *current = items;
  mikaCommAgnisItem *next = NULL;
  while(current)
  { 
    next = current->next;
    delete current;
    current = next;
  }
  return 0;
}

/** parse the list of items contained in an XML data buffer
 *
 *  this is the response from the Agnis POS containing the MENU items
 *
 
 <RESPONSE TYPE="1" MODUL="PUB" LANG="kod">
      <HTLUSEK ID="n">
                <POLOZKA CISLOPOLOZKY="n"       //kód pokladní položky
                  NAZEVPOLOZKY="c(40)"  	//název položky
                  ZKRATKA="c(10)"       	//zkrácený název položky
                  JEDNCENA="n"          	//jednotková cena
                  PROCDPH="n"  	                //procento DPH u položky
                  MAXPOVOLSLEVA="n" />  	//max.povolená sleva
                  TYPPOLOZKY="c(1)"             //typ položky
                  AKTIVNI="C(1)"                //zda je položka aktivní
                <POLOZKA ... />
                <POLOZKA>
                  ...
                </POLOZKA>
      </HTLUSEK>
  </RESPONSE>
  
 */
int mikaCommAgnis::parseItemsListData( unsigned char *xmlData )
{
  #ifdef _DEBUG
    printf("mikaCommAgnis::parseItemsListData\n");
  #endif
  if (!xmlData)
  {
    #ifdef _DEBUG
      printf("mikaCommAgnis::parseItemsListData error invalid argument\n");
    #endif
    return 1;
  }
  
  TiXmlDocument doc;
  doc.Parse((const char*)xmlData,0,TIXML_ENCODING_UTF8);
  TiXmlElement *root = doc.FirstChildElement("RESPONSE");
  if (!root)
  {
    #ifdef _DEBUG
      printf("mikaCommAgnis::parseItemsListData error RESPONSE object not found\n");
    #endif
    return 1;
  }
  
  TiXmlElement *htlusek = root->FirstChildElement("HTLUSEK");
  if (!htlusek)
  {
    #ifdef _DEBUG
      printf("mikaCommAgnis::parseItemsListData error HTLUSEK object not found\n");
    #endif
    return 1;
  }
  
  TiXmlElement *item = root->FirstChildElement("POLOZKA");
  if (item)
  {
    parseItemXmlObject(item);
    while(item = item->NextSiblingElement())
    {
      parseItemXmlObject(item);
    }
    debugItems();
  }
  else
  {
    #ifdef _DEBUG
      printf("mikaCommAgnis::parseItemsListData error no items in list\n");
    #endif
    return 1;
  }
  
  return 0;
}

/** fill the buffer with the value of an attribute from xmlObject
 */
int mikaCommAgnis::fillBufferFromXmlAttribute( TiXmlElement *xmlObject, unsigned char *attribute, unsigned char *buffer )
{
  #ifdef _DEBUG
    printf("mikaCommAgnis::fillBufferFromXmlAttribute\n");
  #endif
  if (!xmlObject || !attribute || !buffer)
  {
    #ifdef _DEBUG
      printf("mikaCommAgnis::fillBufferFromXmlAttribute error invalid arguments\n");
    #endif
    return 1;
  }
  
  char *value = (char*)xmlObject->Attribute((char*)attribute);
  if (value)
  {
    strcpy((char*)buffer,value);
  }
  else
  {
    #ifdef _DEBUG
      printf("mikaCommAgnis::fillBufferFromXmlAttribute warning couldn't find attribute %s\n",
            (char*)attribute);
    #endif
    return 1;
  }
  
  return 0;
}

/** parse the XML item and save it as an item in items for the Menu
 *
                <POLOZKA CISLOPOLOZKY="n"       //kód pokladní položky
                  NAZEVPOLOZKY="c(40)"  	//název položky
                  ZKRATKA="c(10)"       	//zkrácený název položky
                  JEDNCENA="n"          	//jednotková cena
                  PROCDPH="n"  	                //procento DPH u položky
                  MAXPOVOLSLEVA="n" />  	//max.povolená sleva
                  TYPPOLOZKY="c(1)"             //typ položky
                  AKTIVNI="C(1)"                //zda je položka aktivní
                <POLOZKA ... /> 
 */
int mikaCommAgnis::parseItemXmlObject( TiXmlElement *xmlItem )
{
  #ifdef _DEBUG
    printf("mikaCommAgnis::parseItemXmlObject\n");
  #endif
  if (!xmlItem)
  {
    #ifdef _DEBUG
      printf("mikaCommAgnis::parseItemXmlObject error invalid argument\n");
    #endif
    return 1;
  }
  
  struct mikaCommAgnisItem *item = (struct mikaCommAgnisItem*)malloc(sizeof(struct mikaCommAgnisItem));
  
  /** data is saved in generic buffers / converted real-time at a later stage
   */
  fillBufferFromXmlAttribute(xmlItem,(unsigned char*)"CISLOPOLOZKY",(unsigned char*)item->number);
  fillBufferFromXmlAttribute(xmlItem,(unsigned char*)"NAZEVPOLOZKY",(unsigned char*)item->name);
  fillBufferFromXmlAttribute(xmlItem,(unsigned char*)"ZKRATKA",(unsigned char*)item->shortcut);
  fillBufferFromXmlAttribute(xmlItem,(unsigned char*)"JEDNCENA",(unsigned char*)item->price);
  fillBufferFromXmlAttribute(xmlItem,(unsigned char*)"PROCDPH",(unsigned char*)item->procdph);
  fillBufferFromXmlAttribute(xmlItem,(unsigned char*)"AKTIVNI",(unsigned char*)item->active);
  
  addItem(item);
  
  return 0;
}

int mikaCommAgnis::debugItems( )
{
  mikaCommAgnisItem *current = items;
  while(current)
  {
    current = current->next;
  }
  return 0;
}

/** send an order of an item to the Agnis POS
 *
 <?xml version="1.0" encoding="Windows-1250" standalone="yes"?>
 <REQUEST TYPE="4" MODUL="PUB" LANG="kod">
      <HTLUSEK ID="n" />
                                                    //číslo účtu
      <OBJEDNAVKA          UCET="n"
                           MISTNOST="n"             //číslo místnosti
                           STUL="n">                //číslo stolu
             <POLOZKA ID="n(15)"                    //id.položky
                           CISLOPOLOZKY="n"         //kód pokladní položky
                           MNOZSTVI="n"             //množství
                           JEDNCENA="n"             //jednotková cena
                           POZNAMKA="c(30)"/>       //poznámka
             <POLOZKA ... />
      </OBJEDNAVKA>
 </REQUEST>

 */
int mikaCommAgnis::orderItem( struct mikaCommAgnisOrder *order )
{
  #ifdef _DEBUG
    printf("mikaCommAgnis::orderItem\n");
  #endif
  if (!order)
  {
    #ifdef _DEBUG
      printf("mikaCommAgnis::orderItem error invalid argument\n");
    #endif
    return 1;
  }
  
  if (!order->myBill || !order->myItem)
  {
    #ifdef _DEBUG
      printf("mikaCommAgnis::orderItem error invalid order\n");
    #endif
    return 1;
  }
  
  unsigned char tmp[_MIKA_COMM_AGNIS_LARGE];
  unsigned char lang[_MIKA_COMM_AGNIS_SMALL];
  
  if (setup->lang[0]!=0x00)
    strcpy((char*)lang,(char*)setup->lang);
  else
    strcpy((char*)lang,_MIKA_COMM_AGNIS_DEFAULT_LANG);
  
  strcpy((char*)requestBuffer,
    "<?xml version=\"1.0\" encoding=\"Windows-1250\" standalone=\"yes\"?>\n");
  sprintf((char*)tmp,
    "<REQUEST TYPE=\"4\" MODUL=\"PUB\" LANG=\"%s\">\n\
         <OBJEDNAVKA UCET=\"%d\" MISTNOST=\"%d\" STUL=\"%d\">\n\
          <POLOZKA ID=\"0\" CISLOPOLOZKY=\"%d\" MNOZSTVI=\"1\" JEDNCENA=\"%d\">\n\
          </POLOZKA>\n\
         </OBJEDNAVKA>\n\
    </REQUEST>\n\
    "
    ,(char*)lang,
    order->myBill->myId,
    order->myBill->myRoom,
    order->myBill->myTable,
    atoi((char*)order->myItem->number),
    atoi((char*)order->myItem->price)
    );
  strcat((char*)requestBuffer,(char*)tmp);
  responseBuffer[0] = 0x00;
  netRequest->handshakeRequest(
    &myHost,(unsigned char*)requestBuffer,
    (unsigned char*)responseBuffer,_KB*_MIKA_COMM_AGNIS_LARGE);
  
  return 0;
}

/** Bill identification for usage with Agnis POS
 *
 */
mikaCommAgnisBill::mikaCommAgnisBill( int id, int room, int table )
{
  myId 		= id;
  myRoom 	= room;
  myTable 	= table;
}

mikaCommAgnisBill::~mikaCommAgnisBill( )
{
}

/** Order identification for usage with Agnis POS
 */
mikaCommAgnisOrder::mikaCommAgnisOrder( mikaCommAgnisBill *bill, mikaCommAgnisItem *item )
{
  if (bill) myBill = bill;
  else      myBill = NULL;
  if (item) myItem = item;
  else      myItem = NULL;
  
}

mikaCommAgnisOrder::~mikaCommAgnisOrder( )
{
}

