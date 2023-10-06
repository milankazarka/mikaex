#ifndef _MIKA_SIGNAGE_WIDGET_H_
#define _MIKA_SIGNAGE_WIDGET_H_

#include "mikaWidget.h"
#include <pthread.h>

#define _DEFAULT_CLOCKTIK 2
#define _MIKA_SIGNAGE_WIDGET_TEST

enum _MIKA_SIGNAGE_WIDGET_TYPES {
    _MIKA_SIGNAGE_WIDGET_IMAGE,
    _MIKA_SIGNAGE_WIDGET_VIDEO
};

class mikaSignageWidgetItem {
    public:
        mikaSignageWidgetItem *next;
        unsigned char myId[512];
        int myType;
        int mw;
        int mh;
    
        #ifdef _TR_SYSTEM
            struct tr_Object *image;
        #endif
    
        mikaSignageWidgetItem( unsigned char *id, int w, int h, int type );
        ~mikaSignageWidgetItem( );
        
        int load( );
        int getType( );
};

class mikaSignageWidget : public mikaWidget {
    public:
        pthread_t thread;
        int simpleClocktik;
        mikaSignageWidgetItem *items;
    
        mikaSignageWidget( struct mikaWidgetData *data );
        ~mikaSignageWidget( );
        
        // start the playback
        int play( );
        
        void onEvent( struct mikaEvent *event );
        void onButtonDown( struct mikaEvent *event );
        void onButtonUp( struct mikaEvent *event );
        
        void threadLoop( );
        int setPlaylist( unsigned char *filename );
        int setClocktik( int clocktik );
        int addItem( mikaSignageWidgetItem *item );
        int playItem( mikaSignageWidgetItem *item );
};

#endif
