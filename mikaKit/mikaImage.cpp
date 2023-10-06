#include "mikaImage.h"
#include "mikaCommon.h"

#include "lowCommon.h"
#include "lowObjects.h"

mikaImage::mikaImage( unsigned char *filename, struct mikaFrame *frame )
{
  if (!filename || !frame)
  {
    #ifdef _DEBUG
      printf("mikaImage::mikaImage error invalid arguments\n");
    #endif
  }
  myFrame = *frame;
  lowImage = malloc(sizeof(struct mikaImageLow));
  struct mikaImageLow *dImageLow = (struct mikaImageLow*)lowImage;
  dImageLow->attr.x = frame->x;
  dImageLow->attr.y = frame->y;
  dImageLow->attr.w = frame->w;
  dImageLow->attr.h = frame->h;
  dImageLow->image = tr_ImageInitEx(NULL,&dImageLow->attr,(char*)filename);
  if (!dImageLow->image)
  {
    #ifdef _DEBUG
      printf("mikaImage::mikaImage error couldn't create image\n");
    #endif
  }
}

mikaImage::~mikaImage( )
{
}
