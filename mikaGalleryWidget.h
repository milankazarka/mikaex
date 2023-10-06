#ifndef _MIKA_GALLERY_WIDGET_H_
#define _MIKA_GALLERY_WIDGET_H_

#include "mikaWidget.h"
#include "config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <dirent.h>

#define _MIKA_GALLERY_WIDGET_ARROWS_NO

enum _MIKA_GALLERY_ITEM_TYPES {
  _MIKA_GALLERY_IMAGE
};

class mikaGalleryItem {
  public:
    unsigned char myId[512];
    int myType;
    int mw;
    int mh;
    mikaGalleryItem *prev;
    mikaGalleryItem *next;
    #ifdef _TR_SYSTEM
      struct tr_Object *image;
    #endif
  
    mikaGalleryItem( unsigned char *id, int w, int h, int type );
    ~mikaGalleryItem( );
    
    int load( );
};

class mikaGalleryWidgetButton : public mikaWidget {
  public:
    mikaObject *gallery;
    
    mikaGalleryWidgetButton( struct mikaWidgetData *data );
    mikaGalleryWidgetButton( struct mikaWidgetData *data, mikaWidget *parent );
    ~mikaGalleryWidgetButton( );
    
    void onEvent( struct mikaEvent *event );
    void onButtonDown( struct mikaEvent *event );
    void onButtonUp( struct mikaEvent *event );
};

class mikaGalleryWidget : public mikaWidget {
  public:
    mikaGalleryItem *items;
    
    mikaGalleryItem *topItem;
    mikaGalleryItem *currentItem;
    
    mikaGalleryWidgetButton *prev;
    mikaGalleryWidgetButton *next;
  
    mikaGalleryWidget( struct mikaWidgetData *data );
    ~mikaGalleryWidget( );
    
    void onEvent( struct mikaEvent *event );
    void onButtonDown( struct mikaEvent *event );
    void onButtonUp( struct mikaEvent *event );
    
    int addItem( mikaGalleryItem *item );
    // load all items from a directory
    int loadItemsDirectory( unsigned char *path );
    int update( ); // update with the current gallery item
    
    int onControl( mikaGalleryWidgetButton *button );
    int onNext( );
    int onPrev( );
};

#endif
