#include "mikaSignageWidget.h"
#include "mikaSkinEx.h"
#include "config.h"

void *mikaSignageRunThread( void *data );

mikaSignageWidgetItem::mikaSignageWidgetItem( unsigned char *id, int w, int h, int type )
{
        if (id)
        {
                #ifdef _DEBUG
                        printf("mikaSignageWidgetItem::mikaSignageWidgetItem id(%s)\n",
                                (char*)id);
                #endif
                strcpy((char*)myId,(char*)id);
                myType = type;
                mw = w;
                mh = h;
                
                image = NULL;
                load();
        }
        else
        {
                // add error handling
        }
}

mikaSignageWidgetItem::~mikaSignageWidgetItem( )
{
}

/** load the item according to the id and type
 */
int mikaSignageWidgetItem::load( )
{
        switch(myType)
        {
                case _MIKA_SIGNAGE_WIDGET_IMAGE:
                        #ifdef _TR_SYSTEM
                                struct tr_Attr attr;
                                attr.x = 0;
                                attr.y = 0;
                                attr.w = mw;
                                attr.h = mh;
                                image = tr_ImageInitEx(NULL,&attr,(char*)myId);
                                if (!image)
                                {
                                        #ifdef _DEBUG
                                                printf("mikaSignageWidgetItem::load error, couldn't create image\n");
                                        #endif
                                }
                                
                        #endif
                        break;
                case _MIKA_SIGNAGE_WIDGET_VIDEO:
                        break;
        }
        return 0;
}

int mikaSignageWidgetItem::getType( )
{
        return myType;
}

mikaSignageWidget::mikaSignageWidget( struct mikaWidgetData *data )
: mikaWidget(data)
{
    #ifdef _DEBUG
        printf("mikaSignageWidget::mikaSignageWidget\n");
    #endif
    items = NULL;
    tr_SetColor(myData.widget,(char*)"#cecece");
    simpleClocktik = _DEFAULT_CLOCKTIK;
}

mikaSignageWidget::~mikaSignageWidget( )
{
    
}

int mikaSignageWidget::play( )
{
        #ifdef _DEBUG
                printf("mikaSignageWidget::play\n");
        #endif
        pthread_create(&thread,NULL,mikaSignageRunThread,this);
        return 0;
}

void mikaSignageWidget::onEvent( struct mikaEvent *event )
{
}

void mikaSignageWidget::onButtonDown( struct mikaEvent *event )
{
}

void mikaSignageWidget::onButtonUp( struct mikaEvent *event )
{
}

void *mikaSignageRunThread( void *data )
{
        #ifdef _DEBUG
                printf("mikaSignageRunThread\n");
        #endif
        if (!data)
        {
                #ifdef _DEBUG
                        printf("mikaSignageRunThread error, invalid data\n");
                #endif
                return NULL;
        }
        mikaSignageWidget *sw = (mikaSignageWidget*)data;
        sw->threadLoop();
        return NULL;
}

/** run the loop controlling the widget's content
 *
 */
void mikaSignageWidget::threadLoop( )
{
        #ifdef _DEBUG
                printf("mikaSignageWidget::threadLoop\n");
        #endif
        int c = 0;
        while(1)
        {
                #ifdef _DEBUG
                        printf("mikaSignageWidget::threadLoop cycle\n");
                #endif
                #ifdef _MIKA_SIGNAGE_WIDGET_TEST
                        if (c==0)
                        {
                                binarySemaphoreWait(bigInterface->activity.sem->id);
                                tr_SetColor(myData.widget,(char*)"#cecece");
                                binarySemaphorePost(bigInterface->activity.sem->id);
                                c++;
                        } else
                        {
                                binarySemaphoreWait(bigInterface->activity.sem->id);
                                tr_SetColor(myData.widget,(char*)"#303030");
                                binarySemaphorePost(bigInterface->activity.sem->id);
                                c--;
                        }
                        tr_WindowToTop(myData.window);
                        tr_WindowExpose(myData.window);
                #endif
                if (!items)
                        sleep(simpleClocktik);
                else
                {
                        // make fully thread safe, for adding items dynamically
                        //
                        mikaSignageWidgetItem *current = items;
                        if (current)
                        {
                                while(current)
                                {
                                        #ifdef _DEBUG
                                                printf("mikaSignageWidget::threadLoop cycle\n");
                                        #endif
                                        binarySemaphoreWait(bigInterface->activity.sem->id);
                        
                                        playItem(current);
                                        
                                        current = current->next;
                                        binarySemaphorePost(bigInterface->activity.sem->id);
                                        tr_WindowToTop(myData.window);
                                        tr_WindowExpose(myData.window);
                                
                                        sleep(simpleClocktik);
                                }
                        }
                        else
                                sleep(simpleClocktik);
                }
        }
}

int mikaSignageWidget::addItem( mikaSignageWidgetItem *item )
{
        #ifdef _DEBUG
                printf("mikaSignageWidget::addItem\n");
        #endif
        if (!item)
        {
                return 1;
        }
        binarySemaphoreWait(bigInterface->activity.sem->id);
        item->next = items;
        items = item;
        binarySemaphorePost(bigInterface->activity.sem->id);
        return 0;
}

/** load the items according to the playlist setup
 */
int mikaSignageWidget::setPlaylist( unsigned char *playlist )
{
        if (!playlist)
        {
                return 1;
        }
        return 0;
}

/** set the time each item is displayed in seconds
 *
 */
int mikaSignageWidget::setClocktik( int clocktik )
{
        simpleClocktik = clocktik;
        return 0;
}

int mikaSignageWidget::playItem( mikaSignageWidgetItem *item )
{
        if (!item)
        {
                #ifdef _DEBUG
                        printf("mikaSignageWidget::playItem error, invalid item\n");
                #endif
                return 1;
        }
        
        switch(item->getType())
        {
                case _MIKA_SIGNAGE_WIDGET_IMAGE:
                        #ifdef _TR_SYSTEM
                                tr_AddImage(myData.widget,item->image);
                                tr_ImagePosCenter(item->image,myData.widget);
                        #endif
                        break;
                case _MIKA_SIGNAGE_WIDGET_VIDEO:
                        break;
        }
        
        return 0;
}
