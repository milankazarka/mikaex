#ifndef _MIKA_CE_H_
#define _MIKA_CE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "mikaCeObject.h"
#include "mikaCeParcel.h"
#include "mikaCeNetwork.h"

class mikaCe {
  public:
    mikaCeObject *objects;
    mikaCeParcel *parcels;
    mikaCeNetworkListener *listener;
  
    pthread_mutex_t mutex;
  
    mikaCe( );
    ~mikaCe( );
    
    void lock( );
    void unlock( );
    void threadClocktik( );
    void runEngine( );
    void listObjects( );
    int pushObject( mikaCeObject *object );
    int addObject( mikaCeObject *object );
    int mvObject( mikaCeObject *object, struct mikaFrame *frame );
    int rmObject( mikaCeObject *object );
    mikaCeObject *objectAtId( char *id );
    int addParcel( mikaCeParcel *parcel );
    int listObjectsForParcel( mikaCeParcel *parcel );
    struct mikaFrame frameOfObjectInParcel( mikaCeParcel *parcel, mikaCeObject *object );
};

extern mikaCe *dce;

#endif
