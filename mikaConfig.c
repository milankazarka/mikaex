/*
 *    Copyright (C) 2010,2011 by Milan Kazarka
 *
 *    You may distribute under the terms of the Artistic License,
 *    as specified in the README file.
 *
 */

#include "mikaConfig.h"
#include "mikaSockets.h"

int mikaConfigGet( unsigned char *activity )
{
  if (!activity)
    return 1;
  
  struct mikaSockets *ds = mikaSocketsNew();
  ds->out[0] = 0x00;
  ds->in[0] = 0x00;
  sprintf((char*)ds->out,"<msg>\n<mod>\nmikaConfig\n</mod>\n<config>\n%s\n<config>\n</msg>\n",
    (char*)activity);
  mikaSocketsSend(ds);
  if (ds->in[0]!=0x00)
  {
    #ifdef _DEBUG
      printf("mikaConfigGet response(%s)\n",(char*)ds->in);
    #endif
  }
  free(ds);
  
  return 0;
}
