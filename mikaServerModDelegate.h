#ifndef _MIKA_SERVER_MOD_DELEGATE_H_
#define _MIKA_SERVER_MOD_DELEGATE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct mikaServerModDelegate {
  void *(*onHandshake)(void *myServerMod, void *myServer, void *myConnection);
} mikaServerModDelegate;

#endif
