#ifndef _MIKA_VIDEO_WIDGET_H_
#define _MIKA_VIDEO_WIDGET_H_

#include "mikaWidget.h"
#include "mikaItem.h"
#include "config.h"

#define _SAMPLE_RATE 300000 // ns

class mikaVideoImageItem : public mikaItem {
  public:
    #ifdef _TR_SYSTEM
      struct tr_Object *image;
    #endif
  
    mikaVideoImageItem( unsigned char *path, int w, int h );
    ~mikaVideoImageItem( );
};

class mikaVideoWidget : public mikaWidget {
  public:
    mikaVideoImageItem *items;
    pthread_mutex_t smutex; // sampling mutex
    int bplay;
  
    mikaVideoWidget( struct mikaWidgetData *data );
    ~mikaVideoWidget( );
    
    int loadDirectory( unsigned char *directory );
    int addItem( mikaVideoImageItem *item );
    int showItem( mikaVideoImageItem *item );
    
    int play( );
    int stop( );
    int playThread( );
    int update( );
};

#endif
