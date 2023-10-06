/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "draqieConfig.h"
#include "draqieSockets.h"

int draqieConfigGet( unsigned char *activity )
{
  if (!activity)
    return 1;
  
  struct draqieSockets *ds = draqieSocketsNew();
  ds->out[0] = 0x00;
  ds->in[0] = 0x00;
  sprintf((char*)ds->out,"<msg>\n<mod>\ndraqieConfig\n</mod>\n<config>\n%s\n<config>\n</msg>\n",
    (char*)activity);
  draqieSocketsSend(ds);
  if (ds->in[0]!=0x00)
  {
    #ifdef _DEBUG
      printf("draqieConfigGet response(%s)\n",(char*)ds->in);
    #endif
  }
  free(ds);
  
  return 0;
}
