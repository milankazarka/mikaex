/** copyright Mika Ltd.
 *  created by Milan Kazarka
 *  M.R.Stefanika 44/29, Ziar nad Hronom 965 01, Slovakia
 */
#ifndef _MIKA_COMM_AGNIS_H_
#define _MIKA_COMM_AGNIS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include "common.h"
#include "mikaHost.h"
#include "mikaNetRequest.h"

#define _MIKA_COMM_AGNIS_SMALL 64
#define _MIKA_COMM_AGNIS_LARGE 4096
#define _MIKA_COMM_AGNIS_DEFAULT_LANG "en"

/** Menu item as defined by Agnis POS
 *
 */
typedef struct mikaCommAgnisItem {
  unsigned char id[_MIKA_COMM_AGNIS_SMALL];		// ID
  unsigned char number[_MIKA_COMM_AGNIS_SMALL]; 	// ID in the Agnis POS
  unsigned char name[_MIKA_COMM_AGNIS_LARGE];		// name
  unsigned char shortcut[_MIKA_COMM_AGNIS_SMALL];	// short name
  unsigned char price[_MIKA_COMM_AGNIS_SMALL];	// price of 1 item
  unsigned char procdph[_MIKA_COMM_AGNIS_SMALL];	// tax
  unsigned char active[_MIKA_COMM_AGNIS_SMALL];	// active 0/1
  
  struct mikaCommAgnisItem *next;
} mikaCommAgnisItem;

class mikaCommAgnisSetup {
  public:
    unsigned char lang[_MIKA_COMM_AGNIS_SMALL];
    int htlusekId;
  
    mikaCommAgnisSetup( );
    ~mikaCommAgnisSetup( );
};

class mikaCommAgnisBill {
  public:
    int myId;
    int myRoom;
    int myTable;
    
    mikaCommAgnisBill( int id, int room, int table );
    ~mikaCommAgnisBill( );
};

class mikaCommAgnisOrder {
  public:
    mikaCommAgnisBill *myBill;
    mikaCommAgnisItem *myItem;
  
    mikaCommAgnisOrder( mikaCommAgnisBill *bill, mikaCommAgnisItem *item );
    ~mikaCommAgnisOrder( );
};

class mikaCommAgnis {
  public:
    mikaCommAgnisSetup *setup;
    struct mikaHost myHost;
    struct mikaCommAgnisItem *items;
    unsigned char requestBuffer[_KB*_MIKA_COMM_AGNIS_LARGE];
    unsigned char responseBuffer[_KB*_MIKA_COMM_AGNIS_LARGE];
    /** object through which we make all network requests
     */
    mikaNetRequest *netRequest;
  
    mikaCommAgnis( struct mikaHost *host );
    ~mikaCommAgnis( );
    
    int addItem( struct mikaCommAgnisItem *item );
    /** get the list of items registered at the Agnis POS
     */
    int getItemsList( );
    /** release the items
     */
    int releaseItemsList( );
    int parseItemsListData( unsigned char *xmlData );
    int parseItemXmlObject( TiXmlElement *xmlItem );
    /** find an attribute in the xmlObject and fill the buffer with it's value
     */
    int fillBufferFromXmlAttribute( TiXmlElement *xmlObject, unsigned char *attribute, unsigned char *buffer );
    int debugItems( );
    int orderItem( struct mikaCommAgnisOrder *order );
};

#endif
