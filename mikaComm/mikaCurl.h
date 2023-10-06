#ifndef _MIKA_CURL_H_
#define _MIKA_CURL_H_

#include "mikaNetRequest.h"

class mikaCurl {
  public:
    mikaCurl( );
    ~mikaCurl( );
};

void mikaCurlInit( );
void mikaCurlDeinit( );

extern mikaCurl *mikaCurlInstance;

#endif
