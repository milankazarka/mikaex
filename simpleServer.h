#ifndef _SIMPLE_SERVER_H_
#define _SIMPLE_SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <netdb.h>

#include <pthread.h>

#define SIMPLE_TXT 4096

enum  STATES {
  STATE_NEW,
  STATE_ACCEPTED,
  STATE_PREPAIRED,
  STATE_SERVED,
  STATE_FINISHED
};

typedef struct simpleOrder
{
  unsigned char name[SIMPLE_TXT];
  unsigned char state[SIMPLE_TXT];
  struct simpleOrder *next;
} simpleOrder;

static struct simpleOrder *orders;

#endif
