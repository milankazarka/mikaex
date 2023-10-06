#include "mikaVideoWidget.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <dirent.h>

mikaVideoImageItem::mikaVideoImageItem( unsigned char *path, int w, int h )
  : mikaItem()
{
  if (!path)
  {
    #ifdef _DEBUG
      printf("mikaVideoImageItem::mikaVideoImageItem error, invalid path");
    #endif
    // error handling
  }
  
  #ifdef _DEBUG
    printf("mikaVideoImageItem::mikaVideoImageItem load(%s)\n",
      (unsigned char*)path);
  #endif
  #ifdef _TR_SYSTEM
    struct tr_Attr attr;
    attr.x = 0;
    attr.y = 0;
    attr.w = w;
    attr.h = h;
    image = tr_ImageInitEx(NULL,&attr,(char*)path);
  #endif
}

mikaVideoImageItem::~mikaVideoImageItem( )
{
}

mikaVideoWidget::mikaVideoWidget( struct mikaWidgetData *data )
  : mikaWidget(data)
{
  #ifdef _DEBUG
    printf("mikaVideoWidget::mikaVideoWidget\n");
  #endif
  items = NULL;
  bplay = 0;
  pthread_mutex_init(&smutex,NULL);
}

mikaVideoWidget::~mikaVideoWidget( )
{
}

int mikaVideoWidget::loadDirectory( unsigned char *directory )
{
  #ifdef _DEBUG
    printf("mikaVideoWidget::loadDirectory\n");
  #endif
  if (!directory)
  {
    #ifdef _DEBUG
      printf("mikaVideoWwidget::loadDirectory error, invalid directory\n");
    #endif
    return 1;
  }
  
  unsigned char path[512];
  struct dirent *dp = NULL;
  DIR *dir = opendir((char*)directory);
  if (dir)
  {
    while((dp=readdir(dir))!=NULL)
    {
      if (dp->d_name[0]=='.')
        continue;
        
      sprintf((char*)path,"%s/%s",(char*)directory,(char*)dp->d_name);  
      mikaVideoImageItem *item = new mikaVideoImageItem((unsigned char*)path,
        myData.attr.w,myData.attr.h);
      addItem(item);
    }
    closedir(dir);
  }
  else
  {
    #ifdef _DEBUG
      printf("mikaVideoWidget::loadDirectory error, couldn't read directory(%s)\n",
        (char*)directory);
    #endif
    return 1;
  }
  
  return 0;
}

int mikaVideoWidget::addItem( mikaVideoImageItem *item )
{
  if (!item)
    return 1;
  item->next = items;
  items = item;
  return 0;
}

void *mikaVideoWidgetPlayThread( void *data )
{
  mikaVideoWidget *vw = (mikaVideoWidget*)data;
  vw->playThread();
  return NULL;
}

int mikaVideoWidget::play( )
{
  #ifdef _DEBUG
    printf("mikaVideoWidget::play\n");
  #endif
  
  if (!items)
  {
    #ifdef _DEBUG
      printf("mikaVideoWidget::play error, no items to play\n");
    #endif
  }
  
  bplay = 1;
  pthread_t t;
  pthread_create(&t,NULL,mikaVideoWidgetPlayThread,this);
  return 0;
}

int mikaVideoWidget::stop( )
{
  #ifdef _DEBUG
    printf("mikaVideoWidget::stop\n");
  #endif
  pthread_mutex_lock(&smutex);
  bplay = 0;
  pthread_mutex_unlock(&smutex);
  return 0;
}

/** the update of the video is asynchronious, so we can basically have any
 *  sample rate as we wish, the video won't have lags is the system can't
 *  handle the speed, but there will be frames droped
 */
int mikaVideoWidget::playThread( )
{
  #ifdef _DEBUG
    printf("mikaVideoWidget::playThread\n");
  #endif
  while(1)
  {
    pthread_mutex_lock(&smutex);
    if (bplay==0)
    {
      pthread_mutex_unlock(&smutex);
      break;
    }
    pthread_mutex_unlock(&smutex);
    mikaVideoImageItem *current = items;
    while(current)
    {
      showItem(current);
      usleep(_SAMPLE_RATE);
      current = (mikaVideoImageItem*)current->next;
    }
    usleep(10000);
  }
  return 0;
}

int mikaVideoWidget::showItem( mikaVideoImageItem *item )
{
  if (!item)
  {
    #ifdef _DEBUG
      printf("mikaVideoWidget::showItem error, invalid item\n");
    #endif
    return 1;
  }
  
  #ifdef _TR_SYSTEM
    tr_AddImage((struct tr_Object*)getLow(),item->image);
  #endif
  update();
  
  return 0;
}

int mikaVideoWidget::update( )
{
  tr_WindowToTop(myData.window);
  tr_WindowExpose(myData.window);
  return 0;
}

