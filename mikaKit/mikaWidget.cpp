/** copyright Mika Limited
 *
 */
#include "mikaWidget.h"
#include "mikaWindow.h"
#include "mikaCanvas.h"
#include "mikaScene.h"
#include "mikaImage.h"

#include "lowCommon.h"
#include "lowObjects.h"

mikaWidget::mikaWidget( dhandle canvas, struct mikaFrame *frame )
{
  #ifdef _DEBUG
    printf("mikaWidget::mikaWidget\n");
  #endif
  if (!canvas || !frame)
  {
    #ifdef _DEBUG
      printf("mikaWidget::mikaWidget invalid arguments\n");
    #endif
  }
  myFrame = *frame;
  parentCanvas = canvas;
  
  mikaCanvas *dCanvas = (mikaCanvas*)canvas;
  struct mikaCanvasLow *dCanvasLow = (struct mikaCanvasLow*)dCanvas->lowCanvas;
  lowWidget = malloc(sizeof(struct mikaWidgetLow));
  struct mikaWidgetLow *dWidgetLow = (struct mikaWidgetLow*)lowWidget;
  
  dWidgetLow->parentCanvas = dCanvasLow->canvas;
  dWidgetLow->attr.x = frame->x;
  dWidgetLow->attr.y = frame->y;
  dWidgetLow->attr.w = frame->w;
  dWidgetLow->attr.h = frame->h;
  tr_WindowToTop(dCanvasLow->parentWindow);
  dWidgetLow->widget = tr_ButtonInitEx2(NULL,&dWidgetLow->attr,dCanvasLow->canvas);
  tr_SetColor(dWidgetLow->widget,(char*)"#e0e0e0");
}

mikaWidget::~mikaWidget( )
{
}

/** set the widget's color
 */
int mikaWidget::setColor( unsigned char *colorCode )
{
  #ifdef _DEBUG
    printf("mikaWidget::setColor(%s)\n",(char*)colorCode);
  #endif
  if (!colorCode)
    return 1;
  
  struct mikaWidgetLow *dWidgetLow = (struct mikaWidgetLow*)lowWidget;
  mikaCanvas *dCanvas = (mikaCanvas*)parentCanvas;
  struct mikaCanvasLow *dCanvasLow = (struct mikaCanvasLow*)dCanvas->lowCanvas;
  tr_WindowToTop(dCanvasLow->parentWindow);
  tr_SetColor(dWidgetLow->widget,(char*)colorCode);
  
  return 0;
}

/** add an image, positioning it to the center
 */
int mikaWidget::addImage( dhandle image )
{
  if (!image)
    return 1;
  
  struct mikaWidgetLow *dWidgetLow = (struct mikaWidgetLow*)lowWidget;
  mikaCanvas *dCanvas = (mikaCanvas*)parentCanvas;
  struct mikaCanvasLow *dCanvasLow = (struct mikaCanvasLow*)dCanvas->lowCanvas;
  tr_WindowToTop(dCanvasLow->parentWindow);
  
  mikaImage *dImage = (mikaImage*)image;
  struct mikaImageLow *dImageLow = (struct mikaImageLow*)dImage->lowImage;
  tr_AddImage(dWidgetLow->widget,dImageLow->image);
  tr_ImagePosCenter(dImageLow->image,dWidgetLow->widget);
  
  return 0;
}
