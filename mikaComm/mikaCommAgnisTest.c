#include "mikaCommAgnis.h"

int main( )
{
  mikaHost host;
  strcpy((char*)host.name,"localhost");
  host.port = 50000;
  mikaCommAgnis *agnis = new mikaCommAgnis(&host);
  agnis->getItemsList();
  
  if (agnis->items)
  {
    mikaCommAgnisBill *bill = new mikaCommAgnisBill(101,1,1);
    mikaCommAgnisOrder *order = new mikaCommAgnisOrder(bill,agnis->items);
    agnis->orderItem(order);
  }
  
  return 0;
}
