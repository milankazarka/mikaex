#ifndef _LISTENER_H_
#define _LISTENER_H_

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
#include <netdb.h>
#include <pthread.h>

struct listenerDelegate {
  void *(*onListener)(int nid);
};

class listener {
public:
  int sockfd;
  struct listenerDelegate delegate;

  listener( );
  ~listener( );
  void loop( );
  void loopThreaded( );
};

#endif
